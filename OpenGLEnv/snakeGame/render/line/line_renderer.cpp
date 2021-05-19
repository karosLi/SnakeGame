//
//  line_renderer.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/13.
//

#include "line_renderer.h"

LineRenderer::LineRenderer(Shader &shader)
{
    this->shader = shader;
    this->initRenderData();
}

LineRenderer::~LineRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void LineRenderer::DrawLine(glm::vec2 position, glm::float_t length, GLboolean horizontal, glm::float_t rotate, glm::vec4 color)
{
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(horizontal ? 0.5f * length : 0.0f, !horizontal ? 0.5f * length : 0.0f, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(horizontal ? -0.5f * length : 0.0f, !horizontal ? -0.5f * length : 0.0f, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(horizontal ? length : 1.0f, !horizontal ? length : 1.0f, 1.0f)); // last scale

    this->shader.SetMatrix4("model", model);

    // render textured quad
    this->shader.SetVector4f("spriteColor", color);

    glBindVertexArray(this->quadVAO);
    glLineWidth(0.2f);
    glEnable(GL_LINE_SMOOTH);
    
    if (horizontal) {
        glDrawArrays(GL_LINES, 0, 2);
    } else {
        glDrawArrays(GL_LINES, 2, 2);
    }
    
    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
    glBindVertexArray(0);
}

void LineRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos
        // 位置
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };
    
    /**
     因为正交投影矩阵的影响，[0,0]在屏幕左上角
     2个顶点都是在 x, y 的正方向上

               |
               |
     ----------------------------------> x
               |----------- |
               |
               |
               y
     */
    
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
