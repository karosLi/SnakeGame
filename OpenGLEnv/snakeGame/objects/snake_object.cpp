//
//  snake_object.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/14.
//

#include "snake_object.h"
#include "resource_manager.h"

// 构造函数
SnakeObject::SnakeObject(glm::vec2 position, glm::vec2 nodeSize, GLfloat initialLength, Texture2D *sprites, GLuint spriteCount, GLfloat spriteRotation, glm::vec2 velocity, glm::vec4 color): Position(position), NodeSize(nodeSize), InitialLength(initialLength), Sprites(sprites), SpriteCount(spriteCount), SpriteRotation(spriteRotation), Velocity(velocity), Color(color), Pause(GL_TRUE) {
    this->NodeDistance = this->NodeSize.x * 1.0f;
    this->LoadNodes();
}

void SnakeObject::LoadNodes() {
    // 清空过期数据
    this->Nodes.clear();
    
    Texture2D headSprite = this->Sprites[0];
    Texture2D bodySprite = this->Sprites[1];
    for (GLuint i = 0; i < this->InitialLength; i++) {
        glm::vec3 pos(this->Position.x, this->Position.y + i * this->NodeDistance, 0);
        
        if (i == 0) {
            GameObject node(pos, this->NodeSize, headSprite, glm::vec4(1.0f));
            this->Nodes.push_back(node);
            this->Position = pos;
            this->MoveHead();
        } else {
            GameObject node(pos, this->NodeSize, bodySprite, glm::vec4(1.0f));
            node.Velocity = this->Nodes[0].Velocity;
            node.Rotation = this->Nodes[0].Rotation;
            node.RotationQuat = this->Nodes[0].RotationQuat;
            this->Nodes.push_back(node);
        }
    }
}

void SnakeObject::MoveHead() {
    // 单位化速度向量，可以获取到蛇的方向
    glm::vec2 direction = glm::normalize(this->Velocity);
    
    // 获取蛇方向与x正轴的夹角(弧度)
    GLfloat radians = glm::atan(direction.y, direction.x);
    GLfloat rotaion = glm::degrees(radians);
    // 为了让蛇纹理方向与蛇移动方向一致，需要旋转蛇的节点
    rotaion += this->SpriteRotation;
    
    /// 移动蛇头
    this->Nodes[0].Position = glm::vec3(this->Position, this->Nodes[0].Position.z);
    this->Nodes[0].Velocity = direction;
    this->Nodes[0].Rotation = rotaion;
    this->Nodes[0].RotationQuat = glm::angleAxis(glm::radians(rotaion), glm::vec3(0.0f, 0.0f, 1.0f));
}

void SnakeObject::Move(GLfloat dt) {
    if (this->Pause) {
        return;
    }
    
    this->Position += this->Velocity * dt;
    this->MoveHead();
    
    /**
     旋转参考
     http://www.opengl-tutorial.org/cn/intermediate-tutorials/tutorial-17-quaternions/
     https://docs.unity3d.com/ScriptReference/Quaternion.Slerp.html
     */
    /// 移动其他节点
    GLuint size = static_cast<GLuint>(this->Nodes.size());
    for (GLint i = 1; i < size; i++) {
        GameObject &preNode = this->Nodes[i-1];
        GameObject &curNode = this->Nodes[i];
 
        GLfloat distance = glm::distance(preNode.Position,  curNode.Position);
        GLfloat interpFactor = this->NodeDistance / distance;
        
        if (interpFactor >= 1.0f) {
            continue;
        }
        
        /// 线性插值位置
        GLfloat z = curNode.Position.z;
        curNode.Position = glm::mix(preNode.Position, curNode.Position, interpFactor);
        // 不修改 z
        curNode.Position.z = z;
        
        /// 四元数 - 旋转插值
        glm::quat interpRotationQuat = glm::slerp(preNode.RotationQuat, curNode.RotationQuat, interpFactor);
        curNode.RotationQuat = interpRotationQuat;
        
        
        /**
        // 方法二 - 球面插值旋转方向，但是在边界情况触发时，会发生转向不平滑的问题
        glm::vec3 slerpDir;
        glm::vec3 preVelocity = glm::vec3(preNode.Velocity, 0.0f);
        glm::vec3 curVelocity = glm::vec3(curNode.Velocity, 0.0f);

        slerpDir = glm::slerp(preVelocity, curVelocity, interpFactor);
        if (glm::isnan(slerpDir.x)) {// 处理边界情况
            // 退化到线性插值
            slerpDir = glm::mix(preVelocity, curVelocity, interpFactor);
        }
        curNode.Velocity = slerpDir;

        GLfloat radians = glm::atan(slerpDir.y, slerpDir.x);

        GLfloat rotation = glm::degrees(radians);
        rotation += this->SpriteRotation;

        curNode.Rotation = rotation;
        **/
        
        /**
         // 方法三 - 四元数 - 旋转插值，但是目前尝试没有得到预期的结果，也有可能是我的姿势不对
         // 用四元数计算旋转插值，求出旋转四元数
         glm::quat preDirQuat(glm::vec3(preNode.Velocity, 0.0f));
         glm::quat curDirQuat(glm::vec3(curNode.Velocity, 0.0f));
         glm::quat interpDirQuat = glm::mix(preDirQuat, curDirQuat, interpFactor);

         // 通过四元数旋转变换把上一个方向变换成期望的插值方向
         glm::vec3 slerpDir = interpDirQuat * glm::vec3(preNode.Velocity, 0.0f);
         curNode.Velocity = slerpDir;

         GLfloat radians = glm::atan(slerpDir.y, slerpDir.x);
         GLfloat rotation = glm::degrees(radians);
         rotation += this->SpriteRotation;

         curNode.Rotation = rotation;
         */
    }
}

void SnakeObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    
}

void SnakeObject::Draw(SpriteRenderer &renderer) {
    GLuint size = static_cast<GLuint>(this->Nodes.size());
    for (GLint i = size - 1; i >= 0; i--) {
        this->Nodes[i].Draw(renderer);
    }
}
