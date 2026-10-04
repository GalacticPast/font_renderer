# Progress Notes

Working notes for picking this project back up without re-deriving context.
See `AGENTS.md` for the project's permanent rules/roadmap — this file is the
"where did I leave off" log, not a spec. Update it as things change; delete
stale sections rather than letting them rot.

## Current direction

Real Slug algorithm (Lengyel, JCGT 2017): one shared SSBO of all glyphs'
curves, one horizontal ray per fragment, signed (nonzero) winding number with
fractional anti-aliased contributions. OpenGL is still temporary (Vulkan
planned once Slug itself works).

Phases 1–7 are done (glyph extraction, curve representation, debug rendering,
GPU data, GPU curve evaluation, fill determination, anti-aliasing). Phase 8
(text layout) is next.

## Milestone: anti-aliased, signed-winding fill

Replaced even-odd parity + 2×2 grid supersampling with the paper's analytic
method, in `fragment.glsl`:

- `horiz_ray_roots` finds up to two roots of `B_y(t) = ray_y` in `[0, 1)`.
- `curve_y_slope_at` gives `d(B_y)/dt` at a root; its sign sets the winding
  contribution (`y` decreasing → +1, increasing → −1).
- `signed_coverage` returns `winding_sign * f`, with
  `f = clamp((crossing_x - frag_pos.x) + 0.5, 0, 1)`. Curve data is already in
  pixel space, so the paper's `m` (pixels-per-em) term is implicitly 1.
- `main()` sums contributions, `alpha = clamp(abs(coverage), 0, 1)`, and
  discards only when alpha is exactly 0.

Verified visually (`bin/slug`, Wayland):
- `A`: solid fill, inner triangular counter renders as background, edges show
  real multi-level gray ramps (not hard 0/255).
- `S`: fills correctly; vertical-ish edges are smooth.
- `B`: both counters render as holes, no sign inversion.

Blending is enabled in `main.c` (`GL_BLEND`, `GL_SRC_ALPHA`,
`GL_ONE_MINUS_SRC_ALPHA`) — required, or the fractional alpha is ignored.

## How it works (current architecture)

- `load_font` (`src/main.c`) parses all 26 letters into one pooled
  `glyphs.curves` array; each `glyph_data` records its own
  `[curves_start_index, curves_end_index)` slice.
- `center_glyphs` converts each glyph's slice to pixel space
  (`em_to_px_scale`) and re-centers it on its own bbox; also computes
  `half_extent = (max - min) / 2`.
- One reusable quad is scaled per-frame to `half_extent * 2` so its
  world-space footprint matches whichever glyph is selected.
- `main()` uploads the entire pooled `glyphs.curves` to one SSBO
  (`binding = 1`) once, and sets the `curve_indicies` uniform per-frame to
  tell the fragment shader which slice belongs to the current glyph.
- `vertex.glsl` passes the quad's world-space position through as `frag_pos`
  (pixel-space, orthographic projection 1:1 with window pixels).

## Known gaps / accepted limitations

1. **Anisotropic AA.** Smoothing only happens along the ray direction (x).
   Near-horizontal edges and tangent points stay jagged (measured on the top
   of `S`'s inner curve: ~one hard jump instead of a ramp). Accepted for now.
   Fix would be a second ray direction or supersampling, as the paper suggests.
2. **`RAY_Y_NUDGE`** (`fragment.glsl`) is still a probabilistic workaround for
   rays passing exactly through shared vertices. The paper solves this exactly
   with a sign-based lookup table (`0x2E74`, 8 equivalence classes) — worth
   revisiting.
3. **Winding sign unverified against the font's contour direction in general.**
   The `y decreasing → +1` choice works for A/S/B, so flip both signs in
   `signed_coverage` if a future glyph renders inverted.
4. Only uppercase A–Z loaded; no lowercase, digits, punctuation, layout, or
   batching.
5. `center_glyphs` re-centers each glyph on its own bbox, which discards the
   baseline-relative position that Phase 8 layout needs.
6. Minor cleanup, not urgent: `ssbo_bind`/`ssbo_unbind`/`ssbo_delete` (`gl.c`)
   are stub/no-op or arguably wrong (`ssbo_bind` unbinds rather than binds —
   harmless since the binding that matters is `glBindBufferBase` in
   `ssbo_create`). No GL object cleanup (VAO/VBO/EBO/SSBO) at shutdown, only
   `shader_destroy`.
7. `STBTT_vcubic` case in `load_font` is an empty no-op — fine for TrueType.

## Bug history (reusable lessons)

**Ray-through-shared-vertex bands.** Symptom: thin horizontal bands across
otherwise-correct glyphs. Cause: the crossing test is ambiguous when the ray
height equals a vertex shared by two curves, and font coordinates are round
numbers so exact hits are common. Epsilon-tuning and vertex-ownership rules
each fixed one letter and broke another. Current workaround: nudge the ray
height by `RAY_Y_NUDGE = 0.0317` (see gap 2).

**Diagnosis method:** dump the real curve data for the broken glyph and replicate
the ray-crossing math in a standalone Python script, no GPU. If the simulation
is clean, the bug is GPU-side (precision, upload, binding); if it reproduces,
it's in the math/data and iterates much faster outside the edit-build-run loop.

## Next steps, smallest-first

1. **Phase 8 design first.** Decide the baseline/origin convention and how
   `center_glyphs` changes (gap 5). Check `stb_truetype.h` comments on
   `stbtt_GetGlyphShape` and `stbtt_GetGlyphHMetrics` for the exact coordinate
   semantics rather than assuming them.
2. Extract advance width and left side bearing per glyph
   (`stbtt_GetGlyphHMetrics`), converted with `em_to_px_scale`.
3. Extend `camera_set_matrix` (or the model matrix) with a per-glyph pen
   translation, in addition to the existing scale.
4. Milestone: two glyphs side by side with correct spacing from real advances.
   Then a full string, then kerning (`stbtt_GetCodepointKernAdvance`), then
   batching (one draw call for many glyphs).
