#version 150 compatibility

in vec4 vColor; // El color se pasa desde el vertex shader
in vec2 vTexCoord; // Coordenadas de textura pasadas desde el vertex shader (para ejercicio 1.7)

// Ejercicio 1.7: Uniforms para descartar fragmentos
uniform bool discardOn; // variable que activa o desactiva el descarte de fragmentos
uniform float discardThreshold; // umbral que varía sinusoidalmente

out vec4 fFragColor; // Color final del fragmento que se enviará al framebuffer

void main()
{
	// Ejercicio 1.7: Descartar fragmentos según coordenadas de textura
	if (discardOn)
	{
		// Multiplicamos las coords de textura por 8 y nos quedamos con la parte decimal
		float s = fract(vTexCoord.s * 8.0);
		float t = fract(vTexCoord.t * 8.0);

		// Si ambos valores son mayores que el umbral, descartamos
		if (s > discardThreshold && t > discardThreshold)
			discard;
	}

	// Ejercicio 1.4: Color interpolado con perspectiva corregida
	fFragColor = vColor;
}
