#version 430  

// Le asignamos el ID directamente para no tener que localizar el atributo en el código de la aplicación
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

// Idem con las variables uniform
layout(location = 0) uniform mat4 uModelViewProjMatrix;
layout(location = 1) uniform mat4 uModelViewMatrix;
layout(location = 2) uniform mat3 uNormalMatrix;

out vec3 vECNorm; // S.R. Vista para el GS y la iluminación (FS)
out vec3 vColor; // Color del vértice

void main()
{
	vECNorm = normalize(uNormalMatrix * aNormal); // Normal en SR vista normalizada
	vColor = abs(normalize(aNormal));

	// Tarea 2.1: generar la posición del v�rtice en el S.R. de la vista 
	// (en lugar del espacio de la ventana, como ahora)
	// gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);

    // El GS hace la proyección, permite calcular el baricentro en vista antes de proyectar
    gl_Position = uModelViewMatrix * vec4(aPosition, 1.0);
}