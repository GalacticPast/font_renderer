#version 330 core

in vec2 v_uv;
out vec4 FragColor;

void main()
{
    // Loop-Blinn implicit test for a quadratic Bezier triangle.
    // p0 -> (0,0), control -> (0.5,0), p2 -> (1,1); the curve itself is
    // where u*u - v == 0. f < 0 is the region between the chord (p0-p2)
    // and the curve; f > 0 is the sliver cut off between the curve and
    // the straight p0-control-p2 path.
    float f = v_uv.x * v_uv.x - v_uv.y;

    // Antialiased edge: how many pixels wide is one unit of f here.
    float afwidth = fwidth(f);
    float coverage = clamp(0.5 - f / afwidth, 0.0, 1.0);

    FragColor = vec4(vec3(1.0), coverage);
}
