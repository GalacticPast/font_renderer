# AGENTS.md

## Project Overview

This repository is a custom font/text rendering project written primarily in C.

The current goal is to implement a Slug-style GPU text renderer using OpenGL.

The project already has:

- An OpenGL renderer
- Shader loading
- Linux platform support
- Wayland support
- stb_truetype for reading font data
- Custom vector/math types using the `db_*` naming convention
- Glyph contour extraction in progress
- A custom build system using `./build.sh`

The renderer should remain lightweight, understandable, and mostly self-contained.

---

## Main Goal

Implement high-quality GPU text rendering inspired by the Slug algorithm.

The intended pipeline is approximately:

1. Load a font using `stb_truetype`
2. Extract glyph outlines
3. Convert glyph outline commands into usable curve data
4. Represent line segments and quadratic Bézier curves consistently
5. Store glyph geometry in GPU-friendly buffers
6. Render glyph geometry through OpenGL
7. Evaluate curve coverage inside shaders
8. Produce smooth anti-aliased text without generating traditional bitmap glyph atlases

The implementation should be developed incrementally.

Do not redesign the entire renderer unless there is a clear technical reason.

---

## Language

Use C unless the existing file explicitly uses another language.

Do not introduce C++.

Prefer straightforward C code over complex abstractions.

Avoid unnecessary macros, metaprogramming, or hidden control flow.

---

## Coding Style

Follow the existing style of the repository.

Use the existing naming conventions.

Project-specific code generally uses the `db_` prefix.

Examples:

```c
db_vector2
db_vector3
db_vector4

db_vector2_make(...)
db_array_contours_append(...)
```

When adding new project-level types or functions, prefer the same convention.

Example:

```c
typedef struct db_quadratic_curve
{
    db_vector2 p0;
    db_vector2 p1;
    db_vector2 p2;
} db_quadratic_curve;
```

Prefer descriptive names over short names unless the variable is mathematical and its meaning is obvious.

For example:

```c
db_vector2 control_point;
db_vector2 end_point;
```

is preferable to:

```c
db_vector2 a;
db_vector2 b;
```

unless the surrounding algorithm makes the notation clearer.

---

## Build System

The project must continue to build using:

```sh
./build.sh
```

Do not introduce CMake, Meson, Make, Premake, or another build system unless explicitly requested.

Do not make assumptions that require an IDE.

The intended workflow is command-line development on Linux.

When modifying the project, verify that the changes are compatible with the existing build script.

---

## Platform

Primary development platform:

```text
Linux
Wayland
OpenGL
```

Arch Linux is commonly used during development.

Do not replace Wayland with SDL, GLFW, raylib, Qt, GTK, or another windowing abstraction unless explicitly requested.

The platform layer is intentionally custom.

---

## Dependencies

Keep external dependencies minimal.

Existing dependencies such as `stb_truetype` are acceptable.

Do not introduce a new dependency when the functionality can reasonably be implemented inside the project.

Before introducing any new third-party library, explain:

1. Why it is needed
2. What problem it solves
3. Why the existing code cannot reasonably solve the problem
4. Whether it affects the self-contained build goal

Do not introduce package-manager dependencies casually.

Avoid requiring `pkg-config` unless absolutely necessary.

The long-term goal is for the repository to be as self-contained as reasonably possible.

---

## Renderer Architecture

Do not bypass the existing renderer.

Use the project's current OpenGL abstraction and rendering structures where possible.

Before introducing:

- new buffer abstractions
- new shader systems
- new renderer APIs
- new resource managers
- new graphics backends

first inspect the existing implementation.

Extend the existing system rather than creating a parallel renderer.

---

## Font Parsing

Fonts are currently parsed with `stb_truetype`.

Glyph outlines returned by `stb_truetype` may contain commands such as:

```text
STBTT_vmove
STBTT_vline
STBTT_vcurve
STBTT_vcubic
```

The current implementation primarily needs to support TrueType-style outlines.

TrueType glyphs normally use quadratic Bézier curves.

Treat outline commands carefully.

---

## Contour Rules

`STBTT_vmove` begins a new contour.

It is not itself a drawable curve.

It sets the current point and contour starting point.

For example:

```c
case STBTT_vmove:
{
    curr_point.x = vertices[i].x;
    curr_point.y = vertices[i].y;

    contour_start = curr_point;
}
break;
```

`STBTT_vline` represents a straight segment from:

```text
current_point -> end_point
```

A line may later be represented in the renderer as a degenerate quadratic Bézier if that simplifies GPU processing.

One possible representation is:

```text
P0 = current point
P1 = midpoint(P0, P2)
P2 = end point
```

The midpoint must be calculated as:

```c
(P0 + P2) / 2
```

not:

```c
P2 / P0
```

For example:

