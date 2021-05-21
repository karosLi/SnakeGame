//
//  foods_manager.hpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/20.
//

#ifndef FOODS_MANAGER
#define FOODS_MANAGER

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "game_object.h"
#include "texture.h"

class FoodsManager {
    
public:
    std::vector<GameObject> Foods;// 所有食物
    glm::vec2   MapOrigin, MapSize;// 地图原点和大小
    
    /// 食物有纹理和彩点两种
    std::vector<Texture2D> Sprites;// 纹理数组
    std::vector<glm::vec4> Colors;// 颜色数组
    
    FoodsManager(glm::vec2 mapOrigin, glm::vec2 mapSize, std::vector<Texture2D> sprites, std::vector<glm::vec4> colors = {});
    
    // 生成一批纹理食物
    void GenerateSpriteFoods(GLuint foodCount, glm::vec2 foodSize);
    // 生成一批颜色食物
    void GenerateColorFoods(GLuint foodCount, glm::vec2 foodSize);
    
    // 更新食物状态
    void Update(GLfloat dt);
    
    // 渲染
    void Draw(SpriteRenderer &renderer);
    
private:
    glm::vec2 GenearteRandomPosition(glm::vec2 foodSize);
    Texture2D GenearteRandomSprite();
    glm::vec4 GenearteRandomColor();
};

#endif /* foods_manager_hpp */
