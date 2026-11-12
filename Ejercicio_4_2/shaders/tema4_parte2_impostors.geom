#version 430

//  Programa separado porque la primitiva de salida es triangle_strip yn no points
const float uSize2 = 0.04;  // lado/2 de la textura en S.R. vista

layout(location = 3) uniform mat4 uProjectionMatrix;
layout(location = 4) uniform float uExplosionFactor = 0;

in vec3 vECNorm[];
in vec3 vColor[];

layout(triangles) in;

// Con 4 vértices y triangle_strip formamos 2 triángulos = 1 quad:

// Orden de emisión: v0, v1, v2, v3
// Triángulo 1: v0, v1, v2
// Triángulo 2: v1, v3, v2 (OpenGL lo ahce automáticamente)
layout(triangle_strip, max_vertices = 4) out;

out vec3 gColor; // color del fragmento
out vec2 gTexCoord; // coordenadas de textura para la esfera

void main()
{
    // Baricentro en S.R. vista
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;

    vec4 barycenter = (p0 + p1 + p2) / 3.0;

    // Explosión
    vec3 edge1 = vec3(p1 - p0);
    vec3 edge2 = vec3(p2 - p0);
    vec3 triNormal = normalize(cross(edge1, edge2));

    vec4 center = barycenter + vec4(triNormal * uExplosionFactor, 0.0);

    // Color medio de los 3 vértices del triángulo de entrada
    vec3 avgColor = (vColor[0] + vColor[1] + vColor[2]) / 3.0;

    //   En S.R. vista, el eje Z siempre apunta hacia la cámara.
    //   Con la Z fija moviendo solo x e y parece que siempre sea una esfera, pero es un aiamgen 2D

    // v0: esquina inferior-izquierda
    gColor = avgColor;
    gTexCoord = vec2(0.0, 0.0);
    gl_Position = uProjectionMatrix * (center + vec4(-uSize2, -uSize2, 0.0, 0.0));
    EmitVertex();

    // v1: esquina inferior-derecha
    gColor = avgColor;
    gTexCoord = vec2(1.0, 0.0);
    gl_Position = uProjectionMatrix * (center + vec4( uSize2, -uSize2, 0.0, 0.0));
    EmitVertex();

    // v2: esquina superior-izquierda
    gColor = avgColor;
    gTexCoord = vec2(0.0, 1.0);
    gl_Position = uProjectionMatrix * (center + vec4(-uSize2,  uSize2, 0.0, 0.0));
    EmitVertex();

    // v3: esquina superior-derecha
    gColor = avgColor;
    gTexCoord = vec2(1.0, 1.0);
    gl_Position = uProjectionMatrix * (center + vec4( uSize2,  uSize2, 0.0, 0.0));
    EmitVertex();

    EndPrimitive();
}
