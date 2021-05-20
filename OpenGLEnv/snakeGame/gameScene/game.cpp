//
//  sanke_game.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/12.
//

#include "game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "line_renderer.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"

#include "snake_object.h"

#define WINDOW_EDGE 0
#define GRID_ROWS 50
#define GRID_COLS 50

#define GRID_UNIT

/// 渲染
// 四边形渲染对象（可以渲染正方形，长方形和球形）
SpriteRenderer      *SpriteRender;

// 线段渲染对象
LineRenderer        *LineRender;

// 粒子发射器
ParticleGenerator   *Particles;

// 后处理-特效
PostProcessor       *Effects;
GLfloat             ShakeTime = 0.0f;

// 文本
TextRenderer        *Text;

/// 精灵
// 蛇
SnakeObject         *Snake;
// 初始化蛇的速率和方向
const GLfloat   INITIAL_SNAKE_VELOCITY = 100;
const glm::vec2 INITIAL_SNAKE_DIRECTION(0.0f, -1.0f);// 默认向上

Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Lives(3)
{
    glm::vec2 gamePosition = glm::vec2(WINDOW_EDGE, WINDOW_EDGE);
    GLfloat gameWidth = this->Width - 2 * WINDOW_EDGE;
    GLfloat gameHeight = this->Height - 2 * WINDOW_EDGE;
    
    this->GamePosition = gamePosition;
    this->GameWidth = gameWidth;
    this->GameHeight = gameHeight;
    this->GridSize = gameWidth / GRID_ROWS;
}

Game::~Game()
{
    delete SpriteRender;
    delete LineRender;
    delete Particles;
    delete Effects;
    delete Text;
}

void Game::Init()
{
    /// 设置投影矩阵 - 左上角为原点
    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<float>(this->Width),
                                      static_cast<float>(this->Height),
                                      0.0f,
                                      -1.0f, 1.0f);
    
    /// 加载着色器
    ResourceManager::LoadShader("sprite.vs", "sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("line.vs", "line.fs", nullptr, "line");
    ResourceManager::LoadShader("particle.vs", "particle.fs", nullptr, "particle");
    ResourceManager::LoadShader("post_processing.vs", "post_processing.fs", nullptr, "postprocessing");
    
    /// 配置着色器
    Shader spriteShader = ResourceManager::GetShader("sprite");
    spriteShader.Use();
    spriteShader.SetInteger("image", 0);
    spriteShader.SetMatrix4("projection", projection);
    
    Shader lineShader = ResourceManager::GetShader("line");
    lineShader.Use();
    lineShader.SetMatrix4("projection", projection);
    
    
    /// 加载纹理
    // 加载一个空的纹理
    ResourceManager::LoadEmptyTexture();
    // 蛇纹理
    ResourceManager::LoadTexture("snake_head_0.png", GL_TRUE, "snake_head");
    ResourceManager::LoadTexture("snake_body_0.png", GL_TRUE, "snake_body");
    // 粒子
    ResourceManager::LoadTexture("particle.png", GL_TRUE, "particle");
    
    
    /// 创建渲染对象
    // 创建精灵渲染对象
    SpriteRender = new SpriteRenderer(spriteShader);
    // 创建线段渲染对象
    LineRender = new LineRenderer(lineShader);
    // 创建粒子发射器渲染对象
    Particles = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        500
    );
    // 创建特效处理渲染对象
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
    // 创建文本渲染对象
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("OCRAEXT.TTF", 24);
    
    
    /// 创建精灵
    glm::vec2 snakePosition = glm::vec2(this->GamePosition.x + this->GameWidth / 2.0, this->GamePosition.y + this->GameHeight / 2.0);
    
    Texture2D snakeHead = ResourceManager::GetTexture("snake_head");
    Texture2D snakeBody = ResourceManager::GetTexture("snake_body");
    Texture2D snakeSprites[] = {snakeHead, snakeBody, snakeBody};
    glm::vec2 snakeNodeSize = glm::vec2(this->GridSize * 2, this->GridSize * 2);
    // 由于加载的蛇头和身体纹理方向是向上的的，为了让蛇纹理方向与蛇移动方向一致，需要旋转蛇的节点，所以需要顺时针旋转 90 度
    Snake = new SnakeObject(snakePosition, snakeNodeSize, 30, snakeSprites, 3, 90, INITIAL_SNAKE_DIRECTION * INITIAL_SNAKE_VELOCITY, glm::vec4(0.0f, 1.0f, -1.0f, 1.0f));
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)// 游戏中
    {
       /**
        速度=方向 * 速度大小
        */
        /// 处理键盘按键
        if (this->Keys[GLFW_KEY_A])// 按了 A，表示左移
        {
            glm::vec2 left(-1, 0);
            left = glm::normalize(left);
            glm::vec2 snakeDirection = glm::normalize(Snake->Velocity);
            
            if (glm::dot(left, snakeDirection) >= 0) {// 向量点乘的值大于等于 0，说明这两个向量的夹角是小于等于 90 度的
                Snake->Velocity = left * INITIAL_SNAKE_VELOCITY;
            }
        }
        if (this->Keys[GLFW_KEY_D])// 按了 D，表示右移
        {
            glm::vec2 right(1, 0);
            right = glm::normalize(right);
            glm::vec2 snakeDirection = glm::normalize(Snake->Velocity);
            
            if (glm::dot(right, snakeDirection) >= 0) {//  向量点乘的值大于等于 0，说明这两个向量的夹角是小于等于 90 度的
                Snake->Velocity = right * INITIAL_SNAKE_VELOCITY;
            }
        }
        if (this->Keys[GLFW_KEY_W])// 按了 W，表示上移
        {
            // 因为 y 轴正方向朝下，所以 y = -1，表示向上
            glm::vec2 up(0, -1);
            up = glm::normalize(up);
            glm::vec2 snakeDirection = glm::normalize(Snake->Velocity);
            
            if (glm::dot(up, snakeDirection) >= 0) {//  向量点乘的值大于等于 0，说明这两个向量的夹角是小于等于 90 度的
                Snake->Velocity = up * INITIAL_SNAKE_VELOCITY;
            }
        }
        if (this->Keys[GLFW_KEY_S])// 按了 S，表示下移
        {
            glm::vec2 down(0, 1);
            down = glm::normalize(down);
            glm::vec2 snakeDirection = glm::normalize(Snake->Velocity);
            
            if (glm::dot(down, snakeDirection) >= 0) {//  向量点乘的值大于等于 0，说明这两个向量的夹角是小于等于 90 度的
                Snake->Velocity = down * INITIAL_SNAKE_VELOCITY;
            }
        }
        
        /// 处理鼠标按键
        if (this->MouseKeys[GLFW_MOUSE_BUTTON_LEFT])// 按了 左键，表示朝鼠标方向移动
        {
            glm::vec2 mousePos = this->MousePositions[GLFW_MOUSE_BUTTON_LEFT];
            // 点相减等于蛇到鼠标位置的方向向量，就是得到了目标位置
            glm::vec2 targetDirection = glm::normalize(mousePos - Snake->Position);
            glm::vec2 snakeDirection = glm::normalize(Snake->Velocity);
            
            if (glm::dot(targetDirection, snakeDirection) >= 0) {//  向量点乘的值大于等于 0，说明这两个向量的夹角是小于等于 90 度的
                Snake->Velocity = targetDirection * INITIAL_SNAKE_VELOCITY;
            }
        }
        
        if (this->Keys[GLFW_KEY_EQUAL])// 按了 = 表示加速
        {
            Snake->SpeedUp = GL_TRUE;
        } else {
            Snake->SpeedUp = GL_FALSE;
        }
        
        if (this->Keys[GLFW_KEY_SPACE])// 按下空格表示游戏开始
        {
            Snake->Pause = GL_FALSE;
        }
    }
    
    if (this->State == GAME_MENU)// 菜单
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
        {
            this->Level = (this->Level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
        }
        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
        {
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
            this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
        }
    }
    
    if (this->State == GAME_WIN)// 游戏胜利
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
            Effects->Chaos = GL_FALSE;
            this->State = GAME_MENU;
        }
    }
}

