#version 120
attribute vec3 vertexPos;
uniform mat4 MVP;

void main() {
	vec4 v = vec4(vertexPos, 1);
	gl_Position = MVP * v;
}
