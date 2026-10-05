# Progress Notes

Working notes for picking this project back up without re-deriving context.
See `AGENTS.md` for the project's permanent rules/roadmap — this file is the
"where did I leave off" log, not a spec. Update it as things change; delete
stale sections rather than letting them rot.

## Current direction

Real Slug algorithm (Lengyel, JCGT 2017): one shared SSBO of all glyphs'
curves, analytic ray-crossing coverage with fractional anti-aliasing, signed
winding. OpenGL is still temporary (Vulkan planned once Slug itself works).

Phases 1–7 are done. Phase 8 (text layout) is in progress: the glyph data now
lives in em space and each glyph is placed on its own ink box. The pen
position (advance, origin) is not wired up yet.

## Status

Committed (`5217b4e`, "now everything lives in em space"):
- Curves are converted from font units to em once, at load time
  (`funit_to_em = stbtt_ScaleForMappingEmToPixels(font, 1.0)`, i.e. `1/unitsPerEm`).
- Curves are relative to the glyph origin (pen position on the baseline).
  Nothing is centered at load time anymore.
- `text_compute_glyph_bbox` (`text.c`) computes a tight ink box per glyph in
  em, stored as `aabb` (`center`, `half_size`). It checks the endpoints plus
  the turning point of each curve on each axis.

Working tree (uncommitted, in progress):
- `vertex.glsl`: `frag_pos` is the em-space position (matches the curves).
  `gl_Position` multiplies by `font_px` (pixels per em) to place the vertex.
- `fragment.glsl`: the anti-aliasing ramp and the ray nudge are converted to em
  with `font_px`. A vertical ray has been added next to the horizontal one, and
  the two are averaged. This is the experiment for the jagged edges below.
- `main.c`: the model matrix is `translate(aabb.center) × scale(2 × half_size)`,
  built in `camera_set_matrix`. `font_px` is set per frame (currently 200 in
  the working copy; check it before trusting any screenshot).

Not visually verified on the current working tree.

## Earlier milestone: anti-aliased, signed-winding fill

Replaced even-odd parity + 2×2 grid supersampling with the paper's analytic
method, in `fragment.glsl`:

- `horiz_ray_roots` finds up to two roots of `B_y(t) = ray_y` in `[0, 1)`.
- `curve_y_slope_at` gives `d(B_y)/dt` at a root; its sign sets the winding
  contribution (`y` decreasing → +1, increasing → −1).
- Each crossing contributes `winding_sign * f`, with `f` a ramp of the
  crossing's x-distance from the pixel center, saturated to [0, 1].
- The result is `alpha = clamp(abs(coverage), 0, 1)`; discard only when alpha
  is exactly 0.

Verified visually earlier (before em-space): `A` fills with a real gray ramp at
its edges, `S` fills and its vertical-ish edges are smooth, `B` has both
counters as holes.

Blending is enabled in `main.c` (`GL_BLEND`, `GL_SRC_ALPHA`,
`GL_ONE_MINUS_SRC_ALPHA`) — required, or the fractional alpha is ignored.

## How it works (current architecture)

- `text_load_font` (`src/text.c`) parses codepoints `'!'`..`'~'` into one
  pooled `glyphs.curves` array. Each `glyph_data` records its own
  `[curves_start_index, curves_end_index)` slice and its `aabb`. Index `i`
  in `data[]` is the codepoint offset from `'!'`.
- The loader converts curves to em, computes the ink boxes, then uploads the
  whole pool to one SSBO (`binding = 1`) once. The state is kept in a static
  `text_state`, and `text_load_font` returns a pointer to it.
- `main.c` draws one glyph per frame. It sets `curve_indicies` to the glyph's
  slice, `font_px` to the size, and the model matrix from its `aabb`.
- `vertex.glsl` passes the glyph-space position through as `frag_pos`, and
  multiplies by `font_px` only for `gl_Position`.

## Known gaps / accepted limitations

1. **Anisotropic AA (open).** Each ray only gives a ramp for edges roughly
   perpendicular to it. A horizontal ray gives a hard step on near-horizontal
   edges, such as the top of `S`'s bowls. The averaged vertical ray is an
   attempt at this, but the result is not confirmed.
   The paper (JCGT 2017, p. 38) says averaging several ray directions gives
   "greater isotropy, but at a performance cost", and it treats the two
   axis-aligned rays as a good compromise. The paper does not say to use a max
   or a per-pixel selection. Reliability weighting per crossing is my own idea,
   not from the paper.
