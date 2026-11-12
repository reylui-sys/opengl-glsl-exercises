#version 150

// Uniforms
uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
uniform int uDrawSky;

// Índices de refracción
uniform float uEtaR;
uniform float uEtaG;
uniform float uEtaB;
uniform bool uChromatic; // Si true, se calcula un vector refractado diferente para cada canal RGB, si false se usa el mismo para los tres

// Atributos de entrada
in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

// Variables de salida al fragment shader
out vec2 vTexCoord; // Pasa las coordenadas de textura al fragment shader
out vec3 vWorldPos; // Posición del vértice en el espacio del mundo, para usar en cálculos de iluminación
out vec3 vWorldNormal; // Normal transformada al espacio del mundo, para usar en cálculos de iluminación
out vec3 vReflectDir; // Vector de reflexión, para usar en el fragment shader para muestrear el entorno reflejado
out vec3 vRefractDir; // Vector de refracción (usado si no se hace dispersión cromática)
out vec3 vRefractDirR; // Vector de refracción para el canal rojo (usado si se hace dispersión cromática)
out vec3 vRefractDirG; // Vector de refracción para el canal verde (usado si se hace dispersión cromática)
out vec3 vRefractDirB; // Vector de refracción para el canal azul (usado si se hace dispersión cromática)
out vec3 vViewDir; // Vector vista (del vértice hacia la cámara), para usar en cálculos de iluminación y en el fragment shader para muestrear el entorno refractado

void main()
{
	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);

	// Pasar coordenadas de textura
	vTexCoord = aTexCoord;

	// Calcula vectores en espacio del mundo (world space)
	// Posición del vértice en el mundo
	vWorldPos = vec3(uModelMatrix * vec4(aPosition, 1.0)); // Transformar la posición del vértice al espacio del mundo usando la matriz de modelo

	// Normal transformada al espacio del mundo
	// uNormalMatrix = mat3(transpose(inverse(uModelMatrix)))
	vWorldNormal = normalize(uNormalMatrix * aNormal); // Transformar la normal al espacio del mundo usando la matriz de normales (que es la inversa transpuesta de la matriz de modelo) y normalizarla

	// Vector vista (del vértice hacia la cámara)
	vViewDir = normalize(uCameraPos - vWorldPos); // Calcular el vector vista restando la posición del vértice en el mundo de la posición de la cámara y normalizando el resultado

	// Vector reflejo
	// reflect() espera el vector incidente (de la cámara al vértice, o sea -vViewDir)
	vReflectDir = reflect(-vViewDir, vWorldNormal); // Calcular el vector de reflexión usando la función reflect() con el vector incidente (-vViewDir) y la normal del mundo (vWorldNormal)

	// Vector refractado (un solo índice, el verde por defecto)
	vRefractDir = refract(-vViewDir, vWorldNormal, uEtaG); // Calcular el vector de refracción usando la función refract() con el vector incidente (-vViewDir), la normal del mundo (vWorldNormal) y el índice de refracción para el canal verde (uEtaG)

	// Dispersión cromática - un vector refractado por cada canal RGB
	if (uChromatic)
	{
		vRefractDirR = refract(-vViewDir, vWorldNormal, uEtaR);
		vRefractDirG = refract(-vViewDir, vWorldNormal, uEtaG);
		vRefractDirB = refract(-vViewDir, vWorldNormal, uEtaB);
	}
	else
	{
		vRefractDirR = vRefractDir;
		vRefractDirG = vRefractDir;
		vRefractDirB = vRefractDir;
	}
}
