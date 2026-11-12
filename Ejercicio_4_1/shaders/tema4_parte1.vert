#version 400  

in vec3 aPosition;
in vec3 aNormal;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

out vec3 vECPos; // S.R. Vista
out vec3 vECNorm; // S.R. Vista
out vec3 vColor;

void main()
{
	// vECPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0));
	// vECNorm = normalize(uNormalMatrix * aNormal);
	// Solo pasamos el color basado en la normal del cubo original
	vColor = abs(normalize(aNormal));

	// La posicion NO se transforma: se pasa sin multiplicar por MVP
	// El TES sera el encargado de transformarla
	gl_Position = vec4(aPosition, 1.0);
}
