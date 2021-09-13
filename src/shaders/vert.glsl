#version 440 core

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 fragTexCoord;

void main() {
    gl_Position = P * V * vec4(position, 1);
    fragTexCoord = texCoord;
}