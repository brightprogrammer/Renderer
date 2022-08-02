#version 450

// get vertex data
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;

// give out fragment color
layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragPosWorld;
layout (location = 2) out vec3 fragNormalWorld;

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

// push constants
layout( push_constant ) uniform constants {
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushData;

void main(){
    // calculate position of vertex in world space
    vec4 vPositionWorldSpace = pushData.modelMatrix * vec4(vPosition, 1.f);

    // calculate normal in world space
    fragNormalWorld = normalize(mat3(pushData.modelMatrix) * vNormal);
    fragPosWorld = vPositionWorldSpace.xyz;
    fragColor = vColor;

    // calculate position in eye space
    gl_Position = uniformData.projectionViewMatrix * vPositionWorldSpace;

    fragColor = vColor;
}