```c
db_vector2 mid = db_vector2_make(
    (curr_point.x + p2.x) * 0.5f,
    (curr_point.y + p2.y) * 0.5f
);
```

`STBTT_vcurve` represents a quadratic Bézier.

The points are:

```text
P0 = current point
P1 = control point
P2 = end point
```

For `stb_truetype`:

```text
cx, cy = control point
x, y   = endpoint
```

After processing the curve:

```text
current_point = P2
```

---

## Quadratic Bézier Convention

Use the standard quadratic Bézier equation:

```text
B(t) = (1 - t)^2 P0
     + 2(1 - t)t P1
     + t^2 P2
```

where:

```text
0 <= t <= 1
```

Maintain a consistent point ordering everywhere:

```text
P0 = start
P1 = control
P2 = end
```

Do not silently change this ordering between CPU and GPU code.

---

## Contour Closure

Glyph contours are closed.

Keep track of the point introduced by the most recent `STBTT_vmove`.

At the end of a contour, verify whether the current point connects back to the contour start.

Do not assume every glyph contour is a single connected shape.

A glyph may contain multiple contours.

For example, characters such as:

```text
O
B
8
@
```

contain outer and inner contours.

The renderer must eventually preserve this structure so holes render correctly.

---

## Coordinate Systems

Font coordinates and OpenGL coordinates may use different scales and origins.

Do not permanently modify font geometry merely to make an early debugging view work.

Prefer an explicit transform.

Keep these coordinate spaces conceptually separate:

```text
font space
glyph-local space
screen/UI space
clip space
```

Scaling should be predictable.

Do not normalize glyph geometry without documenting why.

---

## Debugging Glyph Geometry

Before implementing advanced shader coverage, it is acceptable and encouraged to visualize glyph geometry.

Useful debugging modes include:

- rendering glyph control points
- rendering Bézier endpoints
- rendering control points in a different debug representation
- rendering tessellated curve approximations
- rendering contour lines
- rendering glyph bounding boxes

When debugging a glyph, start with simple characters.

Good test characters include:

```text
I
L
V
O
S
B
g
@
```

These expose increasingly complex contour behavior.

---

## GPU Representation

Do not prematurely optimize the glyph representation.

First make the representation mathematically correct and easy to inspect.

A curve may initially be represented as:

```c
typedef struct db_quadratic_curve
{
    db_vector2 p0;
    db_vector2 p1;
    db_vector2 p2;
} db_quadratic_curve;
```

Later, this data may be packed differently for GPU efficiency.

Do not introduce compressed or obscure encodings until the renderer works correctly.

---

## Shader Development

Shader code should remain readable.

When implementing curve calculations, use variable names that correspond to the mathematical meaning.

Prefer:

```glsl
vec2 p0;
vec2 control;
vec2 p2;
```

over unexplained vector packing.

When CPU and shader implementations use the same equation, keep their conventions identical.

If a mathematical derivation is non-obvious, add a short comment describing what is being computed.

Do not fill shader files with long tutorial comments.

---

## Slug Algorithm

The goal is inspired by the Slug text rendering approach.

Do not assume the repository is attempting to reproduce every implementation detail of the commercial Slug library.

The priority is understanding and implementing the underlying ideas.

The implementation should be built incrementally:

```text
font outline extraction
        ↓
quadratic curve representation
        ↓
correct contour reconstruction
        ↓
GPU upload
        ↓
debug curve rendering
        ↓
inside/outside or winding determination
        ↓
curve coverage
        ↓
anti-aliased glyph rendering
        ↓
text layout and batching
```

Do not jump directly to the final shader before earlier stages are verified.

---

## Text Rendering Roadmap

When deciding what to implement next, generally prefer this order:

### Phase 1 — Glyph Extraction

Verify that glyph vertices from `stb_truetype` are interpreted correctly.

Support:

```text
move
line
quadratic curve
```

Handle contour boundaries correctly.

### Phase 2 — Curve Representation

Convert glyph outline commands into a consistent internal representation.

Prefer quadratic curves as the common representation where practical.

### Phase 3 — CPU Debug Rendering

Display glyph outlines using the existing renderer.

Approximate Bézier curves with line segments if necessary for debugging.

The purpose here is verification, not final rendering.

### Phase 4 — GPU Data

Upload glyph curve information into buffers suitable for the existing OpenGL renderer.

### Phase 5 — GPU Curve Evaluation

Implement mathematical evaluation of glyph boundaries in shaders.

### Phase 6 — Fill Determination

Determine whether fragments are inside or outside the glyph.

Correctly support holes and multiple contours.

### Phase 7 — Anti-Aliasing

Add smooth edge coverage.

Do not add complex anti-aliasing until glyph geometry is correct.

### Phase 8 — Text Layout

