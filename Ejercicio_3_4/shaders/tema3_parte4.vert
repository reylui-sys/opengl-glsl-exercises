#version 150  

in vec3 aPosition;
in vec3 aNormal;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

uniform int uDrawEnvironment;
uniform vec3 uCameraPos;
uniform vec3 uAmbientLight;

// 4.1b - Colores de la luz de hemisferio
uniform vec3 uGroundLight;   // color de la luz proveniente del suelo
uniform vec3 uSkyLight;      // color de la luz proveniente del cielo
uniform int  uHemisphereMode; // 0 = luz ambiente plana, 1 = hemisferio

out vec3 vNormal;
out vec3 vPosition;
out vec3 vAmbientLight; 
out vec3 vTextCoordCuboEntorno;

out vec3 vReflectDir; // Para ser usado en los reflejos del mapa de reflejos (parte especular de IBL y SPH)


void main()
{
    vec4 pos = vec4(aPosition, 1.0);

	vNormal = normalize(uNormalMatrix * aNormal);
	vPosition = vec3(uModelMatrix * pos);
	vReflectDir = reflect(vPosition - uCameraPos, vNormal);
    // vReflectDir = reflect(vec3(vPosition) - uCameraPos, vNormal); Duda del autocompletar

	if (uDrawEnvironment == 1)
		vTextCoordCuboEntorno = aPosition;
	else
    {
        // 4.1b
        // Interpolamos entre groundLight y skyLight t = (dot(N, up) + 1) / 2   → t ∈ [0,1]
        if (uHemisphereMode == 1) // Se añade este if para el nuevo modo de ambiente hemisferio
        {
            vec3 up = vec3(0.0, 1.0, 0.0);
            float t = (dot(vNormal, up) + 1.0) * 0.5;
            vAmbientLight = mix(uGroundLight, uSkyLight, t);
        }
        else
        {
            // Luz ambiente original
            vAmbientLight = uAmbientLight;
        }
    }
	
	gl_Position = uModelViewProjMatrix * pos;
}