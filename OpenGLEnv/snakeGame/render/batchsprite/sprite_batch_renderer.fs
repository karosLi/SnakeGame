#version 330 core
in vec2 TexCoords;
flat in int TexIndex;
out vec4 color;

uniform sampler2D images[8];

void main()
{
    color = texture(images[TexIndex], TexCoords);
}
