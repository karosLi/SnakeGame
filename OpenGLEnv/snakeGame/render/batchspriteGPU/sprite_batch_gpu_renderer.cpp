//
//  sprite_batch_gpu_renderer.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/7/5.
//

#include "sprite_batch_gpu_renderer.h"

#define MaxTextureNum 8

struct InstanceData {
    // 实例位置
    glm::vec2 Position;
    // 实例大小
    glm::vec2 Size;
    // 实例旋转弧度
    GLfloat Radian;
    // 实例旋转四元数
    glm::quat Quaternion;
    // 纹理索引
    GLint TextureIndex;
    // 纹理 frame, 纹理左下角和纹理宽高，都是小于 1.0
    glm::vec4 TextureFrame;
};

SpriteBatchGPURenderer::SpriteBatchGPURenderer(Shader &shader)
{
    this->shader = shader;
    this->initRenderData();
}

SpriteBatchGPURenderer::~SpriteBatchGPURenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteBatchGPURenderer::DrawSprites(std::vector<GameObject> &sprites)
{
    this->shader.Use();
    
    GLuint count = static_cast<GLuint>(sprites.size());
    // 矩阵数据
    InstanceData* instanceDatas = new InstanceData[count];
    
    // 纹理坐标
//    glm::vec2* textureCoords = new glm::vec2[count * 6];
    
    GLuint textureIndexes[MaxTextureNum] = {0};
    GLuint textureInfoCount = 0;
    
    for (GLint i = 0; i < count; i++) {
        GameObject gameObject = sprites[i];
        
        glm::vec2 position = gameObject.Position;
        glm::vec2 size = gameObject.Size;
        float rotate = gameObject.Rotation;
        glm::quat rotationQuat = gameObject.RotationQuat;
        Texture2D &texture = gameObject.Sprite;
 
        instanceDatas[i].Position = position;
        instanceDatas[i].Size = size;
        instanceDatas[i].Radian = glm::radians(rotate);
        instanceDatas[i].Quaternion = rotationQuat;
        
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
        
        // 设置纹理索引和纹理坐标
        instanceDatas[i].TextureIndex = textureIndex;
//        textureCoords[i + 0] = glm::vec2(0.0, 1.0);
//        textureCoords[i + 1] = glm::vec2(1.0, 0.0);
//        textureCoords[i + 2] = glm::vec2(0.0, 0.0);
//        textureCoords[i + 3] = glm::vec2(0.0, 1.0);
//        textureCoords[i + 4] = glm::vec2(1.0, 1.0);
//        textureCoords[i + 5] = glm::vec2(1.0, 0.0);
//
//        instanceDatas[i].TextureCoords[i + 0] = glm::vec2(0.0, 1.0);
//        instanceDatas[i].TextureCoords[i + 1] = glm::vec2(1.0, 0.0);
//        instanceDatas[i].TextureCoords[i + 2] = glm::vec2(0.0, 0.0);
//        instanceDatas[i].TextureCoords[i + 3] = glm::vec2(0.0, 1.0);
//        instanceDatas[i].TextureCoords[i + 4] = glm::vec2(1.0, 1.0);
//        instanceDatas[i].TextureCoords[i + 5] = glm::vec2(1.0, 0.0);
        
        instanceDatas[i].TextureFrame = glm::vec4(0.0, 0.0, 1.0, 1.0);
    }
    
    // 局部更新纹理坐标：发送纹理坐标数据到GPU
//    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//    glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), count * 6 * sizeof(GLfloat), &textureCoords[0]);
    
    // 更新模型视图矩阵： 发送矩阵数据到GPU
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

void SpriteBatchGPURenderer::initRenderData()
{
    // 初始化单位正方形顶点位置和纹理坐标
    GLfloat vertices[] = {
        // pos             // tex
        // 位置            // 纹理坐标
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 左下角
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 右上角
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 左上角

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // 右下角
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
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &matrixVBO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /// 配置顶点属性取值描述
    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    /// 配置矩阵属性取值描述
    glBindBuffer(GL_ARRAY_BUFFER, matrixVBO);// 绑定矩阵 VBO，让下面的顶点属性从 matrixVBO 里取数据
    // 矩阵属性
    GLsizei size = sizeof(InstanceData);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, size, (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(InstanceData, Size)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(InstanceData, Radian)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(InstanceData, Quaternion)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 1, GL_INT, GL_FALSE, size, (void*)(offsetof(InstanceData, TextureIndex)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(InstanceData, TextureFrame)));
    
    // 设置顶点属性更新方式，0 表示每个顶点更新，1 表示每个实例更新，2 每隔 2 个实例更新，以此类推
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    
    this->shader.Use();
    const GLint samplerIDs[MaxTextureNum] = {0, 1, 2, 3, 4, 5, 6, 7};
    this->shader.SetIntegers("images", MaxTextureNum, (const GLint *)samplerIDs);
}
