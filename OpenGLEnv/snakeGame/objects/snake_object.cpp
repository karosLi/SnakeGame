//
//  snake_object.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/14.
//

#include "snake_object.h"
#include "resource_manager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 构造函数
SnakeObject::SnakeObject(glm::vec2 position, glm::vec2 nodeSize, GLfloat initialLength, std::vector<Texture2D> sprites, GLfloat spriteRotation, glm::vec2 velocity, glm::vec4 color): Position(position), NodeSize(nodeSize), InitialLength(initialLength), Sprites(sprites), SpriteRotation(spriteRotation), Velocity(velocity), Color(color), Pause(GL_TRUE), SpeedUp(GL_FALSE), Died(GL_FALSE) {
    this->NodeDistance = this->NodeSize.x * 1.0f;
    this->SnakeBornCount = initialLength;
    this->LoadNodes();
}

void SnakeObject::LoadNodes() {
    // 清空过期数据
    this->Nodes.clear();
    
    for (GLuint i = 0; i < this->SnakeBornCount; i++) {
        this->AddTailNode();
    }
}

void SnakeObject::AddTailNode()
{
    Texture2D headSprite = this->Sprites[0];
    Texture2D bodySprite = this->Sprites[1];
    
    GLuint size = static_cast<GLuint>(this->Nodes.size());
    if (size == 0) {
        glm::vec2 pos(this->Position.x, this->Position.y + this->NodeDistance);
        GameObject newNode(pos, this->NodeSize, headSprite, glm::vec4(1.0f));
        this->Nodes.push_back(newNode);
        this->Position = pos;
        this->MoveHead();
        return;
    }
    
    
    GameObject lastNode = this->Nodes[size - 1];// 最后一个节点
    GameObject secondLastNode = size == 1 ? lastNode : this->Nodes[size - 2];// 最后一个节点
    
    GLfloat moveDistance;
    if (size == 1) {// 说明只有一个头节点
        moveDistance = this->NodeDistance;
    } else {
        moveDistance = glm::distance(lastNode.Position, secondLastNode.Position);
    }
    
    glm::vec2 direction = lastNode.Velocity;// 最后一个节点的方向
    // 通过最后一个节点位置往相反方向移动节点大小的位置就可以获得新的尾部节点的位置
    glm::vec2 pos = lastNode.Position - direction * moveDistance;
    
    GameObject newNode(pos, this->NodeSize, bodySprite, glm::vec4(1.0f));
    newNode.Velocity = lastNode.Velocity;
    newNode.Rotation = lastNode.Rotation;
    newNode.RotationQuat = lastNode.RotationQuat;
    this->Nodes.push_back(newNode);
}

void SnakeObject::EatFood(glm::vec2 foodPosition)
{
    this->AddTailNode();
}

void SnakeObject::Die()
{
    this->Died = GL_TRUE;
}

void SnakeObject::Reborn()
{
    this->Died = GL_FALSE;
    GLuint size = static_cast<GLuint>(this->Nodes.size());
    this->SnakeBornCount = size;
    
    this->LoadNodes();
}

void SnakeObject::Restart()
{
    this->Died = GL_FALSE;
    this->SnakeBornCount = this->InitialLength;
    
    this->LoadNodes();
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
    this->Nodes[0].Position = this->Position;
    this->Nodes[0].Velocity = direction;
    this->Nodes[0].Rotation = rotaion;
    this->Nodes[0].RotationQuat = glm::angleAxis(glm::radians(rotaion), glm::vec3(0.0f, 0.0f, 1.0f));
}

void SnakeObject::Move(GLfloat dt) {
    if (this->Pause) {
        return;
    }
    
    this->MoveBody1(dt);
//    this->MoveBody2(dt);
}

void SnakeObject::MoveBody1(GLfloat dt) {
    GLfloat speedUp = this->SpeedUp ? 2.0f : 1.0f;
    
    GLfloat speed = glm::length(this->Velocity * speedUp);// 每秒速度
    GLuint size = static_cast<GLuint>(this->Nodes.size());
    for (GLint i = size - 1; i > 0; i--) {
        GameObject &preNode = this->Nodes[i-1];
        GameObject &curNode = this->Nodes[i];
        
        glm::vec2 direction = preNode.Position - curNode.Position;
        direction = glm::normalize(direction);
        curNode.Velocity = direction;
        
        glm::vec2 moveVector = direction * speed * dt;
        GLfloat moveDistance = glm::length(moveVector);
        
        GLfloat interpFactor = moveDistance / this->NodeDistance;
        /// 线性插值位置
        curNode.Position = glm::mix(curNode.Position, preNode.Position, interpFactor);
        /// 四元数 - 旋转插值
        glm::quat interpRotationQuat = glm::slerp(curNode.RotationQuat, preNode.RotationQuat, interpFactor);
        curNode.RotationQuat = interpRotationQuat;
    }
    
    this->Position += this->Velocity * dt * speedUp;
    this->MoveHead();
}

void SnakeObject::MoveBody2(GLfloat dt) {
    GLfloat speedUp = this->SpeedUp ? 2.0f : 1.0f;
    
    this->Position += this->Velocity * dt * speedUp;
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
        
        glm::vec2 direction = preNode.Position - curNode.Position;
        direction = glm::normalize(direction);
        curNode.Velocity = direction;
 
        GLfloat distance = glm::distance(preNode.Position,  curNode.Position);
        GLfloat interpFactor = this->NodeDistance / distance;

        /// 线性插值位置
        curNode.Position = glm::mix(preNode.Position, curNode.Position, interpFactor);

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
    this->Position = position;
    this->Velocity = velocity;
}

void SnakeObject::Draw(SpriteRenderer &renderer) {
    if (this->Died) {
        return;
    }
    
    GLfloat t0 = glfwGetTime();
    GLuint size = static_cast<GLuint>(this->Nodes.size());
    for (GLint i = size - 1; i >= 0; i--) {
        this->Nodes[i].Draw(renderer);
    }
    GLfloat t1 = glfwGetTime();
    printf("SnakeObject::Draw duration time %f\n", t1 - t0);// BatchDraw duration time 0.000015
}

void SnakeObject::BatchDraw(SpriteBatchRenderer &renderer) {
    if (this->Died) {
        return;
    }
    
    /**
     几个节点的话，和上面性能差不多，如果是500个以上的节点，性能可以提升一倍以上
     */
    GLfloat t0 = glfwGetTime();
    
    renderer.DrawSprites(this->Nodes);
    
    GLfloat t1 = glfwGetTime();
    printf("SnakeObject::BatchDraw duration time %f\n", t1 - t0);// BatchDraw duration time 0.000015
}

void SnakeObject::BatchGPUDraw(SpriteBatchGPURenderer &renderer) {
    if (this->Died) {
        return;
    }
    
    /**
     几个节点的话，和上面性能差不多，如果是500个以上的节点，性能比批量渲染提升18倍
     */
    GLfloat t0 = glfwGetTime();
    
    renderer.DrawSprites(this->Nodes);
    
    GLfloat t1 = glfwGetTime();
//    printf("SnakeObject::BatchGPUDraw duration time %f\n", t1 - t0);// BatchGPUDraw duration time 0.000015
}
