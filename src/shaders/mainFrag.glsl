#version 440 core

in vec2 fragTexCoord;

uniform sampler2DArray tex;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTexCord;
uniform usampler2D gTexId;

out vec4 fragColor;

void main() {
    uint texId = texture(gTexId, fragTexCoord).r;
    if(texId == 0)
        discard;
    texId -= 1;
    vec2 texCoord = texture(gTexCord, fragTexCoord).rg;
    //fragColor = vec4(texId, 0,0,1);
    //fragColor = texture(tex, vec3(texCoord, texId));

    fragColor = vec4(texId, 0, 0, 1);

    vec4 color = texture(tex, vec3(texCoord, float(texId)));
    color.a = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    fragColor = color;
}