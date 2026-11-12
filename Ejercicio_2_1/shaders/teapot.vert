#version 150 compatibility

// Ejercicio 1.4: Atributos de la tetera como variables in
in vec3 aPosition; // posición del vértice
in vec3 aNormal;   // normal del vértice (para calcular el color en función de la orientación)
in vec2 aTexCoord; // coordenadas de textura (para ejercicio 1.7)

// Ejercicio 1.4: Uniform para la matriz MVP
uniform mat4 mvp;

uniform mat4 matrixRotation; // Matriz de rotación para las normales (ejercicio 1.6)

// Ejercicio 1.5: Uniforms para morph tetera -> esfera
uniform float morphFactor;  // 0.0 = tetera, 1.0 = esfera
uniform float sphereRadius; // radio de la esfera

// Ejercicio 1.6: Uniform para rotación de colores
uniform float colorAngle; // ángulo de rotación en radianes

out vec4 vColor; // Color calculado en función de la normal (o normal rotada)
out vec2 vTexCoord; // Pasamos las coordenadas de textura al fragment shader (para ejercicio 1.7)

void main()
{
	// Ejercicio 1.5: Morph de la posición
	// Calculamos la posición en la esfera normalizando y escalando
	vec3 spherePos = normalize(aPosition) * sphereRadius;
	// Interpolamos entre la posición original y la esfera
	vec3 finalPos = mix(aPosition, spherePos, morphFactor);

	gl_Position = mvp * vec4(finalPos, 1.0);

	// Ejercicio 1.6: Rotación del color (normal rotada respecto al eje X)
	vec4 rotatedNormal4 = matrixRotation * vec4(aNormal, 0.0);
	vec3 rotatedNormal = rotatedNormal4.xyz;
	vColor = vec4(abs(rotatedNormal), 1.0);

	// Pasar coordenadas de textura al fragment shader (para ejercicio 1.7)
	vTexCoord = aTexCoord;
}
