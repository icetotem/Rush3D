#version 460 core

// input
layout (location = 0) in vec2 a_position;

// output
layout (location = 0) out vec2 v_TexCoord;
   
void main()
{
    v_TexCoord.x = (a_position.x + 1.0) * 0.5;
    v_TexCoord.y = 1.0 - (a_position.y + 1.0) * 0.5;
    gl_Position = vec4(a_position, 1.0, 1.0);
}