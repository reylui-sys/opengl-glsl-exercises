#version 430

// Tamaño (lado/2) del quad billboard en S.R. vista.
const float uSize2 = 0.04;

// El geometry shader recibe primitivas de tipo POINT (un vértice cada una) y emite una tira de triángulos (2 triángulos = 1 quad = 4 vértices).
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// Recibe del vertex shader el color interpolado (como array: un valor por vértice de entrada)
in vec4 vColor[];

// Envía al fragment shader el color y las coordenadas de textura del nuevo vértice
out vec4 gColor;
out vec2 gTexCoord;

// Necesitamos la matriz de proyección para proyectar los 4 vértices del quad
layout(location = 3) uniform mat4 uProjectionMatrix;

void main()
{
    // Centro del billboard: posición de la partícula en espacio de vista.
    // gl_in[0].gl_Position viene ya en view space porque el VS usó uModelViewMatrix.
    vec4 center = gl_in[0].gl_Position;

    // Color de esta partícula
    vec4 col = vColor[0];

    // Construimos el quad desplazando ±uSize2 en X e Y en espacio de vista.
    // Como estamos en vista, el eje Z apunta hacia la cámara, así que mover
    // solo X e Y garantiza que el quad siempre mira de frente a la cámara
    // (billboard orientado a cámara de forma automática).
    // Luego aplicamos uProjectionMatrix a cada vértice.

    // v0: esquina inferior-izquierda
    gColor = col;
    gTexCoord = vec2(0.0, 0.0);
    gl_Position = uProjectionMatrix * (center + vec4(-uSize2, -uSize2, 0.0, 0.0));
    EmitVertex();

    // v1: esquina inferior-derecha
    gColor = col;
    gTexCoord = vec2(1.0, 0.0);
    gl_Position = uProjectionMatrix * (center + vec4( uSize2, -uSize2, 0.0, 0.0));
    EmitVertex();

    // v2: esquina superior-izquierda
    gColor = col;
    gTexCoord = vec2(0.0, 1.0);
    gl_Position = uProjectionMatrix * (center + vec4(-uSize2,  uSize2, 0.0, 0.0));
    EmitVertex();

    // v3: esquina superior-derecha
    gColor = col;
    gTexCoord = vec2(1.0, 1.0);
    gl_Position = uProjectionMatrix * (center + vec4( uSize2,  uSize2, 0.0, 0.0));
    EmitVertex();

    EndPrimitive();
}
