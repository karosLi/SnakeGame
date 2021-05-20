//
//  foods_manager.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/20.
//

#include "foods_manager.h"
#include "resource_manager.h"

FoodsManager::FoodsManager(glm::vec2 mapOrigin, glm::vec2 mapSize, std::vector<Texture2D> sprites, std::vector<glm::vec4> colors): MapOrigin(mapOrigin), MapSize(mapSize), Sprites(sprites), Colors(colors)
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
    // 移除被吃掉的食物
    std::vector<GameObject> destoryedFoods;
    this->Foods.erase(std::remove_if(this->Foods.begin(), this->Foods.end(), [&destoryedFoods](GameObject &food) {
        if (food.Destroyed) {
            destoryedFoods.push_back(food);
        }
        return food.Destroyed;
    }), this->Foods.end());

    // 有多少食物被吃掉，就生成多少新食物
    for (GameObject &food : destoryedFoods) {
        if (food.Destroyed) {
            if (food.Sprite.EmptyTexture) {
                this->GenerateColorFoods(1, food.Size);
            } else {
                this->GenerateSpriteFoods(1, food.Size);
            }
        }
    }
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
    GLuint size = static_cast<GLuint>(this->Sprites.size());
    if (size == 0) {
        return ResourceManager::GetEmptyTexture();
    }
    
    GLuint index = rand() % size;
    return this->Sprites[index];
}

glm::vec4 FoodsManager::GenearteRandomColor()
{
    GLuint size = static_cast<GLuint>(this->Colors.size());
    if (size == 0) {
        return glm::vec4(0.0f);
    }
    
    GLuint index = rand() % size;
    return this->Colors[index];
}
