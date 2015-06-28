R"(
#version 440 core

out uvec4 color;

layout(binding = 4) uniform usampler2D display;
uniform ivec2 size;

void main() {
	color = texture(display, gl_FragCoord.xy / size);
}
)"
