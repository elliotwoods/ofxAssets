#version 150

uniform sampler2DRect tex0;

in vec2 texCoordVarying;
out vec4 outputColor;

uniform float minimum;
uniform float maximum;

void main() {
	vec4 color = texture(tex0, texCoordVarying);
	color.rgb = 1.0 - color.rgb;
	outputColor = color;
}
