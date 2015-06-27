R"(#version 430

uniform sampler2D srcTex;
uniform ivec2 size;
in vec2 texCoord;
out vec4 color;

void main() {
	color = texture(srcTex, gl_FragCoord.xy / size);
})"
