#pragma once
#include <string_view>

inline std::string_view fog_vs = R"(#version 330
// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;

// NOTE: Add your custom variables here

void main() {
    // Send vertex attributes to fragment shader
    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
})";

inline std::string_view fog_fs = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

uniform sampler2D texture0;
uniform vec3 viewPos;

out vec4 finalColor;

void main() {
    float dist = length(viewPos - fragPosition);
    const vec4 fogColor = vec4(14.0/255.0, 11.0/255.0, 10.0/255.0, 1.0);
    float fogFactor = exp(-0.2*dist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    finalColor = fogFactor * finalColor + (1.0 - fogFactor) * fogColor;
})";