[中文教程](https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/)
[英文教程](https://learnopengl.com/Getting-started/Shaders)


### Key Term

**顶点属性(Vertex Attribute)**：顶点位置，法线，切线，顶点色这种。OpenGL确保最少有16个包含4分量的顶点属性可用。

**数据类型**：

| 类型  | 含义                          |
| ----- | ----------------------------- |
| vecn  | 包含n个float分量的默认向量    |
| bvecn | 包含n个bool分量的向量         |
| ivecn | 包含n个int分量的向量          |
| uvecn | 包含n个unsigned int分量的向量 |
| dvecn | 包含n个double分量的向量       |

**Uniform**：Unifrom是从CPU中的应用向GPU中的着色器发送数据的方式。Uniform是全局的，在每个着色器程序对象中都是独一无二的，可以被着色器程序的任意着色器在任意阶段访问。Uniform不是in也不是out。
*如果声明了uniform但是在GLSL代码中没用过他，编译器会在最后的编译后的版本中移除他，这可能会导师几个非常麻烦的错误。*

**文件的读取**：在main.cpp里读取shader类的时候，路径要从根目录一路写到顶点和片段着色器。



### Key Code

**int nrAttributes; 
glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes); 
std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;**  查询硬件的顶点属性上限。


