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
    GLuint      frameIndex;// 对象当前渲染的帧索引
    glm::vec3   Position;
    glm::vec2   Size, Velocity;
    glm::vec4   Color;
    GLfloat     Rotation;
    glm::quat   RotationQuat;
    GLboolean   IsSolid;// 不可被摧毁
    GLboolean   Destroyed;
    // Render state
    Texture2D   Sprite;
    // Constructor(s)
    GameObject();
    GameObject(glm::vec3 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color = glm::vec4(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // Draw sprite
    virtual void Draw(SpriteRenderer &renderer);
};

#endif
