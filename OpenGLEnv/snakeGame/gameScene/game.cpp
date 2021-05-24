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
#include "foods_manager.h"

GLuint GRID_ROWS = 0;
GLuint GRID_COLS = 0;

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
const GLfloat   INITIAL_SNAKE_VELOCITY = 150;
const glm::vec2 INITIAL_SNAKE_DIRECTION(0.0f, -1.0f);// 默认向上

// 食物管理
FoodsManager        *FoodsMgr;

void LoadTextures(GLuint count, std::string filePrefix);
std::vector<Texture2D> GetSkinTextures(std::string headPrefix, std::string bodyPrefix, std::string tailPrefix, GLuint number);
std::vector<Texture2D> GetTextures(GLuint count, std::string filePrefix);

Game::Game(GLuint width, GLuint height)
    : State(GAME_MENU), Keys(), Width(width), Height(height), Lives(3)
{
    glm::vec2 mapOrigin = glm::vec2(0, 0);
    GLuint mapScale = 4;
    GLfloat mapWidth = this->Width * mapScale;
    GLfloat mapHeight = this->Height * mapScale;
    
    this->MapOrigin = mapOrigin;
    this->MapWidth = mapWidth;
    this->MapHeight = mapHeight;
    this->GridSize = 24.0;
    GRID_ROWS = this->MapWidth / 24.0;
    GRID_COLS = this->MapHeight / 24.0;
}

Game::~Game()
{
    delete SpriteRender;
    delete LineRender;
    delete Particles;
    delete Effects;
    delete Text;
    delete Snake;
    delete FoodsMgr;
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
    LoadTextures(6, "skin_head");
    LoadTextures(6, "skin_body");
    LoadTextures(6, "skin_tail");
    // 加载食物
    LoadTextures(14, "food");
    
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
    // 蛇
    std::vector<Texture2D> snakeSprites = GetSkinTextures("skin_head", "skin_body", "skin_tail", 4);
    // 由于加载的蛇头和身体纹理方向是向上的的，为了让蛇纹理方向与蛇移动方向一致，需要旋转蛇的节点，所以需要顺时针旋转 90 度
    Snake = new SnakeObject(glm::vec2(this->MapOrigin.x + this->MapWidth / 2.0, this->MapOrigin.y + this->MapHeight / 2.0), glm::vec2(24, 24), 5, snakeSprites, 90, INITIAL_SNAKE_DIRECTION * INITIAL_SNAKE_VELOCITY, glm::vec4(0.0f, 1.0f, -1.0f, 1.0f));
    
    // 食物
    std::vector<Texture2D> foodSprites = GetTextures(14, "food");
    FoodsMgr = new FoodsManager(this->MapOrigin, glm::vec2(this->MapWidth, this->MapHeight), foodSprites);
    FoodsMgr->GenerateSpriteFoods(300, glm::vec2(24, 24));
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
        
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])// 按下回车键表示游戏继续
        {
            Snake->Pause = GL_FALSE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }
    
    if (this->State == GAME_MENU)// 菜单
    {
        if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE]) // 按下空格表示游戏开始
        {
            this->State = GAME_ACTIVE;
            Snake->Pause = GL_FALSE;
            this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
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
    
    this->UpdateCamera();
    
    FoodsMgr->Update(dt);
    
    this->DoCollisions();
    
    Particles->Update(dt, Snake->Nodes[0], 3);
    
    // 减少抖动时间
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = GL_FALSE;
    }
    
    // 游戏结束检测
    if (Snake->Died) {
        // 如果蛇死亡则激活shake特效
        ShakeTime = 0.05f;
        Effects->Shake = true;
        
        --this->Lives;
        // 玩家是否已失去所有生命值? : 游戏结束
        if (this->Lives == 0)
        {
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }
}

