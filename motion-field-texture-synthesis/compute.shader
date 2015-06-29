R"(
#version 440 core

layout (rgba8ui, binding = 0) uniform uimage2D motion;
layout (rgba32f, binding = 1) uniform image1D position;
layout (rgba32f, binding = 2) uniform image1D velocity;
layout (rgba32i, binding = 3) uniform iimage1D control;
layout (rgba8ui, binding = 4) uniform uimage2D display;

layout (local_size_x = 256) in;

void main() {
	 vec2 pos = 512 * imageLoad(position, int(gl_GlobalInvocationID.x)).xy;
	 imageStore(display, ivec2(pos), uvec4(255, 255, 255, 0));
	 //uvec2 coord = imageLoad(motion, ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.x)).xy;
	 //imageStore(display, ivec2(coord), uvec4(255, 255, 255, 0));
	 //ivec2 pos = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
	 //int index = int(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y);
	 //imageStore(position, index, vec4(0.5, 0, 0.5, 1));
	 //vec4 pixel = imageLoad(position, index) * 256;
	 //imageStore(display, pos, uvec4(pixel.r, pixel.g, pixel.b, pixel.a));
}
)"
