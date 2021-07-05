#version 330 core
in vec2 TexCoords;
flat in float TexIndex;
out vec4 color;

uniform sampler2D images[8];

void main()
{
    // 如果 spriteColor 是 [1,1,1]，则与纹理像素相乘得到的还是纹理像素；举个例子，球的纹理是一个笑脸，而球的颜色是 [1,1,1]，相乘得到的还是纹理像素
    // 如果 spriteColor 是其他值，得到值就是与纹理像素相乘的结果，一般适用于灰度图与颜色相乘，灰度图与颜色相乘，得到是与颜色相近的值
    
    int index = int(TexIndex + 0.1);
    color = texture(images[index], TexCoords);
}