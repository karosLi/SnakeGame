//
//  snake_object.hpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/14.
//

#ifndef SNAKE_OBJECT_H
#define SNAKE_OBJECT_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "game_object.h"
#include "texture.h"

class SnakeObject {
    
public:
    // 蛇的所有节点
    std::vector<GameObject> Nodes;
    
    // 位置，大小，长度，速度(向量，包含了方向和大小)
    GLfloat     InitialLength, SpriteRotation;
    glm::vec2   Position, NodeSize, Velocity;
    
    // 外观
    glm::vec4   Color;// 颜色
    Texture2D   *Sprites;// 头部，中间，尾巴纹理，数组（每个元素存的是一个 Texture2D 类型指针）指针
    GLuint      SpriteCount;//纹理个数
    
    GLboolean   Pause;// 蛇停止移动
    
    // 构造函数
    SnakeObject(glm::vec2 position, glm::vec2 nodeSize, GLfloat initialLength, Texture2D *sprites, GLuint spriteCount, GLfloat spriteRotation, glm::vec2 velocity, glm::vec4 color = glm::vec4(1.0f));
    
    // 更新
    void Move(GLfloat dt);
    void Reset(glm::vec2 position, glm::vec2 velocity);
    
    // 渲染
    void Draw(SpriteRenderer &renderer);
    
private:
    GLfloat     NodeDistance;// 节点间的距离
    void LoadNodes();
    void MoveHead();
    void MoveBody1(GLfloat dt);
    void MoveBody2(GLfloat dt);
};


#endif /* snake_object_h */