void Game::Update(float dt)
{
    Snake->Move(dt);
    
    // 减少抖动时间
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = GL_FALSE;
    }
}
GLboolean food = GL_TRUE;

void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)// 底部游戏渲染
    {
        // Begin rendering to postprocessing quad
        Effects->BeginRender();
        
        glm::vec2 gamePosition = this->GamePosition;
        GLfloat gameWidth = this->GameWidth;
        GLfloat gameHeight = this->GameHeight;
        GLfloat gridSize = this->GridSize;
        
        // 绘制背景
        Texture2D texture = ResourceManager::GetEmptyTexture();
        SpriteRender->DrawSprite(texture, gamePosition, glm::vec2(gameWidth, gameHeight), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        // 绘制网格
        for (GLuint row = 0; row < GRID_ROWS; row++) {
            LineRender->DrawLine(glm::vec2(gamePosition.x, gamePosition.y + gridSize * row), gameWidth, GL_TRUE, 0, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        }
        for (GLuint col = 0; col < GRID_COLS; col++) {
            LineRender->DrawLine(glm::vec2(gamePosition.x + gridSize * col, gamePosition.y), gameHeight, GL_FALSE, 0, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        }
        
//        if (food) {
//            Texture2D foodTexture = ResourceManager::GetEmptyTexture();
//            GLfloat random = rand() % static_cast<GLuint>(gameWidth);
//            glm::vec3 foodPosition = glm::vec3(gamePosition.x + 1, gamePosition.y + 1, 0.0f);
//
//            SpriteRender->DrawSprite(foodTexture, foodPosition, glm::vec2(200, 200), 0.0f, glm::vec4(0.0f, 1.0f, 1.0f, 0.5f));
//        }
        
        // 绘制蛇
        Snake->Draw(*SpriteRender);
    
        // 绘制粒子
        Particles->Draw();
        
        
        // End rendering to postprocessing quad
        Effects->EndRender();
        // Render postprocessing quad
        Effects->Render(glfwGetTime());
        
        /// 文本绘制
        std::stringstream ss; ss << this->Lives;
        Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
    }
    
    if (this->State == GAME_MENU)// 菜单
    {
        Text->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
        Text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
    }
    
    if (this->State == GAME_WIN)// 游戏胜利
    {
        Text->RenderText(
            "You WON!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
        );
        Text->RenderText(
            "Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
        );
    }
}

// 碰撞检测
void Game::DoCollisions()
{
    
}

void Game::ResetLevel()
{
    this->Lives = 3;
}

void Game::ResetPlayer()
{
    
}
