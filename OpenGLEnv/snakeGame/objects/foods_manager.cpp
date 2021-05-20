//
//  foods_manager.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/20.
//

#include "foods_manager.h"
#include "resource_manager.h"

FoodsManager::FoodsManager(glm::vec2 mapOrigin, glm::vec2 mapSize, Texture2D *sprites, GLuint spriteCount, glm::vec4 *colors, GLuint colorCount): MapOrigin(mapOrigin), MapSize(mapSize), Sprites(sprites), SpriteCount(spriteCount), Colors(colors), ColorCount(colorCount)
{
    
}

void FoodsManager::GenerateSpriteFoods(GLuint foodCount, glm::vec2 foodSize)
{
    for (GLuint i = 0; i < foodCount; i++) {
        glm::vec2 pos = this->GenearteRandomPosition(foodSize);
        Texture2D sprite = this->GenearteRandomSprite();
        GameObject food(pos, foodSize, sprite, glm::vec4(1.0f));
        
        this->Foods.push_back(food);
    }
}

void FoodsManager::GenerateColorFoods(GLuint foodCount, glm::vec2 foodSize)
{
    for (GLuint i = 0; i < foodCount; i++) {
        glm::vec2 pos = this->GenearteRandomPosition(foodSize);
        glm::vec4 color = this->GenearteRandomColor();
        GameObject food(pos, foodSize, ResourceManager::GetEmptyTexture(), color);
        
        this->Foods.push_back(food);
    }
}

void FoodsManager::Update(GLfloat dt)
{
    std::vector<GameObject> destoryedFoods;
    
    for (GameObject &food : this->Foods) {
        if (food.Destroyed) {
            destoryedFoods.push_back(food);
        }
    }
    
//    for (GameObject &food : destoryedFoods) {
//        if (food.Destroyed) {
//            destoryedFoods.push_back(food);
//            if (food.Sprite.EmptyTexture) {
//                this->GenerateColorFoods(1, food.Size);
//            } else {
//                this->GenerateSpriteFoods(1, food.Size);
//            }
//        }
//    }
}

void FoodsManager::Draw(SpriteRenderer &renderer)
{
    for (GameObject &food : this->Foods) {
        if (!food.Destroyed) {
            food.Draw(renderer);
        }
    }
}

glm::vec2 FoodsManager::GenearteRandomPosition(glm::vec2 foodSize)
{
    GLfloat x = this->MapOrigin.x + (rand() % ((GLuint)(this->MapSize.x - foodSize.x)));
    GLfloat y = this->MapOrigin.y + (rand() % ((GLuint)(this->MapSize.y - foodSize.y)));
    
    return glm::vec2(x, y);
}

Texture2D FoodsManager::GenearteRandomSprite()
{
    if (this->SpriteCount == 0) {
        return ResourceManager::GetEmptyTexture();
    }
    
    GLuint index = (rand() % this->SpriteCount);
    return this->Sprites[index];
}

glm::vec4 FoodsManager::GenearteRandomColor()
{
    if (this->ColorCount == 0) {
        return glm::vec4(0.0f);
    }
    
    GLuint index = (rand() % this->ColorCount);
    return this->Colors[index];
}
