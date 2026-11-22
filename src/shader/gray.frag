#version 330 core

in vec3 vLocal;
out vec4 FragColor;

void main(void)
{
    const float lineWidth = 1.5;

    vec3 p = abs(vLocal);
    vec3 d = max(fwidth(p), vec3(1e-6));
    vec3 pixelsToEdge = (1.0 - p) / d;

    // On a face one axis is always at the border; an edge is where a second one is too.
    int nearBorder = int(pixelsToEdge.x < lineWidth)
    + int(pixelsToEdge.y < lineWidth)
    + int(pixelsToEdge.z < lineWidth);

    FragColor = nearBorder >= 2 ? vec4(0.0, 0.0, 0.0, 1.0)
    : vec4(0.5, 0.5, 0.5, 1.0);
}