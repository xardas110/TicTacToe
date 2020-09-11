#version 460 core
layout (location = 0) in vec3 pos;   
layout (location = 1) in vec3 normal;

uniform mat4 MVP;
out vec3 norm;
void main()
{
    norm = normal;
    gl_Position = MVP * vec4(pos.x, pos.y, pos.z,1.0);
}  