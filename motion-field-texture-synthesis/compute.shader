R"(
#version 440 core

layout (rgba8ui, binding = 0) uniform uimage2D motion;
layout (rgba32f, binding = 1) uniform image2D position;
layout (rgba32f, binding = 2) uniform image2D velocity;
layout (rgba32i, binding = 3) uniform iimage2D control;
layout (rgba8ui, binding = 4) uniform uimage2D display;

uniform ivec2 size;
layout (local_size_x = 16, local_size_y = 16) in;

const float step = 0.005;

int seed = int(gl_GlobalInvocationID.x);

int rand() {
	seed = (seed << 13) ^ seed;
	seed = (seed * (seed*seed*15731+789221) + 1376312589) & 0x7fffffff;
    return seed;
}

void main() {
	 ivec2 index = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
	 vec4 coord  = imageLoad(position, index);
	 vec2 speed  = imageLoad(velocity, index).xy;

	 if (coord.w < step || clamp(coord.xy, vec2(0, 0), vec2(1, 1)) != coord.xy){
		coord = vec4(0.5, 0.5, 0, rand() / 1073741824.0);
		speed = vec2(rand() / 1073741824.0, rand() / 1073741824.0);
	 } else {
		coord.w -= step;
	 }

	 vec2 pos = coord.xy * size;
	 vec2 force  = vec2(imageLoad(motion, ivec2(pos)).xy) / 255 - vec2(0.5, 0.5);

	 imageStore(display, ivec2(pos), uvec4(uvec4(255, 255, 255, 0) * coord.w));
	
	 pos = (pos + - vec2(0.5, 0.5)) / size;
	 speed += force / 10;

	 imageStore(position, index, vec4(pos.x, pos.y, 0, coord.w));
	 imageStore(velocity, index, vec4(speed.x, speed.y, 0, 0));
}
)"
