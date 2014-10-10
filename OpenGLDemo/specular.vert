attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec3 a_normal;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

varying vec2 texCoord;

varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir;

void main()
{
    vec3 lightPosition = vec3(0.0, 0.0, 100.0);
	
	vec4 transformed = modelViewMatrix * a_position;


    normal = normalMatrix * a_normal;
	lightDir = lightPosition-vec3(transformed);
	eyeVec = -vec3(transformed);

    texCoord = a_texCoord;	
    gl_Position = modelViewProjectionMatrix * a_position;
}
