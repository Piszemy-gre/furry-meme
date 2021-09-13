#version 440 core

uniform sampler2DArray tex;
uniform int textureId;

in vec2 fragTexCoord;
out vec4 color;

void main() {
    color = vec4(texture(tex, vec3(fragTexCoord, textureId)).rgb, 1);
}