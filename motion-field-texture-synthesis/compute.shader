R"(
#version 440 core

layout (rgba8ui, binding = 0) uniform uimage2D motion;
layout (rgba32f, binding = 1) uniform image2D position;
layout (rgba32f, binding = 2) uniform image2D velocity;
layout (rgba32i, binding = 3) uniform iimage2D control;
layout (rgba8ui, binding = 4) uniform uimage2D display;

layout (local_size_x = 16, local_size_y = 16) in;

void main() {
	 ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	 imageStore(display, pos, imageLoad(motion, pos));
	 //imageStore(display,  pos, uvec4(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z, 0));
}
)"
