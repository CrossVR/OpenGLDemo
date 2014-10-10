#version 130

uniform sampler2D s_texture;

in vec2 f_texCoord;
in vec3 f_normal;
in vec3 f_eyeVec;
in vec3 f_lightDir;

out vec4 fragColor;

void main()
{
	vec3 N = normalize(f_normal);
	vec3 L = normalize(f_lightDir);
    float nDotVP = dot(N, L);

	float specular = 0.0;

	if (nDotVP > 0.0)
	{
		vec3 E = normalize(f_eyeVec);
		vec3 R = reflect(-L, N);
		specular =  pow( max(dot(R, E), 0.0), 10.0 );
	}

	float ambient = 0.2;
	float diffuse = max(0.0, nDotVP);
	
	float fac = ambient + specular + diffuse;

	float roundFac = round(fac*2)/2;

	if(fac + 0.2 < roundFac)
	{
		fragColor = vec4(0,0,0,1);
	}
	else
	{
		fragColor = roundFac * texture2D(s_texture, f_texCoord);
	}
}
