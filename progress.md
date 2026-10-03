# Progress Notes

Working notes for picking this project back up without re-deriving context.
See `AGENTS.md` for the project's permanent rules/roadmap — this file is the
"where did I leave off" log, not a spec. Update it as things change; delete
stale sections rather than letting them rot.

## Current direction

Real Slug algorithm (Lengyel's technique): one shared SSBO of all glyphs'
curves, analytic ray-vs-quadratic-curve crossing test in the fragment
shader, even-odd fill rule. OpenGL is still temporary (Vulkan planned once
Slug itself works).

## Milestone: multiple glyphs render correctly, cycling works

All 26 uppercase letters are pooled into one `db_array_curves` on `glyphs`
(not per-glyph arrays anymore). Each `glyph_data` stores
`curves_start_index`/`curves_end_index` (absolute indices into the shared
pool) plus `half_extent`. Pressing `D` cycles the selected glyph and now
correctly re-sizes the quad *and* re-points the fragment shader at that
glyph's own curve range via the `curve_indicies` uniform — both the quad and
the curve data now follow the same `i`. Verified clean on A, S, Q, O, M, J
(mix of straight-line-only, round, and hook/tail glyphs).

## How it works (current architecture)

- `load_font` (`src/main.c`) parses all 26 letters into one pooled
  `glyphs.curves` array; each `glyph_data` records its own
  `[curves_start_index, curves_end_index)` slice.
- `center_glyphs` converts each glyph's slice to pixel space
  (`em_to_px_scale`) and centers it on its own bbox; also computes
  `half_extent = (max - min) / 2`.
- One reusable quad is scaled per-frame to `half_extent * 2` so its
  world-space footprint matches whichever glyph is selected.
- `main()` uploads the *entire* pooled `glyphs.curves` to one SSBO
  (`binding = 1`) once, and sets the `curve_indicies` uniform
  (`vec2(start_index, end_index)`) per-frame to tell the fragment shader
  which slice belongs to the current glyph.
- `vertex.glsl` passes the quad's world-space position through as
  `frag_pos` (pixel-space, matching the curve data's coordinate system).
- `fragment.glsl` does the real test: for each curve in
  `[curve_indicies.x, curve_indicies.y)`, solve `B_y(t) = ray_y`
  (`horiz_ray_roots`), evaluate `B_x(t)` for each valid root
  (`curve_x_at`), count it as a crossing if it's ahead of `frag_pos.x`.
  Odd crossings = inside (fill white); even = `discard`.

## Bug fixed this session: ray-through-shared-vertex, fixed via ray nudge

Symptom: thin horizontal bands cutting across otherwise-correct glyphs (seen
on S, O, Q, M, J at different heights each).

Root cause: the ray-crossing test is ambiguous exactly when the ray height
equals a vertex shared by two adjacent curves — one curve should "own" that
crossing, the other shouldn't, or the parity flips for every pixel in that
row. Font curve coordinates are suspiciously round numbers, so exact vertex
hits are common here, not a rare edge case.

Dead ends tried first: widening the discriminant-near-zero threshold, and an
asymmetric "lower-y endpoint owns the vertex" rule with its own epsilon.
Each fix solved whichever letter was being tested and broke a different one
— a sign that epsilon-tuning was the wrong approach (the right threshold
depends on each glyph's specific geometry).

**Actual fix**: nudge the ray height by a small fixed, non-round offset
(`RAY_Y_NUDGE = 0.0317` in `fragment.glsl`) before doing any curve math, so
landing exactly on a vertex becomes vanishingly unlikely. This let all the
special-case vertex-ownership logic be deleted in favor of the plain,
standard half-open interval rule (`t >= 0.0 && t < 1.0`).

Diagnosis method worth reusing: extract the real curve data for the broken
glyph (temporary `printf` dump in `load_font`/after `center_glyphs`) and
replicate the exact ray-crossing math in a standalone Python script, no GPU
involved. If the simulation is clean, the bug is GPU-side (precision,
upload, binding); if the simulation reproduces it, the bug is in the
math/data and can be iterated on much faster outside the GL/shader
edit-build-run loop.

## Known gaps / not yet done

1. **No anti-aliasing yet** (Phase 7) — binary in/out fill, so diagonal
   edges show ordinary un-anti-aliased staircasing. Expected at this stage.
2. Only uppercase A-Z loaded; no lowercase, digits, punctuation, kerning,
   layout, or batching (Phase 8 territory).
3. Minor cleanup, not urgent: `ssbo_bind`/`ssbo_unbind`/`ssbo_delete`
   (`gl.c`) are stub/no-op or arguably wrong (`ssbo_bind` unbinds rather
   than binds — harmless since the binding that matters is the
   `glBindBufferBase` call in `ssbo_create`). No GL object cleanup
   (VAO/VBO/EBO/SSBO) at shutdown, only `shader_destroy`.
4. `STBTT_vcubic` case in `load_font` is an empty no-op — fine for
   TrueType-only fonts (no cubics expected).

## Next steps, smallest-first

1. **Finish verifying all 26 letters** cycle cleanly (A/S/Q/O/M/J already
   confirmed; worth a quick pass through the rest, especially ones with
   multiple holes like B).
2. Test glyphs with **more than one hole** (B, 8-equivalent if digits get
   added, @) to confirm nested-contour even-odd fill still holds up.
3. Once that's solid, move to **Phase 7 (anti-aliasing)** — not before.
4. Longer term: extend the pooled-SSBO/`curve_indicies` design (already in
   place) to render more than one glyph per draw call, for actual text
   strings — advances, kerning, batching (Phase 8).
