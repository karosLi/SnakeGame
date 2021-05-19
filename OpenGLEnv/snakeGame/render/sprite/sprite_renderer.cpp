//
//  sprite_renderer.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/4/27.
//

#include "sprite_renderer.h"

SpriteRenderer::SpriteRenderer(Shader &shader)
{
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(Texture2D &texture, glm::vec3 position, glm::vec2 size, glm::vec4 color, float rotate, glm::quat rotationQuat)
{
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, position.z));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
    model = glm::translate(model, position);

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    // 正的度数是顺时针
    if (rotate > 0) {
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    } else {
        model = model * glm::mat4_cast(rotationQuat);
    }
    
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->shader.SetMatrix4("model", model);

    // render textured quad
    this->shader.SetVector4f("spriteColor", color);
    
    if (texture.EmptyTexture) {
        this->shader.SetInteger("useImage", false);
    } else {
        this->shader.SetInteger("useImage", true);
        
        glActiveTexture(GL_TEXTURE0);
        texture.Bind();
    }
    
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos             // tex
        // 位置            // 纹理坐标
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    
    /**
     因为正交投影矩阵的影响，[0,0]在屏幕左上角
     6个顶点都是在 x, y 的正方向上

               |
               |
     ----------------------------------> x
               |                |
               |                |
               |----------- |
               |
               y
     */
    
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
