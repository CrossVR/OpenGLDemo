#version 130

in vec4 a_position;
in vec2 a_texCoord;
in vec3 a_normal;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_eyeVec;
out vec3 v_lightDir;

void main()
{
    vec3 lightPosition = vec3(0.0, 0.0, 100.0);
	
	vec4 transformed = modelViewMatrix * a_position;


    v_normal = normalMatrix * a_normal;
	v_lightDir = lightPosition-vec3(transformed);
	v_eyeVec = -vec3(transformed);

    v_texCoord = a_texCoord;	
    gl_Position = a_position;
}
