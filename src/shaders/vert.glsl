#version 440 core

//uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in int texId;

out vec3 fragWorldPosition;
out vec2 fragTexCoord;
flat out uint fragTexId;

void main() {
    gl_Position = P * V * /*M*/ vec4(position, 1);
    fragTexCoord = texCoord;
    fragWorldPosition = position;
    fragTexId = texId;
}