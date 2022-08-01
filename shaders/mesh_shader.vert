#version 450

// get vertex data
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;

// give out fragment color
layout (location = 0) out vec3 outColor;

// get camera data
layout(set = 0, binding = 0) uniform CameraData{
    mat4 view;
    mat4 projection;
} cameraData;

// push constants
layout( push_constant ) uniform constants {
    mat4 objectModelMatrix;
} pushConstants;

void main(){
    // calculate transformed position
    gl_Position = cameraData.projection *
                  cameraData.view *
                  pushConstants.objectModelMatrix *
                  vec4(vPosition, 1.0f);

    vec3 lightColor = vec3(0.25, 0.5, 1);
    vec3 lightPos = vec3(3, 4, 5);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 Normal = mat3(pushConstants.objectModelMatrix) * vNormal;
    vec3 fragPos = vec3(pushConstants.objectModelMatrix * vec4(vPosition, 1.f));
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(vNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * vColor;
    outColor = result;
}
