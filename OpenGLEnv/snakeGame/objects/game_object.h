//
//  game_object.hpp
//  OpenGLEnv
//
//  Created by karos li on 2021/4/27.
//

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "texture.h"
#include "sprite_renderer.h"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // Object state
    GLint       FrameIndex;// 对象当前渲染的帧索引
    glm::vec2   Position, Size, Velocity;// 位置，大小，每秒速度
    glm::vec4   Color;// 颜色
    GLfloat     Rotation;// 旋转角度，正数为顺时针，负数为逆时针
    glm::quat   RotationQuat;// 旋转四元数
    GLboolean   IsSolid;// 不可被摧毁
    GLboolean   Destroyed;// 是否被销毁
    // Render state
    Texture2D   Sprite;// 纹理精灵
    // Constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color = glm::vec4(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // Draw sprite
    virtual void Draw(SpriteRenderer &renderer);
};

#endif
