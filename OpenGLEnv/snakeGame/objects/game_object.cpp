//
//  game_object.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/4/27.
//

#include "game_object.h"

GameObject::GameObject()
    : FrameIndex(0), Position(0, 0), StaticPosition(0, 0), Size(1, 1), Velocity(0.0f), Color(1.0f), Rotation(0.0f), RotationQuat(glm::mat4(1.0f)), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color, glm::vec2 velocity)
    : FrameIndex(0), Position(pos), StaticPosition(0, 0), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), RotationQuat(glm::mat4(1.0f)), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Color, this->Rotation, this->RotationQuat);
}
