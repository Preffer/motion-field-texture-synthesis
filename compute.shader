R"(#version 430

uniform float roll;
layout (rgba8ui) uniform uimage2D destTex;
layout (local_size_x = 16, local_size_y = 16) in;

void main() {
	 ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	 //float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);
	 //float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;
	 //imageStore(destTex, storePos, ivec4(1.0-globalCoef*localCoef, 0.0, 0.0, 0.0));
	 imageStore(destTex,  storePos, uvec4(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z, 0));
})"
