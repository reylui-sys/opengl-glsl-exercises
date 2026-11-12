#version 150

uniform struct LightInfo {
	vec4 lightPos;
	vec3 intensity;
} uLight;

uniform struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} uMaterial;

// Samplers para las texturas
uniform sampler2D uNormalMap;   // Mapa de normales (GL_TEXTURE0)
uniform sampler2D uColorMap;    // Textura de color (GL_TEXTURE1)
uniform sampler2D uGlossMap;    // Mapa de brillos (GL_TEXTURE2)
uniform sampler2D uHeightMap;   // Mapa de alturas (GL_TEXTURE3)

// Variables de control
uniform int uUseNormalMap;    // 0=no usar, 1=usar mapa de normales
uniform int uUseColorMap;     // 0=no usar, 1=usar textura de color
uniform int uUseGlossMap;     // 0=no usar, 1=usar mapa de brillos
uniform int uParallaxMode;    // 0=sin parallax, 1=simple, 2=iterativo

// Inputs desde vertex shader
in vec2 vTexCoord;   // Coordenadas de textura vST en las diapositivas
in vec3 vLightDir;   // Dirección de la luz en espacio tangente
in vec3 vViewDir;    // Dirección de la vista en espacio tangente

out vec4 fragColor;

// Función de iluminación ADS en espacio tangente
vec3 ads(in vec3 lightDir, in vec3 viewDir, in vec3 normal, in vec3 ka, in vec3 kd, in vec3 ks, in float shininess)
{
	vec3 L = normalize(lightDir);
	vec3 V = viewDir; // Ya está normalizado en el vertex shader, por usarlo en el paralaje
	vec3 N = normalize(normal);
	
	vec3 R = reflect(-L, N);
	
	vec3 ambient = ka;
	vec3 diffuse = kd * max(dot(L, N), 0.0);
	vec3 specular = ks * pow(max(dot(R, V), 0.0), shininess);
	
	return uLight.intensity * (ambient + diffuse + specular);
}

// Parallax Mapping Simple
vec2 parallaxMappingSimple(in vec2 texCoord, in vec3 viewDir)
{
	float scale = 0.05;
	float bias = -0.025;
	
	// Leer altura del mapa
	float height = texture(uHeightMap, texCoord).r;
	
	// Calcular desplazamiento. hsb en diapositivas
	float h = height * scale + bias;
	
	// Desplazar coordenadas de textura en dirección opuesta a la vista
	vec2 offset = viewDir.xy * h + texCoord; 
	
	return offset;
}

// Parallax Mapping Iterativo
vec2 parallaxMappingIterative(in vec2 texCoord, in vec3 viewDir)
{
	float scale = 0.05;
	float bias = -0.025;
	int N = 5; // Número de iteraciones
	
	// Inicialización
	float hsb1 = 1.0 * scale + bias; // Altura máxima
	vec2 ST1 = texCoord + hsb1 * viewDir.xy; // Coordenada de textura máxima (despl)
	
	float hsb2 = 0.0 * scale + bias; // Altura minima
	vec2 ST2 = texCoord + hsb2 * viewDir.xy; // Coordenada de textura mínima (despl)
	
	vec2 ST = texCoord; // Coordenada de textura inicial
	float hsb = 0.0; // Altura
	
	// Búsqueda binaria
	for (int i = 0; i < N; i++)
	{
		ST = (ST1 + ST2) / 2.0; // Punto medio
		hsb = (hsb1 + hsb2) / 2.0; // Altura media
		
		float h = texture(uHeightMap, ST).r * scale + bias; // Altura "final" calculada
		
		// Si la altura calculada es menor que la media, se actualiza el valor mínimo
		// Si la altura calculada es mayor que la media, se actualiza el valor máximo
		// Se repite el bucle n veces
		if (h < hsb)
		{
			ST1 = ST;
			hsb1 = hsb;
		}
		else
		{
			ST2 = ST;
			hsb2 = hsb;
		}
	}
	
	return ST;
}

void main()
{
	// Coordenadas de textura (pueden modificarse con parallax)
	vec2 texCoord = vTexCoord;
	vec3 viewDirNorm = normalize(vViewDir);
	
	// Aplicar parallax mapping si está activo
	if (uParallaxMode == 1) {
		texCoord = parallaxMappingSimple(texCoord, viewDirNorm);
	} else if (uParallaxMode == 2) {
		texCoord = parallaxMappingIterative(texCoord, viewDirNorm);
	}
	
	// Obtener normal del mapa de normales o usar (0,0,1)
	vec3 normal;
	if (uUseNormalMap == 1) {
		// Leer del mapa de normales y transformar de [0,1] a [-1,1]
		normal = texture(uNormalMap, texCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
	} else {
		// Normal por defecto en espacio tangente
		normal = vec3(0.0, 0.0, 1.0);
	}
	
	// Obtener propiedades del material
	vec3 ka, kd, ks;
	float shininess;
	
	if (uUseColorMap == 1)
	{
		// Usar textura de color
		vec3 texColor = texture(uColorMap, texCoord).rgb;
		ka = texColor * 0.2;  // Ambiente = color * 0.2
		kd = texColor;         // Difusa = color
		ks = vec3(0.5);        // Especular fijo
		shininess = uMaterial.shininess;
	}
	else
	{
		// Usar propiedades del material
		ka = uMaterial.ambient;
		kd = uMaterial.diffuse;
		ks = uMaterial.specular;
		shininess = uMaterial.shininess;
	}
	
	// Si se usa mapa de brillos, modular el especular
	if (uUseGlossMap == 1)
	{
		float gloss = texture(uGlossMap, texCoord).r;
		ks = vec3(gloss);
	}
	
	// Calcular iluminación
	vec3 color = ads(vLightDir, viewDirNorm, normal, ka, kd, ks, shininess);
	
	fragColor = vec4(color, 1.0);
}
