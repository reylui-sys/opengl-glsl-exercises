#version 150 compatibility

// Ejercicio 1.2: gl_Vertex y gl_Color por variables in
in vec4 aPosition;
in vec4 aColor;

// Ejercicio 1.3: gl_ModelViewProjectionMatrix por uniform
uniform mat4 mvp;

out vec4 vColor;

void main()
{
	vColor = aColor; // Pasamos el color al fragment shader a través de la variable de salida vColor
	gl_Position = mvp * aPosition; // Transformamos la posición del vértice por la matriz mvp y la asignamos a gl_Position
}
