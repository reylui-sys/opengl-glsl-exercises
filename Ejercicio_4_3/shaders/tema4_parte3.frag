#version 430

// Ahora recibe del geometry shader (no del vertex shader directamente)
in vec4 gColor;
in vec2 gTexCoord;

uniform sampler2D uSpriteTex;

out vec4 fFragColor;

void main()
{
    vec2 centerDist = gTexCoord - vec2(0.5, 0.5);
    float dist = length(centerDist);

    const float sphereRadius = 0.38;

    if (dist > sphereRadius)
        discard;

    vec4 texColor = texture(uSpriteTex, gTexCoord);
    fFragColor = texColor * gColor;
}