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

#define EPSILON       1.52587890625e-5
#define RAY_NUDGE_PX  0.0317


// ------------------------------------------------------------
// Horizontal ray:
// Find t where B_y(t) = ray_y
// ------------------------------------------------------------

vec2 horiz_ray_roots(Curve curve, float ray_y)
{
    float a = curve.p0.y - 2.0 * curve.p1.y + curve.p2.y;
    float b = 2.0 * (curve.p1.y - curve.p0.y);
    float c = curve.p0.y - ray_y;

    vec2 roots = vec2(-1.0);

    // Linear case.
    if (abs(a) < EPSILON)
    {
        if (abs(b) > EPSILON)
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

    discriminant = max(discriminant, 0.0);

    float sqrt_discriminant = sqrt(discriminant);

    float t1 = (-b + sqrt_discriminant) / (2.0 * a);
    float t2 = (-b - sqrt_discriminant) / (2.0 * a);

    int count = 0;

    if (t1 >= 0.0 && t1 < 1.0)
        roots[count++] = t1;

    // Prevent a tangent/double root from being counted twice.
    if (abs(t2 - t1) > EPSILON &&
        t2 >= 0.0 &&
        t2 < 1.0)
    {
        roots[count++] = t2;
    }

    return roots;
}


// ------------------------------------------------------------
// Vertical ray:
// Find t where B_x(t) = ray_x
// ------------------------------------------------------------

vec2 vert_ray_roots(Curve curve, float ray_x)
{
    float a = curve.p0.x - 2.0 * curve.p1.x + curve.p2.x;
    float b = 2.0 * (curve.p1.x - curve.p0.x);
    float c = curve.p0.x - ray_x;

    vec2 roots = vec2(-1.0);

    // Linear case.
    if (abs(a) < EPSILON)
    {
        if (abs(b) > EPSILON)
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

    discriminant = max(discriminant, 0.0);

    float sqrt_discriminant = sqrt(discriminant);

    float t1 = (-b + sqrt_discriminant) / (2.0 * a);
    float t2 = (-b - sqrt_discriminant) / (2.0 * a);

    int count = 0;

    if (t1 >= 0.0 && t1 < 1.0)
        roots[count++] = t1;

    if (abs(t2 - t1) > EPSILON &&
        t2 >= 0.0 &&
        t2 < 1.0)
    {
        roots[count++] = t2;
    }

    return roots;
}


// ------------------------------------------------------------
// Evaluate B_x(t)
// ------------------------------------------------------------

float curve_x_at(Curve curve, float t)
{
    return curve.p0.x
         + 2.0 * t * (curve.p1.x - curve.p0.x)
         + t * t *
           (curve.p0.x - 2.0 * curve.p1.x + curve.p2.x);
}


// ------------------------------------------------------------
// Evaluate B_y(t)
// ------------------------------------------------------------

float curve_y_at(Curve curve, float t)
{
    return curve.p0.y
         + 2.0 * t * (curve.p1.y - curve.p0.y)
         + t * t *
           (curve.p0.y - 2.0 * curve.p1.y + curve.p2.y);
}


// ------------------------------------------------------------
// dB_y/dt
// Used for horizontal crossings.
// ------------------------------------------------------------

float curve_y_slope_at(Curve curve, float t)
{
    float a = curve.p0.y - 2.0 * curve.p1.y + curve.p2.y;
    float b = 2.0 * (curve.p1.y - curve.p0.y);

    return 2.0 * a * t + b;
}


// ------------------------------------------------------------
// dB_x/dt
// Used for vertical crossings.
// ------------------------------------------------------------

float curve_x_slope_at(Curve curve, float t)
{
    float a = curve.p0.x - 2.0 * curve.p1.x + curve.p2.x;
    float b = 2.0 * (curve.p1.x - curve.p0.x);

    return 2.0 * a * t + b;
}


// ------------------------------------------------------------
// Horizontal coverage.
//
// Solve B_y(t) = ray_y,
// then use B_x(t) for the coverage ramp.
//
// y decreasing -> +1
// y increasing -> -1
// ------------------------------------------------------------

float horizontal_coverage(
    Curve curve,
    float t,
    float ray_x)
{
    float x = curve_x_at(curve, t);

    float f = clamp(
        (x - ray_x) * font_px + 0.5,
        0.0,
        1.0
    );

    float slope = curve_y_slope_at(curve, t);

    // Tangent: curve touches the ray but does not cross it.
    if (abs(slope) < EPSILON)
        return 0.0;

    float winding_sign =
        (slope < 0.0) ? 1.0 : -1.0;

    return winding_sign * f;
}


// ------------------------------------------------------------
// Vertical coverage.
//
// Solve B_x(t) = ray_x,
// then use B_y(t) for the coverage ramp.
//
// x increasing -> +1
// x decreasing -> -1
// ------------------------------------------------------------

float vertical_coverage(
    Curve curve,
    float t,
    float ray_y)
{
    float y = curve_y_at(curve, t);

    float f = clamp(
        (y - ray_y) * font_px + 0.5,
        0.0,
        1.0
    );

    float slope = curve_x_slope_at(curve, t);

    if (abs(slope) < EPSILON)
        return 0.0;

    float winding_sign =
        (slope > 0.0) ? 1.0 : -1.0;

    return winding_sign * f;
}


// ------------------------------------------------------------

void main()
{
    // frag_pos and curve coordinates are assumed to be in
    // font/em space. Convert the pixel nudge into that space.
    float ray_y =
        frag_pos.y + RAY_NUDGE_PX / font_px;

    float ray_x =
        frag_pos.x + RAY_NUDGE_PX / font_px;

    float horizontal_coverage_sum = 0.0;
    float vertical_coverage_sum   = 0.0;


    for (int i = int(frag_curve_indicies.x);
         i < int(frag_curve_indicies.y);
         ++i)
    {
        Curve curve = curves[i];


        // ----------------------------------------------------
        // Horizontal ray
        // B_y(t) = ray_y
        // ----------------------------------------------------

        vec2 hroots =
            horiz_ray_roots(curve, ray_y);

        if (hroots.x >= 0.0)
        {
            horizontal_coverage_sum +=
                horizontal_coverage(
                    curve,
                    hroots.x,
                    frag_pos.x
                );
        }

        if (hroots.y >= 0.0)
        {
            horizontal_coverage_sum +=
                horizontal_coverage(
                    curve,
                    hroots.y,
                    frag_pos.x
                );
        }


        // ----------------------------------------------------
        // Vertical ray
        // B_x(t) = ray_x
        // ----------------------------------------------------

        vec2 vroots =
            vert_ray_roots(curve, ray_x);

        if (vroots.x >= 0.0)
        {
            vertical_coverage_sum +=
                vertical_coverage(
                    curve,
                    vroots.x,
                    frag_pos.y
                );
        }

        if (vroots.y >= 0.0)
        {
            vertical_coverage_sum +=
                vertical_coverage(
                    curve,
                    vroots.y,
                    frag_pos.y
                );
        }
    }


    // Horizontal and vertical rays are two estimates of
    // the same pixel coverage.
    float coverage =
        0.5 *
        (
            horizontal_coverage_sum +
            vertical_coverage_sum
        );

    float alpha =
        clamp(abs(coverage), 0.0, 1.0);

    if (alpha <= 0.0)
        discard;

    FragColor = vec4(vec3(1.0), alpha);
}
