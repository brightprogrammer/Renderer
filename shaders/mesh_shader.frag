//glsl version 4.5
#version 450

// get fragment color
layout (location = 0) in vec3 inFragColor;
layout (location = 1) in vec3 inFragPosWorld;
layout (location = 2) in vec3 inFragNormalWorld;

// output write
layout (location = 0) out vec4 outFragColor;

// get uniform data
layout(set = 0, binding = 0) uniform UniformData {
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 lightPosition;
    float padding1;
    vec4 lightColor;
    vec3 viewPosition;
    float specularStrength;
} uniformData;

void main(){
    // calculate direction to light from vertex
    vec3 directionToLight = uniformData.lightPosition - inFragPosWorld;

    // calculate direction from vertex to viewer (camera)
    vec3 viewDir = normalize(uniformData.viewPosition - inFragPosWorld);

    // and reflect with given normal
    vec3 reflectDir = normalize(reflect(-directionToLight, inFragNormalWorld));

    // for point source we need to take into account the attenuation factor
    // attenuation is decaying of light intensity as distance from lightsource increases
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared

    // calculate light color after adjusting for intensity
    vec3 lightColor = uniformData.lightColor.xyz * uniformData.lightColor.w * attenuation;
    vec3 ambientLight = uniformData.ambientLightColor.xyz * uniformData.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalize(inFragNormalWorld), normalize(directionToLight)), 0);
    vec3 specularLight = uniformData.specularStrength * pow(max(dot(viewDir, reflectDir), 0), 32) * lightColor;

    outFragColor = vec4((ambientLight + diffuseLight + specularLight) * inFragColor, 1.f);
}
