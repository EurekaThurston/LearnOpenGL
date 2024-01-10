// Hello Triangle 练习
// 创建相同的两个三角形.但对它们的数据使用不同的VAO和VBO
// https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/

// GLAD的头文件包含了正确的OpenGL头文件,应在其他依赖于OpenGL的头文件之前包含
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);"
                                 "}\n";

const char *fragmentShaderOrangeSource = "#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "void main()\n"
                                         "{\n"
                                         "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
                                         "}\n";

const char *fragmentShaderYellowSource = "#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "void main()\n"
                                         "{\n"
                                         "FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);"
                                         "}\n";

int main()
{
    // 初始化GLFW
    glfwInit();
    // 使用glfwWindowHint()函数来配置GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // 设置主版本,由于使用的是OpenGL3.3,主版本为3(第一个3), v4.6 => 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // 设置次版本,由于使用的是OpenGL3.3,次版本为3(第二个3), v4.6 => 6
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 告诉GLFW使用的是Core-Profile
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);             // 如果使用的是Mac OS, 加上这行

    // 创建窗口对象
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 设置视口
    glViewport(0, 0, 800, 600); // 前两个参数是渲染窗口左下角的位置,第三、四个参数控制渲染窗口的宽度和高度(像素)

    // 注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 定义顶点数组
    // 两个相同的三角形
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f, // 左
        -0.0f, -0.5f, 0.0f, // 右
        -0.45f, 0.5f, 0.0f, // 上
        };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f, // 左
        0.9f, -0.5f, 0.0f, // 右
        0.45f, 0.5f, 0.0f  // 上
        };

    // 创建VBO, VAO
    // 同时创建两个VAO,VBO
    unsigned int VBOs[2],VAOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    // 第一个三角形设置
    // --------------
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    // 启用顶点属性(默认是禁用的)
    glEnableVertexAttribArray(0);
    // 第二个三角形设置
    // --------------
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);


    

    // -------------------------------------------------------------------------------------
    // Shader 
    // 创建顶点和片段色器
    unsigned int vertexShader;
    unsigned int fragmentShaderOrange;
    unsigned int fragmentShaderYellow;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
    // 附加着色器代码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShaderOrange, 1, &fragmentShaderOrangeSource, NULL);
    glCompileShader(fragmentShaderOrange);
    glShaderSource(fragmentShaderYellow, 1, &fragmentShaderYellowSource, NULL);
    glCompileShader(fragmentShaderYellow);

    // 创建着色器程序对象
    unsigned int shaderProgramOrange;
    unsigned int shaderProgramYellow;
    shaderProgramOrange = glCreateProgram();
    shaderProgramYellow = glCreateProgram();
    // 将第一个着色器附加到第一个着色器程序对象
    glAttachShader(shaderProgramOrange, vertexShader);
    glAttachShader(shaderProgramOrange, fragmentShaderOrange);
    glLinkProgram(shaderProgramOrange);
    // 将第二个着色器附加到第二个着色器程序对象
    glAttachShader(shaderProgramYellow, vertexShader);
    glAttachShader(shaderProgramYellow, fragmentShaderYellow);
    glLinkProgram(shaderProgramYellow);

    // 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderOrange);
    glDeleteShader(fragmentShaderYellow);
    // -------------------------------------------------------------------------------------

    

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制第一个三角形
        glUseProgram(shaderProgramOrange);
        // 设置线框绘制模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // 绘制第二个三角形
        glUseProgram(shaderProgramYellow);
        // 设置线框绘制模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES,0, 3);

        glBindVertexArray(0);

        // 检查并调用事件,交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 资源释放
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteProgram(shaderProgramOrange);
    glDeleteProgram(shaderProgramYellow);

    glfwTerminate();
    return 0;
}
