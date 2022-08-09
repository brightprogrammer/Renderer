//glsl version 4.5
#version 450

// get fragment color
layout (location = 0) in vec3 inFragColor;
layout (location = 1) in vec3 inFragPosWorld;
layout (location = 2) in vec3 inFragNormalWorld;

// output write
layout (location = 0) out vec4 outFragColor;

#define MAX_LIGHTS 16

struct PointLight {
    vec4 position;
    vec4 color;
};

struct DirectionalLight {
    vec4 direction;
    vec4 color;
};

// get uniform data
layout(set = 0, binding = 0) uniform UniformData {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    vec4 ambient;
    vec3 viewPosition;
    uint numPointLights;
    PointLight pointLights[MAX_LIGHTS];
} uniformData;

void main(){
    // in the beginning there is ambient only
    vec3 totalLighting = uniformData.ambient.xyz * uniformData.ambient.w;

    // go through all lights
    for(uint i = 0; i < uniformData.numPointLights; i++){
        // calculate direction to light from vertex
        vec3 directionToLight = uniformData.pointLights[i].position.xyz - inFragPosWorld;

        // calculate direction from vertex to viewer (camera)
        vec3 viewDir = normalize(uniformData.viewPosition - inFragPosWorld);

        // and reflect with given normal
        vec3 reflectDir = normalize(reflect(-directionToLight, inFragNormalWorld));

        // for point source we need to take into account the attenuation factor
        // attenuation is decaying of light intensity as distance from lightsource increases
        // float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
        float attenuation = 1.0 / sqrt(dot(directionToLight, directionToLight)); // distance

        float specular = 0.5;

        // calculate light color after adjusting for intensity
        vec3 lightColor = uniformData.pointLights[i].color.xyz * uniformData.pointLights[i].color.w * attenuation;
        float dotOfNormalAndLightDir = dot(normalize(inFragNormalWorld), normalize(directionToLight));
        vec3 diffuseLight = lightColor * max(dotOfNormalAndLightDir, 0);

        // specular lighting only if light is on right side
        vec3 specularLight = vec3(0);
        if(dotOfNormalAndLightDir >= 0.0){
            specularLight = specular * pow(max(dot(viewDir, reflectDir), 0), 32) * lightColor;
        }

        // effective specular and diffuse of all lights
        totalLighting += diffuseLight + specularLight;
    }

    outFragColor = vec4(totalLighting * inFragColor, 1.f);
}
