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
unsigned int loadTexture(char const *path, bool gammaCorrection);
// 渲染
void renderCube();
void renderQuad();
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
// HDR曝光
float exposure = 1.0f;
// ---------------------------


int main() {
    // -----初始化GLFW-----
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                    // 使用OpenGL3.x版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                    // 使用OpenGLx.3版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    // 使用Core模式
    glfwWindowHint(GLFW_SAMPLES, 4);                                  // 设置MSAA子采样点数

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

    // -----创建Shader-----
    Shader CubeShader("./src/MyCode/27_Bloom/Shaders/Cube.vert", "./src/MyCode/27_Bloom/Shaders/Cube.frag");
    Shader LightShader("./src/MyCode/27_Bloom/Shaders/Light.vert", "./src/MyCode/27_Bloom/Shaders/Light.frag");
    Shader BlurShader("./src/MyCode/27_Bloom/Shaders/Blur.vert", "./src/MyCode/27_Bloom/Shaders/Blur.frag");
    Shader BloomFinalShader("./src/MyCode/27_Bloom/Shaders/BloomFinal.vert", "./src/MyCode/27_Bloom/Shaders/BloomFinal.frag");

    // -----贴图加载-----
    unsigned int T_box_diffuse = loadTexture("./Assets/T_container_Diffuse.png", true);
    unsigned int T_box_specular = loadTexture("./Assets/T_container_specular.png", true);

    // HDR
    // -----创建浮点帧缓冲-----
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create floating point color buffer
    unsigned int colorBuffer[2];                      
    glGenTextures(2, colorBuffer);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scrWidth, scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffer[i], 0);
    }
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scrWidth, scrHeight);
    // attach buffers
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // -----乒乓FBO-----
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorBuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorBuffers);
    for (int i = 0; i < 2; i++) {
        
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scrWidth, scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            cout << "Framebuffer not complete!" << endl;
        }
    }


    // -----绑定贴图通道-----
    CubeShader.use();
    CubeShader.setInt("Diffuse", 0);
    CubeShader.setInt("Specular", 1);
    BlurShader.use();
    BlurShader.setInt("OriginalScene", 0);
    BloomFinalShader.use();
    BloomFinalShader.setInt("HDRScene", 0);
    BloomFinalShader.setInt("BlurScene", 1);

    // -----灯光信息-----
    // positions
    std::vector<glm::vec3> lightPos;
    lightPos.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
    lightPos.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    lightPos.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
    lightPos.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
    // colors
    std::vector<glm::vec3> lightCol;
    lightCol.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
    lightCol.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
    lightCol.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
    lightCol.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

    // -----OpenGL渲染设置-----
    // 深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // 模板测试
    // glEnable(GL_STENCIL_TEST);
    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // 开启混合
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 开启面剔除
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // 开启MSAA
    glEnable(GL_MULTISAMPLE);
    // 渲染设置
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);
        glfwSetKeyCallback(window, key_callback);

        // 渲染指令
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DeltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;


        // Pass 1
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 V = camera.GetViewMatrix();
        glm::mat4 P = glm::perspective(glm::radians(camera.Zoom), (float)scrWidth/(float)scrHeight, 0.1f, 100.0f);
        glm::mat4 M = glm::mat4(1.0f);
        CubeShader.use();
        CubeShader.setMat4("V", V);
        CubeShader.setMat4("P", P);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, T_box_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, T_box_specular);
        // 设置灯光位置和颜色
        for (int i = 0; i < lightPos.size(); i++) {
            CubeShader.setVec3("lights[" + to_string(i) + "].Position", lightPos[i]);
            CubeShader.setVec3("lights[" + to_string(i) + "].Color", lightCol[i]);
        }
        // 地板
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
        M = glm::scale(M, glm::vec3(12.5f, 0.5f, 12.5f));
        CubeShader.setMat4("M", M);
        CubeShader.setVec3("camPos", camera.Position);
        renderCube();

        // 场景中的其他小方块
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, T_box_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, T_box_specular);
        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, 1.5f, 0.0));
        M = glm::scale(M, glm::vec3(0.5f));
        CubeShader.setMat4("M", M);
        renderCube();

        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(2.0f, 0.0f, 1.0));
        M = glm::scale(M, glm::vec3(0.5f));
        CubeShader.setMat4("M", M);
        renderCube();

        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(-1.0f, -1.0f, 2.0));
        M = glm::rotate(M, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        CubeShader.setMat4("M", M);
        renderCube();

        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, 2.7f, 4.0));
        M = glm::rotate(M, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        M = glm::scale(M, glm::vec3(1.25));
        CubeShader.setMat4("M", M);
        renderCube();

        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(-2.0f, 1.0f, -3.0));
        M = glm::rotate(M, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        CubeShader.setMat4("M", M);
        renderCube();

        M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(-3.0f, 0.0f, 0.0));
        M = glm::scale(M, glm::vec3(0.5f));
        CubeShader.setMat4("M", M);
        renderCube();

        // 光源方块
        LightShader.use();
        LightShader.setMat4("V", V);
        LightShader.setMat4("P", P);
        for (int i = 0; i < lightPos.size(); i++) {
            M = glm::mat4(1.0f);
            M = glm::translate(M, lightPos[i]);
            M = glm::scale(M, glm::vec3(0.25f));
            LightShader.setMat4("M", M);
            LightShader.setVec3("lightColor", lightCol[i]);
            renderCube();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Pass 2 模糊
        bool horizontal = true, firstIteration = true;
        unsigned int blurAmount = 50;
        BlurShader.use();
        for (int i = 0; i < blurAmount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            BlurShader.setInt("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffer[1] : pingpongColorBuffers[!horizontal]);
            renderQuad();
            horizontal = !horizontal;
            if (firstIteration) {
                firstIteration = false;
            }
        }


        // Pass 3 混合
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);       // 未经模糊处理的HDR场景
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
        BloomFinalShader.use();
        BloomFinalShader.setFloat("exposure", exposure);
        renderQuad();
        

        glfwSwapBuffers(window);    // 交换缓冲
        glfwPollEvents();           // 检查并调用事件
    }

    // 关闭窗口
    glfwTerminate();
    return 0;
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                                // right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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
unsigned int loadTexture(char const *path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
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

