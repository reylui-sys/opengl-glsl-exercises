#version 430

in vec3 gColor; // Color interpolado desde el vértice para cada fragmento

// Le asignamos el ID directamente para no tener que localizar la variable uniform en el código de la aplicación
// layout(location = 5) uniform sampler2D uSpriteTex;

out vec4 fFragColor;

void main()
{
	vec4 color;
	
	color = vec4(gColor, 1.0);

	fFragColor = color;	// Se pinta con el color del GS, que a su vez es el color del vértice (sin texturizar)
}