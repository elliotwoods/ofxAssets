#version 150

uniform sampler2DRect texture;

in vec2 texCoordVarying;
out vec4 outputColor;

uniform float minimum;
uniform float maximum;

void main() {
	vec4 color = texture2DRect(texture, texCoordVarying);
	color.rgb = 1.0 - color.rgb;
	outputColor = color;
}
