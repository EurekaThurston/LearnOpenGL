[中文教程](https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/)
[英文教程](https://learnopengl.com/Getting-started/Hello-Triangle)
[[LearnOpenGL/B站code/src/MyCode/02_HelloTriangle/main.cpp|代码]]
[[LearnOpenGL/B站code/src/MyCode/02_HW1/main.cpp|Assignment-1]]
[[LearnOpenGL/B站code/src/MyCode/02_HW2/main.cpp|Assignment-2]]
[[LearnOpenGL/B站code/src/MyCode/02_HW3/main.cpp|Assignment-3]]

 ### Key Term

**顶点数组对象 (Vertex Array Object, VAO)：** 用于储存顶点数据的属性配置。VAO会告诉OpenGL如何解释储存在VBO中的顶点属性，如步长等。
**顶点缓冲对象 (Vertex Buffer Object, VBO)：** 用于储存大量的顶点数据，好一次性发送给显卡，而不是每个顶点发送一次。从CPU把数据发送到显卡比较慢，所以能一块发就一块发。VBO的缓冲类型是GL_ARRAY_BUFFER。

**索引缓冲对象 (Element Buffer Object, EBO / Index Buffer Object, IBO)：** 用于指定绘制的顺序，以减少额外的顶点开销。  

**图形渲染管线：** OpenGL的大部分工作都是关于把3D坐标转换变为适应用户屏幕的2D像素，这个过程由图形渲染管线来完成。管线将一堆原始的图形数据(顶点位置，法线方向，顶点色等)途径一个输送管道，期间经过各种变化处理最终渲染出现在屏幕。管线分为两个主要部分，第一部分把3D坐标转换为2D坐标，第二部分吧2D坐标转变为实际的有颜色的像素。
*注* ：2D坐标和像素是不同的，2D坐标精确的表示一个点在2D空间中的位置，而2D像素是这个点的近似值，2D像素受到屏幕/窗口分辨率的限制。
*注2*：[[图形 1.1 - 渲染流水线|TA百人渲染管线笔记]]

**标准化设备坐标(Normalized Device Coordinate, NDC)**：x, y, z的值都在-1.0到1.0的一个空间，任何落在范围外的坐标都会被裁剪掉，不会显示在屏幕上。当顶点坐标在顶点着色器中处理过后，他们就应该是标准设备坐标。

### Key Code 

**unsigned int VBO(VAO) / unsigned int\[] VBOs\[n](VAOs\[n])**
##### VAO
**glBindeVertexArray(VAO对象)** 绑定VAO。要是用VAO只需要这一步。
##### VBO
**glGenBuffers**(VBO数量，&VBO对象)，用于生成VBO。
**glBindBuffer**(缓冲类型，VBO对象(VBOs\[n]))，用于将创建的VBO绑定到缓冲。
**glBufferData**(缓冲类型，指定传输数据的大小(单位：字节)，实际的数据本身，指定希望显卡如何管理给定的数据)，用于把用户定义的数据复制到当前绑定的缓冲的函数里。
**glVertexAttribPointer**(指定需要配置的顶点属性，指定顶点属性的大小，数据的类型，是否normalize，顶点数据步长，起始位置偏移量)
##### EBO
**unsigned int EBO;**
**glGenBuffers(EBO数量，&EBO对象)**，用于生成EBO
**glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW)**
**glDrawElements(绘制类型，顶点数量，索引的类型，偏移量)**， 在使用EBO的时候需要使用glDrawElements来代替glDrawArrays来进行绘制。

##### Shader
**glShaderSource**(被编译的着色器对象，源码字符串数量，真正的源码，暂时为NULL)