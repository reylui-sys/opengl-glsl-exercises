#version 430

//  Recibe las coordenadas de textura del billboard y descarta los fragmentos que caen fuera del círculo de la esfera.

in vec3 gColor;
in vec2 gTexCoord;

layout(location = 5) uniform sampler2D uSpriteTex;

out vec4 fFragColor;

void main()
{
    // Las coordenadas de textura van de [0,1] × [0,1]. El centro de la textura es (0.5, 0.5). El radio de la esfera en espacio de textura es 0.38

    // Si la distancia del fragmento al centro supera ese radio, el fragmento está duera de la esfera y se descarta.
    // Esto convierte el quad cuadrado en un círculo (la silueta de la esfera).

    vec2 centerDist = gTexCoord - vec2(0.5, 0.5);
    float dist = length(centerDist);

    const float sphereRadius = 0.38;

    if (dist > sphereRadius)
        discard;    // fragmento fuera de la esfera

    vec4 texColor = texture(uSpriteTex, gTexCoord);
    //fFragColor = vec4(gColor, 1.0) * texColor;

    fFragColor = vec4(gColor, 1.0);
}
