#version 150 

in vec3 vTextCoordCuboEntorno;
in vec3 vNormal;
in vec3 vPosition;
in vec3 vAmbientLight;

in vec3 vReflectDir; // Para ser usado en los reflejos del mapa de reflejos (parte especular de IBL y SPH)

uniform samplerCube uEnvironmentMap;   // textura 0 – mapa de entorno
uniform samplerCube uSpecularMap;      // textura 1 – mapa especular  (4.4)
uniform samplerCube uIrradianceMap;    // textura 2 – mapa de irradiación difusa (4.3)

uniform vec3 uCameraPos;
uniform int uDrawEnvironment;

// 4.1a
uniform int uAmbientOnlyMode;  // 1 = sólo luz ambiente (sin difusa ni especular)

uniform int uSpecularActivated;

struct LightInfo {
	vec3 lightDir; // Dirección de la luz (S.R. mundo)
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

// 4.2: modo armónicos esféricos
uniform int  uSphHarmonicsMode;  // 1 = activo
uniform mat4 uMred;
uniform mat4 uMgreen;
uniform mat4 uMblue;

// Ejercicio 4.3
uniform int uIBLMode;  // 1 = activo

out vec4 fFragColor;

// Factor de escala por los que hay que multiplicar el resultado de la ilum. basada en imágenes (difusa)
const float IBL_Scale = 1.2;
// Factor de escala por los que hay que multiplicar el resultado de la ilum. basada en armónicos (difusa)
const float SPH_Scale = 0.90;

// Dado el vector normal n = (x,y,z,1), la luz difusa para un canal es:
//   L = n^T · M · n   (con la normal homogénea)
vec3 sphericalHarmonicsLight(vec3 n)
{
    vec4 nh = vec4(n, 1.0);
    float r = dot(nh, uMred * nh);
    float g = dot(nh, uMgreen * nh);
    float b = dot(nh, uMblue * nh);
    return max(vec3(r, g, b), vec3(0.0));
}

void main()
{
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 norm = normalize(vNormal);

	if ( uDrawEnvironment == 1 )
	{
		fFragColor = texture(uEnvironmentMap, vTextCoordCuboEntorno); 
		return;
	}

    if (uSphHarmonicsMode == 1)
    {
        // Parte difusa SPH
        vec3 sphLight = sphericalHarmonicsLight(norm) * SPH_Scale;
        vec3 color = uMaterial.diffuse * sphLight;

        if (uSpecularActivated == 1)
        {
            vec3 reflColor = vec3(texture(uSpecularMap, vReflectDir));
            color += uMaterial.specular * reflColor;
        }

        fFragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
        return;
    }

    if (uIBLMode == 1)
    {
        // Parte difusa IBL: muestreamos el mapa de irradiación con la normal
        vec3 iblLight = vec3(texture(uIrradianceMap, norm)) * IBL_Scale;
        vec3 color = uMaterial.diffuse * iblLight;

        if (uSpecularActivated == 1)
        {
            vec3 reflColor = vec3(texture(uSpecularMap, vReflectDir));
            color += uMaterial.specular * reflColor;
        }

        fFragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
        return;
    }

    ambient = uMaterial.ambient * vAmbientLight;
    // 4.1a
    // Si uAmbientOnlyMode == 1, sólo se aplica la parte ambiente.
    if (uAmbientOnlyMode == 0)
    {
        // Parte difusa
        vec3 ldir = normalize(uLight.lightDir);
        diffuse = uMaterial.diffuse * uLight.intensity * max(dot(ldir, norm), 0.0);

        // Parte especular (tecla 'E')
        if (uSpecularActivated == 1)
        {
            vec3 view = normalize(uCameraPos - vPosition);
            vec3 refl = normalize(reflect(-ldir, norm));
            specular = uMaterial.specular * uLight.intensity * pow(max(dot(refl, view), 0.0), uMaterial.shininess);
        }
    }

		fFragColor = vec4(ambient + diffuse + specular, 1.0); 
        // fFragColor = vec4(clamp(ambient + diffuse + specular, 0.0, 1.0), 1.0); 
}