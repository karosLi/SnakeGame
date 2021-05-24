//
//  camera_2d.hpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/24.
//

#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera2D {
private:
    GLuint       Width, Height;// 窗口宽高
    GLboolean    OriginOnLeftTop;// 是否是原点在左上角
    glm::vec2    FocusPosition;// 跟随的点
    GLfloat      Zoom = 1.0f;// 缩放大小
    
public:
    Camera2D(GLuint width, GLuint height, GLboolean originOnLeftTop = GL_TRUE);
    
    void UpdateFocusPosition(glm::vec2 position);
    void UpdateZoom(GLfloat zoom);
    glm::mat4 GetProjectionMatrix();
};

#endif /* camera_2d_h */
