#version 150

in vec3 aPosition; // Posición del vértice en el S.R. local al objeto
in vec3 aNormal; // Normal del vértice en el S.R. local al objeto
in vec2 aTexCoord;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
	vec3 intensity;
	vec3 k;
	vec3 dir; // Dirección del foco (S.R. de la vista)
	float cutOffInt; // Ángulo interior (límite zona iluminada)
	float cutOffExt; // Ángulo exterior (límite zona penumbra)
};
uniform LightInfo uLight;

struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo uMaterial;

uniform int uLightMode; // 0=punta, 1=foco

out vec3 vColor;
flat out vec3 vColorFlat; // Flat para que no se interpole el color entre vértices, sino que se tome el del último vértice del triángulo
// Es el equivalente a usar el GL_FLAT en OpenGL fijo
out vec3 vPosition;
out vec3 vNormal;

// ADS para la luz puntual
// Parámetros:	pos - posición del vértice en el S.R. de la vista
//				n - normal del vértice en el S.R. de la vista
vec3 ads(in vec3 pos, in vec3 n)
{
	vec3 ldir = normalize( vec3(uLight.lightPos) - pos );
	vec3 vdir = normalize( vec3(-pos) );
	vec3 r = reflect(-ldir, n);

	// Atenuación con la distancia
    float d = length(vec3(uLight.lightPos) - pos);
    float attenuation = 1.0 / (uLight.k[0] + uLight.k[1]*d + uLight.k[2]*d*d);

	vec3 color;
	color = attenuation * uLight.intensity * (   uMaterial.ambient 
								 + uMaterial.diffuse * max(dot(ldir,n), 0.0) 
								 + uMaterial.specular * pow(max(dot(r,vdir),0.0), uMaterial.shininess)
							   );

	return color;
}

vec3 adsSpot(in vec3 pos, in vec3 n)
{
    vec3 ldir = normalize(vec3(uLight.lightPos) - pos);
    vec3 vdir = normalize(-pos);
    vec3 r = reflect(-ldir, n);
    vec3 spotDir = normalize(uLight.dir);

    // Coseno del ángulo entre dirección del foco y vector al vértice
    float cosAngle = dot(-ldir, spotDir);

    // Cosenos de los ángulos límite
    float cosInt = cos(radians(uLight.cutOffInt));
    float cosExt = cos(radians(uLight.cutOffExt));

    // Factor de atenuación: 0 fuera, 1 dentro, suave entre medias
    float spotFactor = smoothstep(cosExt, cosInt, cosAngle);

    // Atenuación con la distancia
    float d = length(vec3(uLight.lightPos) - pos);
    float attenuation = 1.0 / (uLight.k[0] + uLight.k[1]*d + uLight.k[2]*d*d);

    vec3 ads = attenuation * uLight.intensity * (
          uMaterial.ambient
        + uMaterial.diffuse  * max(dot(ldir, n), 0.0)
        + uMaterial.specular * pow(max(dot(r, vdir), 0.0), uMaterial.shininess)
    );

    // Fuera del cono solo llega el ambiente
    vec3 ambient = uLight.intensity * uMaterial.ambient;
    return mix(ambient, ads, spotFactor);
}

void main()
{
	vec3 ecPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0)); // Posición en el S.R. vista
	vec3 ecNorm = normalize(uNormalMatrix * aNormal); // Normal en el S.R. vista

	vec3 colorCalculado;
	if (uLightMode == 0) {
		colorCalculado = ads(ecPos, ecNorm); // Luz puntual
	} else {
		colorCalculado = adsSpot(ecPos, ecNorm); // Foco
	}

	// Me sale el mismo resultado que sin clamp
	vColor = clamp(colorCalculado, 0.0, 1.0);
	vColorFlat = clamp(colorCalculado, 0.0, 1.0);

	// Para el modo 2
	vPosition = ecPos;
	vNormal = ecNorm;

	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
}