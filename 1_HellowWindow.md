[中文教程](https://learnopengl-cn.github.io/01%20Getting%20started/03%20Hello%20Window/)
[英文教程](https://learnopengl.com/Getting-started/Hello-Window)

### Key Term

**Context(上下文)**: 记录了OpenGL渲染需要的所有信息和状态，里面记录了当前绘制使用的颜色、是否有光照计算以及开启的光源等使用OpenGL函数调用设置的状态和状态属性。

**GLAD**: 用来管理OpenGL的函数指针，在调用任何OpenGL的函数之前我们需要初始化GLAD。

**Double Buffer(双缓冲)**:  由于图像是从左到右，从上到下逐像素绘制的，单缓冲会导致将渲染的过程显示给了用户(图像闪烁)。双缓冲，即**前缓冲**保存最终输出的图像，而图像在**后缓冲**上渲染，可以使图像立即完整的显示出来。

**清空屏幕**: 在每个新的渲染迭代(每一帧?)开始时需要清屏，不然上一帧的画面还会在当前帧显示。使用glClear函数来清空屏幕的缓冲。

### Key Code

**glfwWindowShouldClose**函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话该函数返回`true`然后渲染循环便结束了，之后为我们就可以关闭应用程序了。

**glfwPollEvents**函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。

**glfwSwapBuffers**函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。

**glClear**函数会清空屏幕的缓冲，可以通过输入参数GL_COLOR_BUFFER_BIT,GL_DEPTH_BUFFER_BIT,GL_STENCIL_BUFFER_BIT来清空颜色、深度、模板缓冲。

**glClearColor(RGBA)** 函数用来设置清空屏幕所用的颜色，当调用glClear来清空颜色缓冲之后，整个屏幕的颜色都会背填充为glClearColor里所设置的颜色。

### 注意事项

渲染循环结束后(程序要退出了)，我们需要正确释放/删除之前分配的所有资源，通过在main函数的最后调用glfwTerminate函数来完成。

