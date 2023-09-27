#version 450

// input
layout (location = 0) in vec2 a_position;

// output
layout (location = 0) out vec2 v_texcoord0;
   
void main()
{
    v_texcoord0.x = (a_position.x + 1.0) * 0.5;
    v_texcoord0.y = 1.0 - (a_position.y + 1.0) * 0.5;
    gl_Position = vec4(a_position, 1.0, 1.0);
}