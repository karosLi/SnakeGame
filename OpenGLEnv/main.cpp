//
//  main.cpp
//  OpenGLEnv
//
//  Created by karos li on 2021/5/12.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "game.h"
#include "resource_manager.h"

#define GRID_COLUMNS 40
#define GRID_ROWS 40

using namespace std;

template<typename ... Args>
string string_format(const string& format, Args ... args){
    size_t size = 1 + snprintf(nullptr, 0, format.c_str(), args ...);  // Extra space for \0
    // unique_ptr<char[]> buf(new char[size]);
    char bytes[size];
    snprintf(bytes, size, format.c_str(), args ...);
    return string(bytes);
}

// GLFW function declerations
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// The Width of the screen
const GLuint SCREEN_WIDTH = 600;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

Game SnakeName(SCREEN_WIDTH, SCREEN_HEIGHT);

void board() {
    
}

int main(int argc, char *argv[])
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "贪吃蛇", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // 注册窗口事件
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // buffer 交换间隔，一次屏幕刷新(一帧)就交换一次
    glfwSwapInterval(1);
    
    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 指定清空颜色缓冲的颜色值
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // DeltaTime variables
    GLuint frameCount = 0;
    GLfloat t0 = glfwGetTime(), t1, fps = 0.0f;// 用于计算 FPS
    GLfloat deltaTime = 0.0f; // deltaTime 帧间隔
    GLfloat lastFrame = 0.0f;
    GLfloat frameRate = 1.0/60.0;// 限制在 1秒钟 60 帧
    
    // Initialize game
    SnakeName.Init();
    
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time - per-frame time logic 计算每一帧的绘制时间
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        
        // 轮询和处理事件
        glfwPollEvents();
        
        if (deltaTime >= frameRate)
        {
            // 管理用户点击按键
            SnakeName.ProcessInput(deltaTime);

            // 更新游戏状态
            SnakeName.Update(deltaTime);

            // 渲染
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            SnakeName.Render();
            
            t1 = glfwGetTime();
            if ((t1 - t0) >= 1.0 || frameCount == 0) {// 用于计算 1 秒钟多少帧
                fps = (GLdouble)frameCount / (t1 - t0);
                glfwSetWindowTitle(window, string_format("贪吃蛇 FPS：%.1f", fps).c_str());

                t0 = t1;
                frameCount = 0;
            }
            frameCount++;

            // 交换前后台缓冲
            glfwSwapBuffers(window);

            lastFrame = currentFrame;
        }
    }
    
    // Delete all resources as loaded using the resource manager
    ResourceManager::Clear();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)// ESC 杀掉进程
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)// 按下某个按键
        {
            SnakeName.Keys[key] = GL_TRUE;
        }
        else if (action == GLFW_RELEASE)// 释放某个按键
        {
            SnakeName.Keys[key] = GL_FALSE;
            SnakeName.KeysProcessed[key] = GL_FALSE;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button >=0 && button < 8) {
        
        if (action == GLFW_PRESS)// 按下了某个鼠标按键
        {
            SnakeName.MouseKeys[button] = GL_TRUE;
            GLdouble xpos, ypos;
            // 获取鼠标点击的位置
            glfwGetCursorPos(window, &xpos, &ypos);
            SnakeName.MousePositions[button] = glm::vec2(xpos, ypos);
        } else if (action == GLFW_RELEASE)// 释放某个鼠标按键
        {
            SnakeName.MouseKeys[button] = GL_FALSE;
        }
    }
    
}
