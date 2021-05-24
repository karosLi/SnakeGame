//
//  camera_2d.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/24.
//

#include "camera_2d.h"

Camera2D::Camera2D(GLuint width, GLuint height, GLboolean originOnLeftTop): Width(width), Height(height), OriginOnLeftTop(originOnLeftTop)
{
    
}

void Camera2D::UpdateFocusPosition(glm::vec2 position)
{
    this->FocusPosition = position;
}

void Camera2D::UpdateZoom(GLfloat zoom)
{
    this->Zoom = zoom;
}

glm::mat4 Camera2D::GetProjectionMatrix()
{
    glm::vec2 focusPostion = this->FocusPosition;
    
    GLfloat left = focusPostion.x - static_cast<float>(this->Width / 2.0);
    GLfloat right = focusPostion.x + static_cast<float>(this->Width / 2.0);
    GLfloat top = focusPostion.y - static_cast<float>(this->Height / 2.0);
    GLfloat bottom = focusPostion.y + static_cast<float>(this->Height / 2.0);
    
    glm::mat4 projection;
    if (this->OriginOnLeftTop) {
        /// 设置投影矩阵 - 左上角为原点
        projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    } else {
        projection = glm::ortho(left, right, top, bottom, -1.0f, 1.0f);
    }
    
    glm::mat4 scaleMatrix = glm::scale(glm::vec3(this->Zoom, this->Zoom, 1.0f));
    return scaleMatrix * projection;
}
