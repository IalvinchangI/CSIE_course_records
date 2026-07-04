#version 330 core

out vec4 fragColor;

in VertexToFragment
{
    vec3 worldPosition;
    vec3 normal;
    vec2 textureCoordinate;
}
vertexToFragment;

uniform sampler2D objectTexture;
uniform vec2 textureOffset;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    // texture offset
    vec2 movedUV = vertexToFragment.textureCoordinate + textureOffset;
    vec4 texColor = texture(objectTexture, movedUV);

    // light
    //// ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    //// diffuse
    vec3 norm = normalize(vertexToFragment.normal);
    vec3 lightDir = normalize(lightPos - vertexToFragment.worldPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // result
    vec3 result = (ambient + diffuse) * texColor.rgb;
    fragColor = vec4(result, texColor.a);
}
