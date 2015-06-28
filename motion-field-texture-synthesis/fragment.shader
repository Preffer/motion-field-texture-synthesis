R"(
#version 440 core

out vec4 color;

layout(binding = 4) uniform sampler2D display;
ivec2 size = ivec2(512, 512);

void main() {
	color = texture(display, gl_FragCoord.xy / size);
	//color = vec3(gl_FragCoord.x / 512, gl_FragCoord.y / 512,  gl_FragCoord.z / 512);
}
)"
