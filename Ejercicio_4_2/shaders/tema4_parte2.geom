#version 430

// Tamaño (lado / 2) del cuadrado donde pegamos la imagen de la esfera
// const float uSize2 = 0.04; // Conento porque se usa en el otro shader para el offset de la textura

// Le asignamos el ID directamente para no tener que localizar la variable uniform en el código de la aplicación
layout(location = 0) uniform mat4 uModelViewProjMatrix;
layout(location = 1) uniform mat4 uModelViewMatrix;
layout(location = 2) uniform mat3 uNormalMatrix;
layout(location = 3) uniform mat4 uProjectionMatrix;
layout(location = 4) uniform float uExplosionFactor = 0;
layout(location = 5) uniform sampler2D uSpriteTex; // textura esfera
layout(location = 6) uniform bool uImpostorMode = false; // activar impostores

// Datos que llegan del Vertex Shader. Como la primitiva de entrada es un triángulo, los arrays tienen 3 elementos.
in vec3 vECNorm[]; // Normal de cada vértice en S.R. vista
in vec3 vColor[]; // Color de cada vértice

// Tipo de primitiva de entrada. El GS recibe triángulos completos (3 vértices).
layout(triangles) in;

// Tipo de primitiva de salida y máximo de vértices
// Un punto por triángulo, max_vertices = 1
// Un quad (2 triángulos, 4 vértices), max_vertices = 4
layout(points, max_vertices = 1) out;

// Datos de salida hacia el Fragment Shader
out vec3 gColor; // Color interpolado del punto / quad

void main()
{
	// Tarea 2.1 (por hacer): fijar los datos de salida del vértice y emitir el vértice
    
	// Posiciones de los 3 vértices en S.R. vista
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;

    // Baricentro en S.R. vista
    vec4 barycenter = (p0 + p1 + p2) / 3.0;

    // Tarea 2.2 – Explosión: desplazamos el baricentro a lo largo de la normal

    // Normal del triángulo en S.R. vista: Producto vectorial
    vec3 edge1 = vec3(p1 - p0);
    vec3 edge2 = vec3(p2 - p0);
    vec3 triNormal = normalize(cross(edge1, edge2));

    // Baricentro desplazado
    vec4 explodedPos = barycenter + vec4(triNormal * uExplosionFactor, 0.0);

    gl_Position = uProjectionMatrix * explodedPos;

    // Color medio de los 3 vértices del triángulo
    gColor = (vColor[0] + vColor[1] + vColor[2]) / 3.0;

    // Emite el único vértice (un punto)
    EmitVertex();
    EndPrimitive();

	// Tarea 2.3 (por hacer): generar como salida el siguiente strip de triángulos (con 2 triángulos)
	//     v2    v3
	//     *-----*
	//     |\    |
	//     | \   |
	//     |  \  |
	//     |   \ |
	//     *-----*
	//    v0     v1
	//
	//  fijar los datos de salida y emitir sus vértices
}
