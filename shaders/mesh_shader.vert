#version 450

// get vertex data
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;

// give out fragment color
layout (location = 0) out vec3 outColor;

// get camera data
layout(set = 0, binding = 0) uniform UniformData {
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 lightPosition;
    float padding1;
    vec4 lightColor;
    vec3 viewPosition;
    float specularStrength;
} uniformData;

// push constants
layout( push_constant ) uniform constants {
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushData;

void main(){
    // calculate position of vertex in world space
    vec4 vPositionWorldSpace = pushData.modelMatrix * vec4(vPosition, 1.f);

    // calculate normal in world space
    vec3 vNormalWorldSpace = normalize(mat3(pushData.modelMatrix) * vNormal);

    // calculate direction to light from vertex
    vec3 directionToLight = uniformData.lightPosition - vPositionWorldSpace.xyz;

    // calculate direction from vertex to viewer (camera)
    vec3 viewDir = normalize(uniformData.viewPosition - vPositionWorldSpace.xyz);
    // and reflect with given normal
    vec3 reflectDir = normalize(reflect(-directionToLight, vNormalWorldSpace));

    // for point source we need to take into account the attenuation factor
    // attenuation is decaying of light intensity as distance from lightsource increases
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared

    // calculate light color after adjusting for intensity
    vec3 lightColor = uniformData.lightColor.xyz * uniformData.lightColor.w * attenuation;
    vec3 ambientLight = uniformData.ambientLightColor.xyz * uniformData.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(vNormalWorldSpace, normalize(directionToLight)), 0);
    vec3 specularLight = uniformData.specularStrength * pow(max(dot(viewDir, reflectDir), 0), 32) * lightColor;

    // calculate position in eye space
    gl_Position = uniformData.projectionViewMatrix * vPositionWorldSpace;

    outColor = (diffuseLight + ambientLight + specularLight) * vColor;
}
