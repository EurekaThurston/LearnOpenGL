// ----------包含文件----------
// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// 数学
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// 图片处理
#include <LearnOpenGL/stb_image.h>
// Shader类
#include <LearnOpenGL/Shader_s.h>
// 摄像机类
#include <LearnOpenGL/Camera.h>
// 模型类
#include <LearnOpenGL/Model.h>
// C++
#include <iostream>
// ---------------------------


// ----------命名空间----------
using namespace std;
// ---------------------------


// ----------创建函数----------
// 调整窗口大小回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// 处理用户输入
void processInput(GLFWwindow *window);
// 用户输入(按一次只触发一次)
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
// 计算摄像机俯仰角和偏航角
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// 滚轮调整FOV来进行缩放
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// 纹理加载
unsigned int loadTexture(char const *path); 
// ---------------------------

// ----------全局参数----------
// 窗口大小
unsigned int scrWidth = 1920;
unsigned int scrHeight = 1080;
// 摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// DeltaTime
float deltaTime = 0.0f;     // 渲染当前帧与上一帧的时间差
float lastTime = 0.0f;      // 完成渲染上一帧的时间
// 鼠标参数
bool firstMouse = true;           // 是否是第一次打开窗口
float lastX = scrWidth / 2.0f;    // 上一帧鼠标X轴位置
float lastY = scrHeight / 2.0f;   // 上一帧鼠标Y轴位置
// 摄像机参数
float yaw = -90.0f; // 0.0f的偏航角会指向正右方向，所以需要把他转到正前方
float pitch = 0.0f;
float fov = 45.0f;
float maxFov = 45.0f;
// // 光源位置
// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
// 聚光灯开关
float spotLightSwitch = 1.0;
// ---------------------------


int main() {
    // -----初始化GLFW-----
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                    // 使用OpenGL3.x版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                    // 使用OpenGLx.3版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    // 使用Core模式

    // -----创建窗口对象-----
    GLFWwindow *window = glfwCreateWindow(scrWidth, scrHeight, "LearnOpenGL", NULL, NULL);    // 设置窗口宽高、名称
    glfwMakeContextCurrent(window);                                                           // 设置OpenGLContext
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // -----注册窗口变化监听-----
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);                        // 注册窗口大小函数
    glfwSetCursorPosCallback(window, mouse_callback);                                         // 注册鼠标输入函数
    glfwSetScrollCallback(window, scroll_callback);                                           // 注册鼠标滚轮函数

    // -----设置视口-----
    glViewport(0, 0, scrWidth, scrHeight);

    // -----创建和编译Shader-----
    Shader ObjectShader("./src/MyCode/17_Blending/ObjectShader.vert", "./src/MyCode/17_Blending/ObjectShader.frag");
    Shader OutlineShader("./src/MyCode/17_Blending/Outline.vert", "./src/MyCode/17_Blending/Outline.frag");

    // -----加载模型-----
    Model Cube("./Assets/Mesh/TestScene/Cube.obj");
    Model Sphere("./Assets/Mesh/TestScene/Sphere.obj");
    Model Torus("./Assets/Mesh/TestScene/Torus.obj");
    Model Cone("./Assets/Mesh/TestScene/Cone.obj");
    Model Floor("./Assets/Mesh/TestScene/Plane.obj");

    // -----OpenGL渲染设置-----
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);
        glfwSetKeyCallback(window, key_callback);

        // 渲染指令
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // DeltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // View, Projection矩阵构建
        glm::mat4 V = camera.GetViewMatrix();
        glm::mat4 P = glm::perspective(glm::radians(camera.Zoom), (float)scrWidth/(float)scrHeight, 0.1f, 100.0f);

        OutlineShader.use();
        OutlineShader.setMat4("V", V);
        OutlineShader.setMat4("P", P);

        ObjectShader.use();
        ObjectShader.setMat4("V", V);
        ObjectShader.setMat4("P", P);
        ObjectShader.setVec3("lDir", -(glm::vec3(-0.5f, -0.5f, -0.5f)));
        ObjectShader.setVec3("camPos", camera.Position);

        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, -5.0f, -10.0f)); // translate it down so it's at the center of the scene
        ObjectShader.setMat4("M", M);
        ObjectShader.setMat3("M_normal", (glm::mat3(glm::transpose(glm::inverse(M)))));
        glStencilMask(0x00);
        Floor.Draw(ObjectShader);

        float scale = 1.05f;

        glStencilFunc(GL_ALWAYS, 1, 0xFF);      // 所有片段都更新模板华冲
        glStencilMask(0xFF);        // 启用模板缓冲写入
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(-5.0f, -3.5f, -10.0f));
        ObjectShader.use();
        ObjectShader.setMat4("M", M);
        Cube.Draw(ObjectShader);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(-5.0f, -3.5f, -10.0f));
        M = glm::scale(M, glm::vec3(scale));
        OutlineShader.use();
        OutlineShader.setMat4("M", M);
        Cube.Draw(OutlineShader);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);

        glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有片段都更新模板华冲
        glStencilMask(0xFF);               // 启用模板缓冲写入
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, -3.5f, -5.0f));
        ObjectShader.use();
        ObjectShader.setMat4("M", M);
        Sphere.Draw(ObjectShader);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, -3.5f, -5.0f));
        M = glm::scale(M, glm::vec3(scale));
        OutlineShader.use();
        OutlineShader.setMat4("M", M);
        Sphere.Draw(OutlineShader);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);

        glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有片段都更新模板华冲
        glStencilMask(0xFF);               // 启用模板缓冲写入
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(5.0f, -3.5f, -10.0f));
        ObjectShader.use();
        ObjectShader.setMat4("M", M);
        Torus.Draw(ObjectShader);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(5.0f, -3.5f, -10.0f));
        M = glm::scale(M, glm::vec3(scale));
        OutlineShader.use();
        OutlineShader.setMat4("M", M);
        Torus.Draw(OutlineShader);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);

        glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有片段都更新模板华冲
        glStencilMask(0xFF);               // 启用模板缓冲写入
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, -3.5f, -15.0f));
        ObjectShader.use();
        ObjectShader.setMat4("M", M);
        Cone.Draw(ObjectShader);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, -3.5f, -15.0f));
        M = glm::scale(M, glm::vec3(scale));
        OutlineShader.use();
        OutlineShader.setMat4("M", M);
        Cone.Draw(OutlineShader);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);

        glfwSwapBuffers(window);    // 交换缓冲
        glfwPollEvents();           // 检查并调用事件
    }

    // 关闭窗口
    glfwTerminate();
    return 0;
}


// ----------设置函数----------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        spotLightSwitch *= -1.0f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// 加载纹理贴图
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}