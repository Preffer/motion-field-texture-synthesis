R"(#version 440 core

layout (rgba8ui, binding = 0) uniform uimage2D motion;
layout (rgba32f, binding = 1) uniform image3D position;
layout (rgba32f, binding = 2) uniform image3D velocity;
layout (rgba32f, binding = 3) uniform image3D control;
layout (rgba8ui, binding = 4) uniform uimage2D display;

uniform int depth;
uniform int batch;
uniform ivec2 size;
layout (local_size_x = 16, local_size_y = 16) in;

const float step = 0.0001;

vec2 rand(uint x, uint y, int z) {
	ivec3 pos = ivec3(x, y, z);
	vec4 pixel = imageLoad(control, pos);
	imageStore(control, pos, pixel.yzwx);
	return pixel.xy;
}

void main() {
	int i = 0;
	int done = 0;
	while (i < depth && done < batch) {
		ivec3 index = ivec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, i);
			
		vec4 coord = imageLoad(position, index);
		vec2 speed;

		if (coord.w < step || clamp(coord.xy, vec2(0, 0), vec2(1, 1)) != coord.xy) {
			done++;
			coord = vec4(0.5, 0.5, 0, 1);
			speed = rand(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, i);
		} else {
			// draw old
			coord.w -= step;
			speed = imageLoad(velocity, index).xy;
		}
		vec2 pos = coord.xy * size;

		imageStore(display, ivec2(pos), uvec4(mix(imageLoad(display, ivec2(pos)), uvec4(255, 255, 255, 0), coord.w)));

		vec2 force = vec2(imageLoad(motion, ivec2(pos)).xy) / 255 - vec2(0.5, 0.5);
		pos = (pos + speed) / size;
		speed += force / 10;

		imageStore(position, index, vec4(pos.x, pos.y, 0, coord.w));
		imageStore(velocity, index, vec4(speed.x, speed.y, 0, 0));
		
		i++;
	}
}
)"
