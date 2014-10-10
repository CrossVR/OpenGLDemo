attribute vec4 a_position;
attribute vec2 a_texCoord;

uniform mat4 modelViewProjectionMatrix;

varying vec2 texCoord;

void main()
{
    texCoord = a_texCoord;	
    gl_Position = modelViewProjectionMatrix * a_position;
}
