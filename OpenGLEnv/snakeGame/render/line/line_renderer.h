//
//  line_renderer.hpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/13.
//

#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

// 线段render
class LineRenderer
{
public:
    // Constructor (inits shaders/shapes)
    LineRenderer(Shader &shader);
    // Destructor
    ~LineRenderer();
    // Renders a defined quad textured with given sprite
    void DrawLine(glm::vec2 position, glm::float_t length, GLboolean horizontal = GL_TRUE, glm::float_t rotate = 0.0f, glm::vec4 color = glm::vec4(1.0f));
private:
    // Render state
    Shader       shader;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};


#endif /* line_renderer_hpp */
