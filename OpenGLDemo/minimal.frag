uniform sampler2D s_texture;
varying vec2 texCoord;


void main()
{
    vec4 col = texture2D(s_texture, texCoord);
    gl_FragColor = col;
}
