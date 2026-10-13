# Progress Notes

Working notes for picking this project back up without re-deriving context.
See `AGENTS.md` for the project's permanent rules/roadmap — this file is the
"where did I leave off" log, not a spec. Update it as things change; delete
stale sections rather than letting them rot.

Last refreshed: 2026-10-13, at `fe39278`.

## Current direction

Real Slug algorithm (Lengyel, JCGT 2017): one shared SSBO of all glyphs'
curves, banded per glyph, analytic ray-crossing coverage with signed winding.
OpenGL is still temporary (Vulkan planned once Slug itself works).

The Slug renderer is done for 2D UI text (see below; kerning and cubics are
deferred). The active track is **renderer batching**, so quads/shapes and text
can interleave in push order, which the UI's floating windows need.

## Status

Working:
- Glyphs `' '`..`'~'` loaded from `assets/font/Archivo-Regular.ttf` into one
  pooled curve array, converted to em once at load. Lines become degenerate
  quadratics (control = midpoint). Empty glyphs (space) are skipped in
  `text_compute_glyph_bbox`.
- Per-glyph tight ink box, 8 horizontal + 8 vertical bands, band curve index
  lists in two SSBOs (`binding = 2`, `3`). Curves at `binding = 1`.
- Fragment shader: paper's contribution table (`0x2E74`), root formulas incl.
  the linear case, crossing fraction ramp, horizontal + vertical rays, 3
  samples per axis, nonzero fill via `abs`.
- Vertex shader: half-pixel dilation of the glyph quad in em (`0.5 / font_px`).
- Layout: advances, ascent/descent/line gap, `\n`, `\t` (4 spaces), wrapping by
  `wrap_width`, `text_calculate_size`. Non-ASCII bytes render as `?`.
- One instanced draw for all glyphs of the frame. Instance layout is the
  `glyph_instance` struct (`text.h`); stride and offsets come from
  `sizeof`/`offsetof`, each `mat4` linked as 4 vec4 columns.
- Logical → physical (display scale) conversion lives in `renderer.c`.

Working tree (uncommitted), `text_fragment.glsl` — items 1–3 below, verified
visually at 11–120 px (`SVWAEHT sow @g8B (ilj) ...`):
- No `t` range checks in `ray_contribution`.
- Band picked per sample ray (`band_index(ray_y / ray_x, ...)`).
- Weighted dual-ray combination (`crossing_weight`, `combine_rays`).
- `SAMPLES_PER_AXIS 1`: looks the same as 3 with the weighting; only curved
  and diagonal edges differ at all. The old shader left a faint gray cross
  inside the 14 px `A`; the new one is clean.

**Build is currently broken:** `main.c:67` calls `renderer_batch()`, but
`renderer.h` renamed the macro to `renderer_layer_batch()`.

## Slug: done for 2D UI text

Fixed (see working tree above):

1. **`t` range check.** The table already guarantees a counted root is in
   range; `t >= 0 && t < 1` rejected the exact-endpoint case (curve ending with
   `v2 == 0` loses its `t = 1` crossing → wrong winding).
2. **Band chosen per pixel, not per sample.** Supersampled rays moved ±1/3 px
   (≈0.014 em at 24 px) while band padding is `1/1024` em, so rays near a band
   boundary missed curves. The band now comes from each sample's ray.
3. **Dual-ray combination.** `0.5 * (h + v)` kept half of the hard step on
   edges running along one ray. Each ray now carries a weight
   (`1 - 2 * |distance_px|` of its nearest crossing, clamped); coverage is the
   weighted mean of `min(|h|, 1)` and `min(|v|, 1)`, floored by the smaller of
   the two for pixels with no crossing within half a pixel.

Still worth a look: sweep the font size slowly on `E`, `H`, `T` for specks on
flat edges, and `NUMBER_OF_BANDS` 32 at 12–16 px for band-boundary lines.

Later / not blocking:
- `STBTT_vcubic` is an empty case: CFF `.otf` fonts would silently lose
  curves. Split cubics into quadratics, or at least fail loudly.
