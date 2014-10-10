attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec3 a_normal;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform int timer;

varying vec2 texCoord;

varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir;

void main()
{
	float PI = 3.14159265358979323846264;

    vec3 lightPosition = vec3(0.0, 0.0, 100.0);
	
	vec4 transformed = modelViewMatrix * a_position;

	float normalTimer = (timer%50)/50.0;

	vec3 pos = vec3(transformed) + a_normal * vec3(sin(PI*normalTimer), sin(PI*normalTimer), sin(PI*normalTimer));

    normal = normalMatrix * a_normal;
	lightDir = lightPosition-pos;
	eyeVec = -pos;

    texCoord = a_texCoord;	
    gl_Position = modelViewProjectionMatrix * vec4(pos[0], pos[1], pos[2], transformed[3]);
}
