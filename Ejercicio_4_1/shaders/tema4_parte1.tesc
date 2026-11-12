#version 400

layout (vertices = 4) out;

// Variable de color recibida del vertex shader (array, uno por punto de control)
in vec3 vColor[];

// Variable de color pasada al TES (array)
out vec3 tcColor[];

// Uniforms para matrices (necesarios para calcular nivel en funcion del punto de vista)
uniform mat4 uModelViewMatrix;

// Constante de teselacion fija
const int uTessLevel = 4;

// Parametros para teselacion dependiente del punto de vista
const float uMinDepth = 3.0;
const float uMaxDepth = 15.0;
const float uMinTessLevel = 4.0;
const float uMaxTessLevel = 32.0; // 64.0

// Modo de teselacion: 0=fijo (1.1), 1=variable global (1.2), 2=variable por parche (1.3), 3=sin grietas (1.4)
uniform int uTessMode = 0;

// Funcion que calcula el nivel de teselacion en funcion de la distancia de un punto (en S.R. local del objeto) al punto de vista
float level(in vec4 pos)
{
	vec4 p = uModelViewMatrix * pos;
	float depth = clamp( (abs(p.z) - uMinDepth) / (uMaxDepth - uMinDepth), 0.0, 1.0);
	return mix(uMaxTessLevel, uMinTessLevel, depth);
}

void main()
{
	// Pasa posiciones de entrada a salida (los TCS se ejecuta 1 vez por punto de control de salida)
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	// Pasar el color al TES
	tcColor[gl_InvocationID] = vColor[gl_InvocationID];

	// nivel de teselacion fijo e igual a uTessLevel
	if (uTessMode == 0)
	{
		gl_TessLevelOuter[0] = float(uTessLevel);
		gl_TessLevelOuter[1] = float(uTessLevel);
		gl_TessLevelOuter[2] = float(uTessLevel);
		gl_TessLevelOuter[3] = float(uTessLevel);
		gl_TessLevelInner[0] = float(uTessLevel);
		gl_TessLevelInner[1] = float(uTessLevel);
	}
	else if (uTessMode == 1) // nivel variable, global para toda la esfera, a partir del centro de la esfera (0,0,0) en S.R. local
	{
		float tl = level(vec4(0.0, 0.0, 0.0, 1.0));
		gl_TessLevelOuter[0] = tl;
		gl_TessLevelOuter[1] = tl;
		gl_TessLevelOuter[2] = tl;
		gl_TessLevelOuter[3] = tl;
		gl_TessLevelInner[0] = tl;
		gl_TessLevelInner[1] = tl;
	}
	else if (uTessMode == 2) // nivel variable por parche,  partir del centro del parche (promedio de los 4 puntos de control)
	{
		vec4 center = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position + gl_in[3].gl_Position) * 0.25;
		float tl = level(center);
		gl_TessLevelOuter[0] = tl;
		gl_TessLevelOuter[1] = tl;
		gl_TessLevelOuter[2] = tl;
		gl_TessLevelOuter[3] = tl;
		gl_TessLevelInner[0] = tl;
		gl_TessLevelInner[1] = tl;
	}
	else if (uTessMode == 3) // nivel variable por LADO del parche, sin grietas
	{
		// Centro de cada lado
		vec4 mid01 = (gl_in[0].gl_Position + gl_in[1].gl_Position) * 0.5;
		vec4 mid12 = (gl_in[1].gl_Position + gl_in[2].gl_Position) * 0.5;
		vec4 mid23 = (gl_in[2].gl_Position + gl_in[3].gl_Position) * 0.5;
		vec4 mid30 = (gl_in[3].gl_Position + gl_in[0].gl_Position) * 0.5;

		gl_TessLevelOuter[0] = level(mid30);
		gl_TessLevelOuter[1] = level(mid01);
		gl_TessLevelOuter[2] = level(mid12);
		gl_TessLevelOuter[3] = level(mid23);

		// El nivel interior se calcula como el maximo de los 4 lados para que no haya discontinuidades en el interior del parche
		float innerLevel = max(max(gl_TessLevelOuter[0], gl_TessLevelOuter[1]), max(gl_TessLevelOuter[2], gl_TessLevelOuter[3]));
		gl_TessLevelInner[0] = innerLevel;
		gl_TessLevelInner[1] = innerLevel;
	}
}
