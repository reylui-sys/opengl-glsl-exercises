#version 150 

in vec3 vECPos; // S.R. Vista
in vec3 vECNorm; // S.R. Vista

// Coordenadas proyectivas del mapa de sombras (interpoladas)
in vec4 vShadowCoord;

out vec4 fFragColor;

uniform int uDrawingShadowMap;

// Sampler de sombras 
uniform sampler2DShadow uShadowMap;

// modo PCF (0=NEAREST, 1=PCF4-LINEAR, 2=PCF16, 3=PCF configurable)
uniform int uPCFMode;

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
	vec3 intensity;
};
uniform LightInfo uLight;
struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo uMaterial;

// Constantes para el PCF configurable
const int DESP = 2;   // amplitud del barrido
const int INC  = 1;   // densidad del barrido

vec3 phongModelDiffAndSpec () 
{
	vec3 ldir = normalize(vec3(uLight.lightPos) - vECPos);
	vec3 view = normalize(vec3(-vECPos));
	vec3 r = reflect(-ldir,vECNorm);

	vec3 color = uLight.intensity * ( uMaterial.diffuse * max(dot(ldir,vECNorm), 0.0) +
									  uMaterial.specular * pow(max(dot(r,view),0),uMaterial.shininess) );

	return color;
}

void main()
{
	// Si se está dibujando el shadow map, solo se escribe profundidad, no se escribe color
	if (uDrawingShadowMap == 1)
	{
		return;
	}

	vec3 ambient = uLight.intensity * uMaterial.ambient;
	vec3 diffAndSpec = phongModelDiffAndSpec();

	// Consulta el mapa de sombras
	float shadow = 1.0;

	if (uPCFMode == 0 || uPCFMode == 1)
	{
		// (modo 0): GL_NEAREST - 1 muestra, resultado binario 0/1
		// (modo 1): GL_LINEAR  - 4 muestras, resultado suavizado
		// textureProj hace la comparación automática
		shadow = textureProj(uShadowMap, vShadowCoord);
	}
	else if (uPCFMode == 2)
	{
		// PCF con 16 muestras
		// Con GL_LINEAR activo, cada textureProjOffset hace internamente 4 muestras -> 4 llamadas x 4 samples = 16 muestras totales
		float sum = 0.0;
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(-1, -1));
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2(-1,  1));
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2( 1,  1));
		sum += textureProjOffset(uShadowMap, vShadowCoord, ivec2( 1, -1));
		shadow = sum * 0.25;
	}
	else if (uPCFMode == 3)
	{
		// PCF configurable con DESP e INC
		float texelSize = 1.0 / float(textureSize(uShadowMap, 0).x);
		float sum  = 0.0;
		int cont = 0;

		for (int i = -DESP; i <= DESP; i += INC)
			for (int j = -DESP; j <= DESP; j += INC)
			{
				vec4 offset = vec4(float(i) * texelSize * vShadowCoord.w,
				                   float(j) * texelSize * vShadowCoord.w,
				                   0.0, 0.0);
				sum += textureProj(uShadowMap, vShadowCoord + offset);
				cont++;
			}

		shadow = sum / float(cont);
	}

	// La sombra solo afecta a difusa+especular, no a ambiente
	fFragColor = vec4( clamp(ambient + shadow * diffAndSpec, 0.0, 1.0), 1.0 );
}