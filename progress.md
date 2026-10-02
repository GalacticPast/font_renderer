# Progress Notes

Working notes for picking this project back up without re-deriving context.
See `AGENTS.md` for the project's permanent rules/roadmap — this file is the
"where did I leave off" log, not a spec. Update it as things change; delete
stale sections rather than letting them rot.

## Current direction

Implementing the **real Slug algorithm (Lengyel's technique)** via an SSBO of
glyph curve data, with the actual inside/outside test done analytically in
the fragment shader (ray-vs-quadratic-curve crossing count, even-odd fill
rule) — not the earlier Loop-Blinn fan/FBO prototype, which was deliberately
deleted. OpenGL is still explicitly **temporary**; the plan is to move to
Vulkan once the Slug algorithm itself works, so keep effort weighted toward
the API-agnostic parts (contour/curve extraction, the curve math) over
GL-specific plumbing.

## Milestone reached: single glyph renders correctly, holes and all

Pressing nothing (default glyph index 0 = 'A') now renders a correct,
solid-filled "A" via the full SSBO → fragment-shader pipeline: no debug
line-strip, no CPU tessellation — the GPU is doing the actual curve-coverage
test. Verified visually (`shader.jpg` screenshots) to have the right
silhouette, correct triangular counter (hole) via the even-odd rule, and no
stray artifacts, after fixing a nasty precision bug (see below).

## How the pipeline works right now

- `load_font` (`src/main.c`) parses all 26 uppercase letters via
  `stbtt_GetGlyphShape` into `glyph_data.curves` (flat array of `{p0, p1
  (control), p2}`, `db_vector4` each — lines are stored as degenerate
  quadratics with `p1` = exact midpoint of `p0`/`p2`).
- `center_glyphs` converts curve points from font design units to pixel
  space (`em_to_px_scale`) and centers each glyph on its own bbox. It also
  now stores `glyph_data.half_extent = (max - min) / 2` (pixel space), used
  to size the glyph's quad.
- One quad (`vertices`/`indices` in `main.c`, a plain `[-0.5, 0.5]` square)
  is scaled per-frame by `camera_set_matrix`'s `model` matrix using
  `half_extent * 2` so its world-space footprint matches the *currently
  selected* glyph's bbox exactly. `view` and `projection` are a flat
  identity-view orthographic setup (1 world unit = 1 pixel).
- The vertex shader (`vertex.glsl`) passes the quad's world-space position
  through as `frag_pos` — a glyph-local, pixel-space coordinate that lines
  up directly with the curve data's own coordinate system.
- `glyphs.data[0].curves` (currently hardcoded to glyph 0 = 'A') is uploaded
  once via `ssbo_create(&b_ssbo, 1, glyphs.data[0].curves.data,
  sizeof(curve), glyphs.data[0].curves.length)` to SSBO binding point `1`,
  matching `layout(std430, binding = 1)` in both shaders.
- The fragment shader (`fragment.glsl`) does the real Slug test: for each
  curve, `horiz_ray_roots` solves `B_y(t) = frag_pos.y` (handling the
  degenerate-line case separately from the general quadratic), then
  `curve_x_at` evaluates `B_x(t)` at each valid root and counts it as a
  crossing if it's ahead of `frag_pos.x`. Odd total crossings = inside
  (filled white); even = `discard`.

## Bug fixed this session: epsilon too tight on the degenerate-line check

`horiz_ray_roots` detects "this curve is actually a straight line" via
`abs(a) < epsilon` (where `a = p0.y - 2*p1.y + p2.y`, exactly `0` in theory
for a line). The curve points go through `em_to_px_scale` multiply +
centering subtraction before reaching the shader, and at pixel-space
magnitudes (hundreds), accumulated float32 rounding pushed some nominally-
exact-zero `a` values to around `1e-5` — past the original `1e-6` threshold.
Those curves fell through to the full quadratic formula, where dividing by
a near-zero `a` causes catastrophic cancellation and wildly unstable roots —
visible as a small jagged notch cut into the solid fill near wherever a
curve happened to accumulate enough rounding error. **Fixed** by widening
the epsilon to `1e-3` in both the `a` and `b` checks in `fragment.glsl`.

Diagnosis method worth remembering: when GPU output looks locally wrong but
the overall shape is right, write a standalone CPU simulation (plain
Python, no GPU/driver involved) of the exact same math against the exact
curve data. If the simulation renders clean, the bug is GPU-side (precision,
upload, binding) — not the algorithm or the data. That's what pinned this
one down after visual guessing (shared-vertex theories, etc.) didn't pan
out.

## Known gaps / not yet done

1. **The SSBO is only ever uploaded once, for glyph 0 ('A').** Pressing `D`
   cycles the selected glyph index `i` and *does* now correctly resize the
   quad to that glyph's own `half_extent` — but the curve data in the SSBO
   is never re-uploaded, so every letter still renders using 'A's curves
   inside a correctly-sized-for-that-letter quad. This is the next concrete
   bug to fix (see Next Steps).
2. **Only real-curve (non-degenerate, `STBTT_vcurve`) rendering is
   unverified.** 'A' happens to be built entirely out of straight lines
   (every curve in it is a degenerate line), so the full quadratic-formula
   branch of `horiz_ray_roots` has never actually been exercised visually.
   Needs testing on a glyph with genuine curves (O, S, B, G) before trusting
   it — and specifically re-checking that the widened `1e-3` epsilon doesn't
   misclassify a real, gently-curved segment as a degenerate line.
3. **No anti-aliasing yet** (Phase 7) — fill is a hard binary in/out test,
   so diagonal edges show ordinary un-anti-aliased staircasing. Expected at
   this stage; don't chase it before more glyphs are verified correct.
4. **Nested/multiple holes untested** — only verified one hole (the "A"
   counter). Per the roadmap's testing progression, still need to check
   B/8/@ for correctly handling more than one nested contour.
5. Minor cleanup items, not urgent: `ssbo_bind`/`ssbo_unbind`/`ssbo_delete`
   (`gl.c`) are stub/no-op or arguably wrong (`ssbo_bind` calls
   `glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)`, which unbinds rather than
   binds — harmless right now since the binding that actually matters is
   the `glBindBufferBase` call already done in `ssbo_create`, but the
   function doesn't do what its name says). No GL object cleanup
   (VAO/VBO/EBO/SSBO) at shutdown, only `shader_destroy`.
6. `STBTT_vcubic` case in `load_font` is an empty no-op — fine for
   TrueType-only fonts (no cubics expected) but worth a comment if it stays
   silently empty.

## Next steps, smallest-first

1. **Fix glyph cycling to actually show the right letter.** When `D` is
   pressed and `i` changes, re-upload (or `glBufferSubData`) that glyph's
   `curves` into the SSBO, not just resize the quad. Simplest first pass:
   just call `ssbo_create` again each time `i` changes (recreates the
   buffer; fine for now, revisit if churn becomes a problem).
2. **Verify real curves work**, not just lines: once cycling works, step
   through to a glyph with actual `STBTT_vcurve` segments (O, S, or G) and
   visually confirm the curved edges render smoothly and correctly through
   the quadratic-formula branch of `horiz_ray_roots`.
3. **Verify multiple/nested holes** on B, 8, or @ per the testing
   progression in `AGENTS.md`.
4. Once single-glyph rendering is solid across that test range, move to
   **Phase 7 (anti-aliasing)** — don't start this before step 2/3 are clean,
   per "correctness before optimization."
5. Longer term (Phase 8 territory): pool multiple glyphs into one shared
   SSBO with per-glyph `curve_start`/`curve_count` uniform ranges (the
   design already discussed) to support rendering more than one glyph/draw
   call at once — needed for actual text strings, advances, and batching.
