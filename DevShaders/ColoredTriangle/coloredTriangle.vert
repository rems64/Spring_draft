#version 450

layout(location = 0) in vec3 vertexLoc;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 offset;

layout(location = 0) out vec3 col;

void main() {
    gl_Position = vec4(vertexLoc.xy+offset, vertexLoc.z, 1.0);
    col = color;
}