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
// 计算摄像机俯仰角和偏航角
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// 滚轮调整FOV来进行缩放
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
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
// 光源位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
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

    // -----顶点数组-----
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // -----创建和编译Shader-----
    Shader ObjectShader("./src/MyCode/08_Color_Default_Scene/ObjectShader.vert", "./src/MyCode/08_Color_Default_Scene/ObjectShader.frag");
    Shader LightShader("./src/MyCode/08_Color_Default_Scene/LightShader.vert", "./src/MyCode/08_Color_Default_Scene/LightShader.frag");
    ObjectShader.use();     // 启用此Shader

    // -----创建VAO,VBO-----
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // 绑定VAO,VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //填充VBO数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0); // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float))); // 纹理坐标
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // -----灯光VAO-----
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // -----OpenGL渲染设置-----
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);

        // 渲染指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DeltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // View, Projection矩阵构建
        glm::mat4 V = glm::mat4(1.0f);
        glm::mat4 P = glm::mat4(1.0f);

        // 绘制物体正方体
        ObjectShader.use();
        ObjectShader.setVec3("baseCol", 1.0f, 0.5f, 0.3f);
        ObjectShader.setVec3("lightCol", 1.0f, 1.0f, 1.0f);
        // Model矩阵构建
        glm::mat4 M_obj = glm::mat4(1.0f);
        M_obj = glm::translate(M_obj, glm::vec3(0.0f, 0.0f, -3.0f));
        V = camera.GetViewMatrix();
        P = glm::perspective(glm::radians(camera.Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);
        ObjectShader.setMat4("M", M_obj);
        ObjectShader.setMat4("V", V);
        ObjectShader.setMat4("P", P);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);    // 绘制三角形
        
        // 绘制光源正方体
        LightShader.use();
        // Model矩阵构建
        glm::mat4 M_light = glm::mat4(1.0f);
        M_light = glm::translate(M_light, lightPos);
        M_light = glm::scale(M_light, glm::vec3(0.2f));
        LightShader.setMat4("M", M_light);
        LightShader.setMat4("V", V);
        LightShader.setMat4("P", P);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);    // 交换缓冲
        glfwPollEvents();           // 检查并调用事件
    }

    // 资源释放
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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