2. **`RAY_NUDGE_PX`** (`fragment.glsl`) is still a probabilistic workaround for
   rays passing exactly through shared vertices. It's expressed in pixels and
   divided by `font_px`. The paper solves this exactly with a sign-based lookup
   table (`0x2E74`, 8 equivalence classes), which is worth revisiting.
3. **Winding sign.** Horizontal rays count `y` decreasing as +1. Vertical rays
   count `x` increasing as +1. Both agree for a clockwise outer contour, and
   that matches the `A`, `S`, and `B` results. Flip both if a glyph renders
   inverted.
4. **Epsilons are in em.** `EPSILON` (`1.5e-5`) and the linear-case checks
   haven't been rechecked at very small or very large `font_px`.
5. **Empty glyphs.** `text_compute_glyph_bbox` has no guard for glyphs with no
   curves, such as space. Their box would hold `MAX`/`MIN` values. The loaded
   range `'!'`..`'~'` has no empty glyphs, so this doesn't show up yet.
6. **No pen position yet.** Glyphs all sit with their origin at the window
   center. Advances and kerning are not extracted.
7. **Only one font** and one glyph draw per frame. No batching.
8. Minor cleanup, not urgent: `ssbo_bind`/`ssbo_unbind`/`ssbo_delete` (`gl.c`)
   are stub/no-op or arguably wrong (`ssbo_bind` unbinds rather than binds —
   harmless since the binding that matters is `glBindBufferBase` in
   `ssbo_create`). No GL object cleanup (VAO/VBO/EBO/SSBO) at shutdown, only
   `shader_destroy`.
9. `STBTT_vcubic` case in the loader is an empty no-op — fine for TrueType.

## Bug history (reusable lessons)

**Ray-through-shared-vertex bands.** Symptom: thin horizontal bands across
otherwise-correct glyphs. Cause: the crossing test is ambiguous when the ray
height equals a vertex shared by two curves, and font coordinates are round
numbers so exact hits are common. Epsilon-tuning and vertex-ownership rules
each fixed one letter and broke another. Current workaround: nudge the ray
height (gap 2).

**Moving the quad broke the curve test.** Translating the glyph with the model
matrix made `frag_pos` (taken from `world_pos`) leave the curves' coordinate
space, so the ray test ran against the wrong place and the glyph vanished. The
fix is to keep `frag_pos` in glyph space and apply screen placement only to
`gl_Position`. The quad itself is positioned in glyph space with the model
matrix.

**Mixing units.** Epsilons and the AA ramp were in pixels. Switching the curves
to em made them wrong by the font size. Any constant in the shader has to say
which unit it's in.

**Diagnosis method:** dump the real curve data for the broken glyph and replicate
the ray-crossing math in a standalone Python script, no GPU. If the simulation
is clean, the bug is GPU-side (precision, upload, binding); if it reproduces,
it's in the math/data and iterates much faster outside the edit-build-run loop.

## Next steps, smallest-first

1. **Settle the jagged-edge question on `S`.** Output the horizontal and
   vertical sums on their own, one image each, and compare them to the averaged
   result. Then decide: keep the average (the paper's method), or try
   reliability weighting. Record the choice here.
2. **Pen position.** Extract advance width and left side bearing per glyph
   (`stbtt_GetGlyphHMetrics`), converted with `funit_to_em`. Check the stb header
   comments on those functions first.
3. **Pen in the vertex shader.** Add the pen (in em) before the `font_px`
   multiply, so `gl_Position = (glyph_pos + pen) × font_px`. Keep the pen out of
   the model matrix so `frag_pos` stays in glyph space.
4. **Milestone: two glyphs side by side** with spacing from real advances. Then
   a full string, then kerning (`stbtt_GetCodepointKernAdvance`).
5. **Batching.** One instanced draw for many glyphs: a per-glyph record
   (slice, box, pen, size) read via `gl_InstanceID`, passed to the fragment
   shader with `flat` varyings.
6. Guard empty glyphs in `text_compute_glyph_bbox` (gap 5) before adding
   spaces to the string.
