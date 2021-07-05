//
//  sprite_batch_renderer.hpp
//  OpenGLEnv
//
//  Created by karos li on 2021/7/1.
//

#ifndef SPRITE_BATCH_RENDERER_H
#define SPRITE_BATCH_RENDERER_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "texture.h"
#include "shader.h"
#include "game_object.h"

// 批量精灵render
class SpriteBatchRenderer
{
public:
    // Constructor (inits shaders/shapes)
    SpriteBatchRenderer(Shader &shader);
    // Destructor
    ~SpriteBatchRenderer();
    // 绘制一批精灵，用同一个纹理
    void DrawSprites(std::vector<GameObject> &sprites);
private:
    // Render state
    Shader       shader;
    unsigned int quadVAO;
    unsigned int matrixVBO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif /* sprite_batch_renderer_hpp */
