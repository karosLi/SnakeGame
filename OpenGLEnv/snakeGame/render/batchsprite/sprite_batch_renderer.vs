#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
/**
 实例化数组，每绘制一次实例，就更新一次属性值
 */
layout (location = 2) in mat4 aInstanceMatrix;
layout (location = 6) in float aTexIndex;

out vec2 TexCoords;
flat out float TexIndex;

uniform mat4 projection;

void main()
{
    TexCoords = aTexCoord;
    TexIndex = aTexIndex;
    gl_Position = projection * aInstanceMatrix * vec4(aPos, 1.0);
}

