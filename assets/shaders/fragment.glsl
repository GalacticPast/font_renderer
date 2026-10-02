#version 430 core

struct Curve
{
    vec4 p0;
    vec4 p1; // control 
    vec4 p2;
};

layout(std430, binding = 1) buffer curve_buffer{
    Curve curves[];
};

out vec4 FragColor;
in vec2 frag_pos;

uniform vec2 curve_indicies;

// Font curve coordinates tend to land on suspiciously round numbers, so a ray
// cast at a pixel's exact y has a real (not astronomically rare) chance of
// passing exactly through a vertex shared by two curves - an ambiguous case
// that can double-count or miss a crossing depending on floating-point
// rounding. Nudging the ray height by a small, arbitrary, non-round offset
// makes an exact hit vanishingly unlikely, which is simpler and far more
// robust than trying to special-case every shared-vertex configuration.
#define RAY_Y_NUDGE 0.0317

// Roots of B_y(t) = frag_pos.y, i.e. where this curve crosses the ray's height.
// Up to two roots (curve.x holds the first, curve.y the second); -1 means "no root".
vec2 horiz_ray_roots(Curve curve, float ray_y)
{
    float a = curve.p0.y - 2.0 * curve.p1.y + curve.p2.y;
    float b = 2.0 * (curve.p1.y - curve.p0.y);
    float c = curve.p0.y - ray_y;

    vec2 roots = vec2(-1.0, -1.0);

    // a == 0 means p1 is the midpoint of p0/p2 (our line-as-curve convention) -
    // the quadratic degenerates to linear, so solve b*t + c = 0 instead.
    if (abs(a) < 1e-2)
    {
        if (abs(b) > 1e-3)
        {
            float t = -c / b;
            if (t >= 0.0 && t < 1.0)
                roots.x = t;
        }
        return roots;
    }

    float discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0)
        return roots;

    float sqrt_discriminant = sqrt(discriminant);
    float t_1               = (-b + sqrt_discriminant) / (2.0 * a);
    float t_2               = (-b - sqrt_discriminant) / (2.0 * a);

    if (t_1 >= 0.0 && t_1 < 1.0)
        roots.x = t_1;
    if (t_2 >= 0.0 && t_2 < 1.0)
        roots.y = t_2;

    return roots;
}

// B_x(t) - the curve's x-coordinate at an already-known t.
float curve_x_at(Curve curve, float t)
{
    return curve.p0.x + 2.0 * t * (curve.p1.x - curve.p0.x) +
           t * t * (curve.p0.x - 2.0 * curve.p1.x + curve.p2.x);
}

void main()
{
    float ray_y = frag_pos.y + RAY_Y_NUDGE;

    int crossings = 0;
    for (int i = int(curve_indicies.x); i < int(curve_indicies.y); i++)
    {
        Curve curve = curves[i];
        vec2  roots = horiz_ray_roots(curve, ray_y);

        if (roots.x >= 0.0 && curve_x_at(curve, roots.x) > frag_pos.x)
            crossings++;
        if (roots.y >= 0.0 && curve_x_at(curve, roots.y) > frag_pos.x)
            crossings++;
    }

    if (crossings % 2 == 0)
        discard;

    FragColor = vec4(vec3(1.0), 1.0);
}