- Dilation `0.5 / font_px` is only right for unrotated, uniformly scaled
  screen-space text. World-space (3D) text needs dilation from the full MVP.
- Kerning (`stbtt_GetGlyphKernAdvance`; note stb only reads `kern`, not
  `GPOS`), UTF-8 / non-ASCII glyphs, multiple fonts.
- Perf: sort each band's curves for early exit; band count per glyph instead of
  a fixed 8. Only after measuring.
- Blending is in gamma space; linear-space blending may change perceived
  weight.

## Renderer batching: design + state

Design (agreed):
- **The renderer draws in push order; the UI pushes back to front.** The UI's
  `ui_get_render_commands()` list already gives that order, so its loop maps
  to `renderer_set_clip` + `renderer_push_*` per element.
- Subsystems (`text`, future `shape`) own an instance array + upload + draw
  range + reset. `renderer.c` owns the batch list, current clip, scissor and
  logical → physical.
- Batch = `{ type, clip, first_instance, instance_count }`. `first_instance`
  indexes **that type's** instance array. A push extends the last batch if
  type and clip match, otherwise starts a new one; zero-instance pushes add
  nothing. Measure instance count as array length before/after the push.
- Flush: upload each type once, then per batch `glScissor` (flip y:
  `physical_height - (y + h)`) and
  `glDrawElementsInstancedBaseInstance(..., count, first_instance)`.
  `gl_InstanceID` does not include the base instance.
- One `shape` pipeline for rect / circle / line (SDF in the fragment shader,
  circle = rounded rect with radius = half size) so shapes don't break batches.

State: `db_render_batch` and a fixed `batches[12]` exist in `renderer`;
`renderer_start_layer`/`renderer_end_layer` are stubs; `renderer_push_quad`
is empty; `text_flush` still draws everything in one call. Not needed by the
design above: the layer scope macro and the `bit1`/`bit2` flag enum (a batch
has exactly one type).

Next: split `text_flush` into upload / draw range / reset, add the batch list
logic, test with text only (same clip → 1 batch, different clips → 2, spaces
only → 0), then add `shape`.

## Known cleanup

- `ssbo_bind` binds buffer `0` instead of `ssbo_object->id` (`gl.c`).
  Harmless today because `ssbo_create` calls `glBindBufferBase`.
- Non-`static` file globals: `t_state` (`text.c`), `t_gl_state`, `vertices`,
  `indices` (`text_gl.c`). A second file declaring `vertices[]` will fail to
  link, since `build.sh` compiles everything into one binary.
- `main.c` calls `text_init`/`text_deinit` directly; the renderer should own
  subsystem init/shutdown order.
- The projection is built in `text_gl.c`; it should move to the renderer once
  a second pipeline needs it.

## Bug history (reusable lessons)

**Ray through a shared vertex.** Symptom: thin horizontal bands across
otherwise-correct glyphs. Font coordinates are round numbers, so exact hits
are common. Epsilons, vertex-ownership rules and a ray nudge each fixed one
letter and broke another. Solved properly by the paper's sign-based table —
and the table only works if nothing else (like a `t` range check) second-
guesses it.

**Moving the quad broke the curve test.** `frag_pos` must stay in glyph (em)
space; screen placement only goes into `gl_Position`.

**Mixing units.** Any shader constant has to say which unit it is in (em vs
px). Switching curves to em broke every pixel-based epsilon and ramp.

**Hand-counted instance layouts.** The instance layout was described in four
places (push order, a vec4-count macro, hand-computed offsets, shader
locations). One C struct + `sizeof`/`offsetof` now covers three of them; the
shader locations remain a manual match.

**`glVertexAttribPointer` size is 1–4.** A `mat4` attribute is 4 locations,
one pointer call per column. Passing 16 is a silent `GL_INVALID_VALUE`.

**Diagnosis method:** dump the real curve data for the broken glyph and
replicate the ray-crossing math in a standalone script, no GPU. If the
simulation is clean, the bug is GPU-side (precision, upload, binding); if it
reproduces, it's in the math/data and iterates much faster outside the
edit-build-run loop.
