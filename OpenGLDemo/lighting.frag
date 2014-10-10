uniform sampler2D s_texture;
varying vec2 texCoord;

varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir;

void main()
{
	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
    float nDotVP = dot(N, L);

	float ambient = 0.2;
	float diffuse = max(0.0, nDotVP);

	vec4 col = (ambient + diffuse) * texture2D(s_texture, texCoord);
    gl_FragColor = col;
}
