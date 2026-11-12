#version 150

// Samplers de texturas
uniform sampler2D uStoneTex;    // Unidad 0: textura piedra
uniform sampler2D uMossTex;     // Unidad 1: textura musgo (RGBA)
uniform samplerCube uTexMap;    // Unidad 2: mapa de entorno (cubemap)

// Otros uniforms
uniform int uDrawSky;           // 1 = dibujar skybox, 0 = dibujar objetos
uniform int uRenderMode;        // 0 = texturas, 1 = reflejo, 2 = refracción
uniform bool uChromatic;        // true = dispersión cromática
uniform bool uFresnel;          // true = ecuaciones de Fresnel
uniform float uEtaG;

uniform float uFresnelF; // Coeficiente de Fresnel (calculado a partir de uEtaG)
//uniform float uF; // Factor de mezcla Fresnel (calculado a partir de f y el ángulo)

// Datos interpolados del vertex shader
in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec3 vReflectDir;
in vec3 vRefractDir;
in vec3 vRefractDirR;
in vec3 vRefractDirG;
in vec3 vRefractDirB;
in vec3 vViewDir;

out vec4 fFragColor;

void main()
{
	// Dibuja el skybox (cubo del entorno)
	if (uDrawSky == 1)
	{
		// Para el skybox, las coordenadas de textura del cubemap son
		// la posición del vértice en el mundo (la dirección desde el centro)
		fFragColor = texture(uTexMap, vWorldPos);
		return;
	}

	if (uRenderMode == 0)
	{
		// Textura simple (stone)
		vec4 stoneColor = texture(uStoneTex, vTexCoord);

		// Multitextura (stone + moss)
		// La textura moss tiene canal alpha: usamos alpha para mezclar
		vec4 mossColor = texture(uMossTex, vTexCoord);
		// mix: cuando mossColor.a = 0 → solo piedra, cuando = 1 → solo musgo
		fFragColor = mix(stoneColor, mossColor, mossColor.a);
		// fFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else if (uRenderMode == 1)
	{
		// Reflejo - muestreamos el cubemap con el vector reflejo
		fFragColor = texture(uTexMap, vReflectDir);
	}
	else if (uRenderMode == 2)
	{
		vec4 refractColor;
		if (uChromatic)
		{
			// Dispersión cromática
			// Cada canal RGB se refracta con un ángulo diferente
			float refractR = texture(uTexMap, vRefractDirR).r;
			float refractG = texture(uTexMap, vRefractDirG).g;
			float refractB = texture(uTexMap, vRefractDirB).b;
			refractColor = vec4(refractR, refractG, refractB, 1.0);
		}
		else
		{
			// Refracción simple (un solo índice)
			refractColor = texture(uTexMap, vRefractDir);
		}

		if (uFresnel)
		{			
			vec3 N = normalize(vWorldNormal);
			vec3 V = normalize(vViewDir);

			// F = f + (1 - f) × (1 - V·N)^5
			float cosTheta = max(dot(V, N), 0.0); // Para evitar valores negativos
			float F = uFresnelF + (1.0 - uFresnelF) * pow(1.0 - cosTheta, 5.0);

			vec4 reflectColor = texture(uTexMap, vReflectDir);
			fFragColor = mix(refractColor, reflectColor, F);
		}
		else
		{
			fFragColor = refractColor;
		}
	}
}
