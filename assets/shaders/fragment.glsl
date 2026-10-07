#version 430 core

struct Curve
{
    vec4 p0;
    vec4 p1; // control point
    vec4 p2;
};

layout(std430, binding = 1) buffer curve_buffer
{
    Curve curves[];
};


out vec4 FragColor;

in vec2 frag_pos;

flat in vec2 frag_curve_indicies;

uniform float font_px;

// Paper Table 1: 16-bit lookup. For input code i, bit 2i is the t1 contribution and bit 2i+1 the t2 contribution.
#define CONTRIBUTION_TABLE 0x2E74

// Supersampling: samples per pixel along each axis (paper section 6). 1 disables it.
#define SAMPLES_PER_AXIS 3

// The paper's only tolerance: a near-zero quadratic term means the curve is a straight line.
#define LINEAR_EPSILON 1e-6


// Evaluate B(t) along the ray's axis: (1-t)^2 p0 + 2t(1-t) p1 + t^2 p2
float bezier_along_ray(float u0, float u1, float u2, float t)
{
    return (1.0 - t) * (1.0 - t) * u0
         + 2.0 * t * (1.0 - t) * u1
         + t * t * u2;
}


// Paper Eq. (3): fraction of the pixel on the near side of a crossing.
// distance_em is the crossing's distance from the pixel center along the ray.
float crossing_fraction(float distance_em)
{
    return clamp(distance_em * font_px + 0.5, 0.0, 1.0);
}


// One curve against one ray that points in +u. v is the offset across the ray, so the ray is v = 0.
// pixel_u is the sample position along the ray.
// Returns the signed contribution of this curve to the winding number.
float ray_contribution(
    float u0, float u1, float u2,
    float v0, float v1, float v2,
    float pixel_u)
{
    // Paper Eq. (2): input code, one bit per control point above the ray.
    int input_code = (v0 > 0.0 ? 2 : 0)
                   + (v1 > 0.0 ? 4 : 0)
                   + (v2 > 0.0 ? 8 : 0);

    int output_code = (CONTRIBUTION_TABLE >> input_code) & 3;

    // Curve never changes the winding number for this ray.
    if (output_code == 0)
        return 0.0;

    // Paper Eq. (1): roots of B_v(t) = 0.
    float a = v0 - 2.0 * v1 + v2;
    float b = v0 - v1;
    float c = v0;

    float t1;
    float t2;

    if (abs(a) < LINEAR_EPSILON)
    {
        // Straight line: one root, shared by both slots.
        t1 = c / (2.0 * b);
        t2 = t1;
    }
    else
    {
        float s = sqrt(max(b * b - a * c, 0.0));
        t1 = (b - s) / a;
        t2 = (b + s) / a;
    }

    float contribution = 0.0;

    if ((output_code & 1) != 0 && t1 >= 0.0 && t1 < 1.0)
    {
        contribution += crossing_fraction(bezier_along_ray(u0, u1, u2, t1) - pixel_u);
    }

    if ((output_code & 2) != 0 && t2 >= 0.0 && t2 < 1.0)
    {
        contribution -= crossing_fraction(bezier_along_ray(u0, u1, u2, t2) - pixel_u);
    }

    return contribution;
}


// Horizontal ray at height ray_y, pointing in +x.
float horizontal_contribution(Curve curve, float ray_y, float pixel_x)
{
    return ray_contribution(
        curve.p0.x, curve.p1.x, curve.p2.x,
        curve.p0.y - ray_y, curve.p1.y - ray_y, curve.p2.y - ray_y,
        pixel_x);
}


// Vertical ray at x = ray_x, pointing in +y.
// Uses the rotation (u, v) = (y, -x), so the sign convention matches the horizontal ray.
float vertical_contribution(Curve curve, float ray_x, float pixel_y)
{
    return ray_contribution(
        curve.p0.y, curve.p1.y, curve.p2.y,
        ray_x - curve.p0.x, ray_x - curve.p1.x, ray_x - curve.p2.x,
        pixel_y);
}


void main()
{
    float horizontal_sum = 0.0;
    float vertical_sum   = 0.0;

    // Sample positions sit on a line across the pixel: horizontal rays shift in y, vertical rays shift in x.
    for (int s = 0; s < SAMPLES_PER_AXIS; s++)
    {
        float offset_px = (float(s) + 0.5) / float(SAMPLES_PER_AXIS) - 0.5;
        float offset_em = offset_px / font_px;

        float ray_y = frag_pos.y + offset_em;
        float ray_x = frag_pos.x + offset_em;

        for (int i = int(frag_curve_indicies.x);
             i < int(frag_curve_indicies.y);
             ++i)
        {
            Curve curve = curves[i];

            horizontal_sum += horizontal_contribution(curve, ray_y, frag_pos.x);
            vertical_sum   += vertical_contribution(curve, ray_x, frag_pos.y);
        }
    }

    // Horizontal and vertical rays are two estimates of the same coverage.
    float coverage =
        0.5 *
        (
            horizontal_sum +
            vertical_sum
        ) / float(SAMPLES_PER_AXIS);

    float alpha =
        clamp(abs(coverage), 0.0, 1.0);

    if (alpha <= 0.0)
        discard;

    FragColor = vec4(vec3(1.0), alpha);
}