void Game::UpdateCamera()
{
    // 摄像机跟随蛇头移动
    glm::vec2 snakePostion = Snake->Position;
    
    GLfloat left = snakePostion.x - static_cast<float>(this->Width / 2.0);
    GLfloat right = left + static_cast<float>(this->Width);
    GLfloat top = snakePostion.y - static_cast<float>(this->Height / 2.0);
    GLfloat bottom = top + static_cast<float>(this->Height);
    glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    
    Shader spriteShader = ResourceManager::GetShader("sprite");
    spriteShader.Use();
    spriteShader.SetMatrix4("projection", projection);
    
    Shader lineShader = ResourceManager::GetShader("line");
    lineShader.Use();
    lineShader.SetMatrix4("projection", projection);
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)// 底部游戏渲染
    {
        // Begin rendering to postprocessing quad
        Effects->BeginRender();
        
        glm::vec2 mapOrigin = this->MapOrigin;
        GLfloat mapWidth = this->MapWidth;
        GLfloat mapHeight = this->MapHeight;
        GLfloat gridSize = this->GridSize;
        
        // 绘制场景背景
        Texture2D sceneTexture = ResourceManager::GetEmptyTexture();
        SpriteRender->DrawSprite(sceneTexture, glm::vec2(-static_cast<float>(this->Width / 2.0), -static_cast<float>(this->Height / 2.0)), glm::vec2(mapWidth + this->Width, mapHeight + this->Height), glm::vec4(0.35f, 0.68f, 0.38f, 1.0f));
        
        // 绘制地图背景
        Texture2D bgTexture = ResourceManager::GetEmptyTexture();
        SpriteRender->DrawSprite(bgTexture, mapOrigin, glm::vec2(mapWidth, mapHeight), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        // 绘制网格
        for (GLuint row = 0; row < GRID_ROWS; row++) {
            LineRender->DrawLine(glm::vec2(mapOrigin.x, mapOrigin.y + gridSize * row), mapWidth, GL_TRUE, 0, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        }
        for (GLuint col = 0; col < GRID_COLS; col++) {
            LineRender->DrawLine(glm::vec2(mapOrigin.x + gridSize * col, mapOrigin.y), mapHeight, GL_FALSE, 0, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        }
        
        // 绘制食物
        FoodsMgr->Draw(*SpriteRender);
        
        // 绘制粒子
        Particles->Draw();
        
        // 绘制蛇
        Snake->Draw(*SpriteRender);
        
        
        // End rendering to postprocessing quad
        Effects->EndRender();
        // Render postprocessing quad
        Effects->Render(glfwGetTime());
        
        /// 文本绘制
        std::stringstream lives; lives << this->Lives;
        Text->RenderText("Lives:" + lives.str(), 5.0f, 5.0f, 1.0f);
        
        std::stringstream nodeLength; nodeLength << Snake->Nodes.size();
        Text->RenderText("Score:" + nodeLength.str(), 150.0f, 5.0f, 1.0f);
    }
    
    if (this->State == GAME_ACTIVE && Snake->Pause)// 游戏中
    {
        Text->RenderText("Press ENTER to reborn", 140.0f, this->Width / 2, 1.0f);
    }
    
    if (this->State == GAME_MENU)// 菜单
    {
        Text->RenderText("Press SPACE to start", 140.0f, this->Width / 2  - 40.0, 1.0f);
        Text->RenderText("Press W/S/A/D to control direction", 50.0f, this->Height / 2, 1.0f);
        Text->RenderText("Press + to speed up", 145.0f, this->Height / 2 + 40, 1.0f);
    }
}

// collision detection
GLboolean CheckCollision(GameObject &one, GameObject &two);// AABB 碰撞检测

// 碰撞检测
void Game::DoCollisions()
{
    // 蛇是否碰到了食物
    for (GameObject &food : FoodsMgr->Foods) {
        if (!food.Destroyed) {
            GLboolean collision = CheckCollision(Snake->Nodes[0], food);
            if (collision) {
                Snake->EatFood(food.Position);
                food.Destroyed = GL_TRUE;
            }
        }
    }
    
    // 蛇是否有撞墙
    if (Snake->Position.x < this->MapOrigin.x ||
        Snake->Position.y < this->MapOrigin.y ||
        Snake->Position.x + Snake->NodeSize.x > (this->MapOrigin.x + this->MapWidth) ||
        Snake->Position.y + Snake->NodeSize.y > (this->MapOrigin.y + this->MapHeight)) {
        Snake->Die();
    }
}

void Game::ResetLevel()
{
    this->Lives = 3;
    Snake->Reset(glm::vec2(this->MapOrigin.x + this->MapWidth / 2.0, this->MapOrigin.y + this->MapHeight / 2.0), INITIAL_SNAKE_DIRECTION * INITIAL_SNAKE_VELOCITY);
    Snake->Restart();
    Snake->Pause = GL_TRUE;
}

void Game::ResetPlayer()
{
    Snake->Reset(glm::vec2(this->MapOrigin.x + this->MapWidth / 2.0, this->MapOrigin.y + this->MapHeight / 2.0), INITIAL_SNAKE_DIRECTION * INITIAL_SNAKE_VELOCITY);
    Snake->Reborn();
    Snake->Pause = GL_TRUE;
}

/// AABB 碰撞检测
GLboolean CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    /**
     我们检查第一个物体的最右侧是否大于第二个物体的最左侧并且第二个物体的最右侧是否大于第一个物体的最左侧；垂直的轴向与此相似。
     */
    // x轴方向碰撞？
    GLboolean collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // y轴方向碰撞？
    GLboolean collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // 只有两个轴向都有碰撞时才碰撞
    return collisionX && collisionY;
}

void LoadTextures(GLuint count, std::string filePrefix)
{
    for (GLuint i = 0; i < count; i++) {
        std::stringstream str, name;
        str << filePrefix << "_" << i << ".png";
        name << filePrefix << "_" << i;
        ResourceManager::LoadTexture(str.str().c_str(), GL_TRUE, name.str());
    }
}

std::vector<Texture2D> GetSkinTextures(std::string headPrefix, std::string bodyPrefix, std::string tailPrefix, GLuint number)
{
    std::vector<Texture2D> sprites;
    for (GLuint i = 0; i < 3; i++) {
        
        std::stringstream name;
        if (i == 0) {
            name << headPrefix;
        } else if (i == 1) {
            name << bodyPrefix;
        } else if (i == 2) {
            name << tailPrefix;
        }
        name << "_" << number;
        
        Texture2D sprite = ResourceManager::GetTexture(name.str());
        sprites.push_back(sprite);
    }
    
    return sprites;
}

std::vector<Texture2D> GetTextures(GLuint count, std::string filePrefix)
{
    std::vector<Texture2D> sprites;
    for (GLuint i = 0; i < count; i++) {
        std::stringstream name;
        name << filePrefix << "_" << i;
        
        Texture2D sprite = ResourceManager::GetTexture(name.str());
        sprites.push_back(sprite);
    }
    
    return sprites;
}