Once individual glyph rendering works, add:

- advances
- bearings
- baseline positioning
- kerning
- strings
- batching

---

## stb_truetype Usage

Do not modify `stb_truetype` itself unless absolutely necessary.

Treat vendor code as external code.

Project-specific font logic belongs outside the vendor directory.

When using `stbtt_GetGlyphShape`, remember to free returned vertex data with the appropriate stb_truetype function when it is no longer needed.

Avoid leaking glyph shape allocations during repeated text rendering.

---

## Memory Management

This is C.

Ownership must remain clear.

For every allocation, determine:

```text
Who allocates it?
Who owns it?
When is it freed?
```

Avoid temporary heap allocations inside per-frame rendering code unless necessary.

Font parsing and glyph preprocessing may allocate during initialization.

Rendering should eventually avoid unnecessary per-frame allocation.

---

## Performance Philosophy

Correctness comes before optimization.

Do not optimize an incorrect glyph pipeline.

Once rendering works correctly, prioritize improvements such as:

- glyph caching
- buffer reuse
- draw batching
- avoiding repeated outline parsing
- reducing CPU/GPU synchronization
- compact GPU curve storage

Measure before performing complicated optimizations.

---

## Error Handling

Check failures when:

- opening font files
- loading font data
- initializing `stb_truetype`
- creating OpenGL objects
- compiling shaders
- linking shader programs
- allocating buffers

Do not silently continue after a critical initialization failure.

Prefer a clear diagnostic message.

---

## Repository Structure

Respect the existing repository structure.

Typical areas may include:

```text
src/
vendor/
bin/
shaders/
build.sh
```

Do not reorganize major directories unless explicitly requested.

Vendor libraries belong in:

```text
vendor/
```

Project source belongs in:

```text
src/
```

Generated binaries belong in:

```text
bin/
```

or whatever directory the current build script already uses.

---

## Working With Existing Code

Before changing a function:

1. Read the surrounding implementation
2. Find where the function is called
3. Understand the associated data structures
4. Check whether an existing helper already solves part of the problem

Avoid creating duplicate functionality.

When an apparent bug exists, explain the cause before rewriting large sections of code.

Prefer the smallest correct fix.

---

## Agent Behavior

When assisting with this repository, act primarily as an engineering mentor.

Do not immediately write large implementations unless explicitly asked.

When the user is learning an algorithm or renderer concept, explain:

```text
what needs to happen
why it needs to happen
what data is needed
how it connects to the existing renderer
```

Then allow the user to implement it.

When reviewing user-written code:

1. Identify the specific bug
2. Explain why it is incorrect
3. Give a small example demonstrating the failure
4. Explain the correct mathematical or programming concept
5. Avoid replacing the entire implementation unless requested

---

## Before Writing Code

Before making a substantial change, inspect the relevant files.

Determine:

- the existing types
- renderer abstractions
- memory conventions
- naming conventions
- build process
- shader organization

Do not invent APIs that duplicate existing systems.

For architecture changes, explain the proposed design first.

---

## When Asked "What Should I Do Next?"

Do not jump several stages ahead.

Inspect the current implementation and identify the smallest next milestone that can be verified visually or mathematically.

For the font renderer, prefer milestones such as:

```text
correctly reconstruct one glyph
```

before:

```text
render an entire string
```

and:

```text
visualize quadratic curves
```

before:

```text
implement final analytical anti-aliasing
```

Each stage should have a clear test.

---

## Testing

Test simple cases before complicated ones.

Suggested progression:

```text
I
L
V
C
O
S
B
8
g
@
```

Also test:

- glyphs with one contour
- glyphs with multiple contours
- holes
- straight lines
- strong curves
- very small glyph sizes
- large glyph sizes

If geometry is incorrect, debug the glyph outline before debugging the fragment shader.

---

## Important Constraints

Do not:

- replace the custom renderer with a graphics framework
- replace Wayland with GLFW or SDL
- replace OpenGL with Vulkan
- replace stb_truetype without a strong reason
- add a traditional bitmap font atlas as the primary renderer
- introduce C++ into C files
- introduce a new build system
- hide mathematical logic behind unexplained abstractions
- optimize before correctness is established

Preserve the educational and experimental nature of the project.

---

## Current Development Focus

The immediate focus is the glyph outline pipeline.

The current work involves converting `stb_truetype` glyph vertices into contours composed of quadratic Bézier curves.

When inspecting this portion of the repository, pay particular attention to:

```text
STBTT_vmove
STBTT_vline
STBTT_vcurve
current point tracking
contour start tracking
contour closure
P0/P1/P2 ordering
```

The next goal after verifying contour extraction is to visualize these curves using the existing OpenGL renderer before implementing the final Slug-style GPU coverage algorithm.