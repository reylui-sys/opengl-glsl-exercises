#version 150

in vec3 vColor; // Color calculado en el vertex shader (modo Phong)
flat in vec3 vColorFlat; // Color calculado en el vertex shader (modo plano)
in vec3 vPosition; // Posición del vértice en el S.R. vista (modo 2)
in vec3 vNormal; // Normal del vértice en el S.R. vista (modo 2)

uniform int uShadingMode; // 0=suave, 1=plano
uniform int uLightMode; // 0=punta, 1=foco

out vec4 fragColor; // Color de salida

// Añado para el modo 2
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

// Reutilizo ADS
vec3 ads(in vec3 pos, in vec3 n)
{
	vec3 ldir = normalize( vec3(uLight.lightPos) - pos );
	vec3 vdir = normalize( vec3(-pos) );
	vec3 r = reflect(-ldir, n);
	vec3 color;
    float d = length(vec3(uLight.lightPos) - pos);
    float attenuation = 1.0 / (uLight.k[0] + uLight.k[1]*d + uLight.k[2]*d*d);
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

// Funcion para el calculo de la luz modo comic en la luz puntual
vec3 comic(in vec3 pos, in vec3 n)
{
    vec3 ldir = normalize(vec3(uLight.lightPos) - pos);
    vec3 vdir = normalize(-pos);
    vec3 r = reflect(-ldir, n);

    // Contorno: normal casi perpendicular a la vista
    if (dot(vdir, n) < 0.14)
        return vec3(0.0);  // negro

    // Zona especular brillante
    if (dot(r, vdir) > 0.94)
        return uMaterial.specular + uMaterial.diffuse;

    // Zona difusa iluminada
    if (dot(ldir, n) > 0.0)
        return uMaterial.diffuse;

    // Zona en sombra
    return uMaterial.ambient * 1.5;
}

// Funcion para el calculo de la luz modo comic en la luz del foco
vec3 comicSpot(in vec3 pos, in vec3 n)
{
    vec3 ldir = normalize(vec3(uLight.lightPos) - pos);
    vec3 vdir = normalize(-pos);
    vec3 r = reflect(-ldir, n);
    vec3 spotDir = normalize(uLight.dir);

    float cosAngle = dot(-ldir, spotDir);
    float cosExt = cos(radians(uLight.cutOffExt));

    // Contorno: siempre negro, independientemente del cono
    if (dot(vdir, n) < 0.14)
        return vec3(0.0);

    // Fuera del cono exterior: solo ambiente
    if (cosAngle < cosExt)
        return uMaterial.ambient * 1.5;

    // Dentro del cono, misma lógica que el puntual
    if (dot(r, vdir) > 0.94)
        return uMaterial.specular + uMaterial.diffuse;

    if (dot(ldir, n) > 0.0)
        return uMaterial.diffuse;

    return uMaterial.ambient * 1.5;
}

void main()
{
    vec3 color;
    if (uShadingMode == 0) {
        color = vColor; // Modo suave: usar el color interpolado
    } else if (uShadingMode == 1) {
        color = vColorFlat; // Modo plano: usar el color del último vértice
    }
    else if (uShadingMode == 2) // Modo 2: calcular el color en el fragment shader
    {
        vec3 n = normalize(vNormal);
        if (uLightMode == 0) {
            color = ads(vPosition, n); // Luz puntual
        } else
            color = adsSpot(vPosition, n); // Foco
    }
    else // Modo 3: calcular el color en el fragment shader
    {
        vec3 n = normalize(vNormal);
        if (uLightMode == 0) {
            color = comic(vPosition, n); // Luz puntual
        } else
            color = comicSpot(vPosition, n); // Foco
    }

    gl_FragColor = vec4(color, 1.0);
}