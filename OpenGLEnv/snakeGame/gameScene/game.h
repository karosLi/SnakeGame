//
//  sanke_game.hpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/12.
//

#ifndef SANKE_GAME_H
#define SANKE_GAME_H

#include <tuple>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_object.h"

// 代表了游戏的当前状态
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
//    glm::vec2 gamePosition = glm::vec2(WINDOW_EDGE, WINDOW_EDGE);
//    GLfloat gameWidth = this->Width - 2 * WINDOW_EDGE;
//    GLfloat gameHeight = this->Height - 2 * WINDOW_EDGE;
    private:
        glm::vec2   GamePosition;// 游戏场景位置
        GLfloat     GameWidth;// 游戏场景宽度
        GLfloat     GameHeight;// 游戏场景高度
        GLuint      GridSize;// 格子大小
    public:
        // 游戏状态
        GameState  State;
        GLboolean  Keys[1024];// 外部输入的按键数组，按下就是 true，释放就是 false
        GLboolean  KeysProcessed[1024];// 外部输入的按键数组，记录按键是否又被处理
        GLboolean  MouseKeys[8];// 外部输入的鼠标按钮数组，按下就是 true，释放就是 false
        glm::vec2  MousePositions[8];// 外部输入的鼠标所在位置数组
        GLuint     Width, Height;// 游戏窗口宽高
        GLuint     Lives;// 玩家生命值
//        std::vector<GameLevel>  Levels;// 关卡数组
        unsigned int            Level;// 当前关卡
//        std::vector<PowerUp>  PowerUps;// 道具
        // 构造函数/析构函数
        Game(GLuint width, GLuint height);
        ~Game();
        // 初始化游戏状态（加载所有的着色器/纹理/关卡）
        void Init();
        // 根据按键输入更新位移
        void ProcessInput(GLfloat dt);
        // 根据时间更新位置
        void Update(GLfloat dt);
        // 渲染画面
        void Render();
        // 碰撞检测
        void DoCollisions();
        // 生成道具
        void SpawnPowerUps(GameObject &block);
        // 更新所有激活的道具
        void UpdatePowerUps(GLfloat dt);
        // 重置游戏
        void ResetLevel();
        void ResetPlayer();
};

#endif /* sanke_game_hpp */
