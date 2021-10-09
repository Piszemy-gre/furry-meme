#version 440 core
#define UINT16_MAX 65535

uniform sampler2DArray tex;

in vec3 fragWorldPosition;
in vec2 fragTexCoord;
flat in uint fragTexId;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec2 gTexCord;
layout (location = 3) out uint  gTexId;

void main() {
    gPosition = fragWorldPosition;
    //gNormal = texture(tex, vec3(fragTexCoord, fragTexId)).rgb;
    gTexCord = fragTexCoord;
    gTexId = fragTexId + 1;
}