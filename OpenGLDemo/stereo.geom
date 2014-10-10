#version 410

layout(triangles, invocations = 2) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 v_texCoord[];
in vec3 v_normal[];
in vec3 v_eyeVec[];
in vec3 v_lightDir[];

uniform mat4 modelViewProjectionMatrix[2];

out vec2 f_texCoord;
out vec3 f_normal;
out vec3 f_eyeVec;
out vec3 f_lightDir;

void main(void) {
	for (int i = 0; i < gl_in.length(); ++i) {
		// Pass-through properties
		f_texCoord = v_texCoord[i];
		f_normal = v_normal[i];
		f_eyeVec = v_eyeVec[i];
		f_lightDir = v_lightDir[i];

		gl_Position = modelViewProjectionMatrix[gl_InvocationID] * gl_in[i].gl_Position;
		gl_ViewportIndex = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}
