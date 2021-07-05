//
//  sprite_batch_renderer.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/7/1.
//

#include "sprite_batch_renderer.h"

#define MaxTextureNum 8

struct InstanceData {
    // 矩阵
    glm::mat4 Matrix;
    // 纹理索引
    GLint TextureIndex;
};

SpriteBatchRenderer::SpriteBatchRenderer(Shader &shader)
{
    this->shader = shader;
    this->initRenderData();
}

SpriteBatchRenderer::~SpriteBatchRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteBatchRenderer::DrawSprites(std::vector<GameObject> &sprites)
{
    this->shader.Use();
    
    GLuint count = static_cast<GLuint>(sprites.size());
    InstanceData* instanceDatas = new InstanceData[count];
    
    GLuint textureIndexes[MaxTextureNum] = {0};
    GLuint textureInfoCount = 0;
    
    for (GLint i = 0; i < count; i++) {
        GameObject gameObject = sprites[i];
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec2 position = gameObject.Position;
        glm::vec2 size = gameObject.Size;
        float rotate = gameObject.Rotation;
        glm::quat rotationQuat = gameObject.RotationQuat;
        Texture2D &texture = gameObject.Sprite;
        
        model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
        // 正的度数是顺时针
        if (rotate > 0) {
            model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
        } else {
            model = model * glm::mat4_cast(rotationQuat);
        }
        
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

        model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale
        
        instanceDatas[i].Matrix = model;
        
        GLint textureIndex = 0;
        GLboolean foundSame = GL_FALSE;
        for (int ii = 0; ii < textureInfoCount; ++ii) {
            if (textureIndexes[ii] == texture.ID) {
                foundSame = GL_TRUE;
                textureIndex = ii;
                break;
            }
        }
        if (foundSame == GL_FALSE) {
            if (textureInfoCount < MaxTextureNum) {
                textureIndexes[textureInfoCount] = texture.ID;
                textureIndex = textureInfoCount;
                textureInfoCount++;
            } else {
                
            }
        }
        
        instanceDatas[i].TextureIndex = textureIndex;
    }
    
    // 发送矩阵数据到GPU
    glBindBuffer(GL_ARRAY_BUFFER, matrixVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(InstanceData), &instanceDatas[0], GL_DYNAMIC_DRAW);
    
    GLuint textureUnit = 0;
    for (GLuint ii = 0; ii < textureInfoCount; ++ii) {
        GLuint textureName = textureIndexes[ii];
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureName);
        textureUnit++;
    }
    
    glBindVertexArray(this->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count);
    glBindVertexArray(0);
}

void SpriteBatchRenderer::initRenderData()
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
    
    glGenBuffers(1, &matrixVBO);
    glBindBuffer(GL_ARRAY_BUFFER, matrixVBO);// 绑定矩阵 VBO，让下面的顶点属性从 matrixVBO 里取数据
    
    // 矩阵属性
    GLsizei size = sizeof(InstanceData);
    GLsizei vec4Size = sizeof(glm::vec4);
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, size, (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, size, (void*)(vec4Size));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, size, (void*)(3 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 1, GL_INT, GL_FALSE, size, (void*)(offsetof(InstanceData, TextureIndex)));
    
    // 设置顶点属性更新方式，0 表示每个顶点更新，1 表示每个实例更新，2 每隔 2 个实例更新，以此类推
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    
    glBindVertexArray(0);
    
    this->shader.Use();
    const GLint samplerIDs[MaxTextureNum] = {0, 1, 2, 3, 4, 5, 6, 7};
    this->shader.SetIntegers("images", MaxTextureNum, (const GLint *)samplerIDs);
}
