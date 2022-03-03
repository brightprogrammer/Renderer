#version 450

// get vertex data
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;

// give out fragment color
layout (location = 0) out vec3 outColor;

// get camera data
layout(set = 0, binding = 0) uniform CameraData{
    mat4 model;
	mat4 view;
	mat4 projection;
} cameraData;

void main(){
    // calculate transformed position
	gl_Position =   cameraData.projection *
                    cameraData.view *
                    cameraData.model *
                    vec4(vPosition, 1.0f);

	outColor = vColor;
}
