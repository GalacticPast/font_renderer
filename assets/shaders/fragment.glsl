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

// Roots of B_y(t) = frag_pos.y, i.e. where this curve crosses the ray's height.
// Up to two roots (curve.x holds the first, curve.y the second); -1 means "no root".
vec2 horiz_ray_roots(Curve curve)
{
    float a = curve.p0.y - 2.0 * curve.p1.y + curve.p2.y;
    float b = 2.0 * (curve.p1.y - curve.p0.y);
    float c = curve.p0.y - frag_pos.y;

    vec2 roots = vec2(-1.0, -1.0);

    // a == 0 means p1 is the midpoint of p0/p2 (our line-as-curve convention) -
    // the quadratic degenerates to linear, so solve b*t + c = 0 instead.
    if (abs(a) < 1e-3)
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
    int curve_count = curves.length();

    int crossings = 0;
    for (int i = 0; i < curve_count; i++)
    {
        Curve curve = curves[i];
        vec2  roots = horiz_ray_roots(curve);

        if (roots.x >= 0.0 && curve_x_at(curve, roots.x) > frag_pos.x)
            crossings++;
        if (roots.y >= 0.0 && curve_x_at(curve, roots.y) > frag_pos.x)
            crossings++;
    }

    if (crossings % 2 == 0)
        discard;

    FragColor = vec4(vec3(1.0), 1.0);
}
