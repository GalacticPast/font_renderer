# Progress Notes

Working notes for picking this project back up without re-deriving context.
See `AGENTS.md` for the project's permanent rules/roadmap — this file is the
"where did I leave off" log, not a spec. Update it as things change; delete
stale sections rather than letting them rot.

## Current direction

Implementing the **real Slug algorithm (Lengyel's technique)**, not the
Loop-Blinn-style fan/FBO-resolve approach that was prototyped earlier and
then intentionally deleted (see "Abandoned approach" below). Curve data for
a glyph is uploaded once to an SSBO; the shader does the inside/outside
curve-coverage test per fragment directly against that buffer, indexed by a
`(curve_start, curve_count)` range.

OpenGL is explicitly **temporary** — the plan is to move to Vulkan once the
Slug algorithm itself is working. Keep that in mind when deciding how much
effort is worth sinking into GL-specific plumbing (SSBO setup, uniform
wiring) vs. the API-agnostic parts (contour/curve extraction, the curve math
itself), which are the parts that carry over.

## What's working / verified

- `load_font` (`src/main.c`) parses all 26 uppercase letters via
  `stbtt_GetGlyphShape`, correctly handling `STBTT_vmove` / `STBTT_vline` /
  `STBTT_vcurve`, contour starts, and contour closure. Verified visually on
  `B` and `S` (multiple contours, holes, mixed lines+curves) before the
  reset described below.
- `center_glyphs` centers glyph geometry on its bounding box.
- Units: raw `stbtt` vertices are **font design units** (not "em units"),
  converted to pixels via `stbtt_ScaleForMappingEmToPixels` →
  `em_to_px_scale`, stored per-glyph.
- Camera/projection: orthographic (`db_matrix4_ortho2d`), 1 world unit = 1
  screen pixel. Centered on `[-W/2, W/2] x [-H/2, H/2]`, matching
  `center_glyphs`.
- `gl.c`/`gl.h` have working `VAO`/`VBO`/`EBO` wrappers plus a new `SSBO`
  wrapper (`ssbo_create`/`ssbo_bind`/`ssbo_unbind`/`ssbo_delete`,
  binding point 2). `shader_create` takes vertex/fragment paths as
  parameters (no longer hardcoded).

## Currently broken / mid-edit

`src/main.c` does **not currently compile** — it's mid-refactor from the old
CPU-tessellated line-strip debug renderer to the SSBO-based curve renderer:

- `main()` (around main.c:284) calls `ssbo_create(&b_ssbo, glyphs.data[0].curves, ...)`
  but the render loop below it (main.c:294-319) still references
  `b_vbo`, `b_vertices`, and `contour_vertex_counts`, none of which are
  declared anymore. This is leftover from the deleted debug path.
- `assets/shaders/vertex.glsl` is a non-compiling sketch: `unifort` typo
  (main.c sibling file, line 4), the `Curve` struct is missing its trailing
  `;`, `curve_buffer` is declared as a flat `vec2 curves[]` instead of
  `Curve curves[]`, and `bezier_solver()` references `t`/`p0`/`p1`/`p2`
  which are never defined/passed in (no per-vertex attributes or SSBO index
  lookup wired up yet).
- `assets/shaders/curve_fragment.glsl` / `curve_vertex.glsl` are deleted
  (part of the abandoned Loop-Blinn prototype, see below) — `gl.c` may still
  reference them in cleanup code; check before assuming they're gone
  everywhere.

None of this is a mystery bug — it's just unfinished plumbing from the
current task in progress (see "Next step" below).

## Abandoned approach (don't resurrect without reason)

An earlier working prototype used **Loop-Blinn fan triangulation + an FBO
accumulation/resolve pass** (per-contour fan triangles, curve-correction
triangles with UV-based `u²-v` coverage test, additive blend into an FBO,
then a resolve shader). It worked end-to-end and rendered a correct
anti-aliased "S". The user deliberately deleted all of it to instead
implement the actual Slug/Lengyel algorithm, which does the curve test
directly against an SSBO of curves rather than pre-triangulating fans. Don't
reintroduce the fan/FBO machinery unless explicitly asked — it was a
deliberate pivot, not an accident.

## Key bugs already fixed (watch for regressions of the same shape)

- `db.h` had `db_max`/`db_min` macros (integer-only, `s64`-cast) that
  **collided by name** with `db_math.h`'s float versions, silently
  truncating float bbox comparisons through an `s64` cast → UBSan abort.
  Renamed to `db_max_s64`/`db_min_s64` in `db.h`. If float min/max math
  looks wrong anywhere, check for a similar silent macro shadowing first.
- `-win_width / 2.0f` where `win_width` is `u32` wraps around (unsigned
  negation), not a simple negative float. Cast to signed before negating.
- `glGetShaderiv` vs `glGetProgramiv` — don't pass a program handle
  (`glCreateProgram`) to a function that expects a shader object handle
  (`glCreateShader`); wrong object type silently triggers
  `GL_INVALID_OPERATION` (1282).
- Duplicate/zero-length first contour: don't manually seed
  `contours_start_indicies` with a `0` before the parse loop *and* let the
  first `STBTT_vmove` push its own `0` — pick one.
- `GL_LINE_STRIP`/`GL_LINE_LOOP` can't skip a stray connecting line between
  disjoint contours by duplicating the last vertex (that trick only works
  for degenerate zero-area triangles, not zero-length lines) — use
  per-contour `glDrawArrays` calls (or `glMultiDrawArrays`) instead.

## Next step

Finish wiring the SSBO path in `main.c` + `vertex.glsl`:

1. Fix `vertex.glsl`: proper `Curve` struct w/ semicolon, `curve_buffer`
   declared as `Curve curves[]`, and decide how each vertex knows which
   curve + `t` it corresponds to (per-vertex attributes, since
   `curve_start`/`curve_count` uniforms only let you loop over a *range* in
   the fragment shader for the coverage test — they don't by themselves
   tell the vertex shader which single curve a given vertex belongs to).
2. Rip out the dangling `b_vbo`/`b_vertices`/`contour_vertex_counts`
   references in `main()`'s render loop, replace with whatever draw
   strategy the finished SSBO design needs (likely one `glDrawArrays` call
   per glyph, using `curve_start`/`curve_count` uniforms set right before
   the draw).
3. Get it building and rendering *one* glyph again (same acceptance bar as
   before: no GL errors, visually correct outline) before expanding to all
   26.

Per `AGENTS.md`'s roadmap this is still Phase 4/5 (GPU data upload + GPU
curve evaluation) — fill/coverage/anti-aliasing (Phase 6/7) comes after the
SSBO plumbing works and renders *something* correct, even just outlines.
