# OpenGL高级功能
#### 目录
```toc
```
#### 深度测试(Depth Test)
- ###### 深度缓冲
	- 深度缓冲用于储存每个片段的**深度值**， 当深度测试(Depth Testing)开启时，OpenGL会将当前渲染片段的深度值与深度缓冲里的深度值进行对比，执行深度测试，如果如果测试通过，深度缓冲的值会更新为新的深度值，如果失败则该片段会被舍弃
	- 深度缓冲又窗口系统自动创建，以16、24或32位的float的形式储存他的深度值。**大部分系统中深度缓冲的精度是24位**
	- 深度缓冲在**片段着色器以及模板测试运行之后**，在**屏幕空间**中运行的
		- 屏幕空间坐标可以在片段着色器中通过gl_FragCoord直接访问
		- gl_FragCoord的x，y分量分别为片段的屏幕空间坐标 ((0,0)为左下角)。而z分量为片段的深度值，即需要与深度测试中需要的值
	- *提前深度测试(Early Depth Testing)：现在大部分GPU都提供的硬件特性，允许深度测试在片段着色器之前运行。当使用提前深度测试时，片段着色器无法写入深度值* 
- ###### 深度测试设置
	- 深度测试默认禁用，需要通过GL_DEPTH_TEST来启用
		- <mark>glEnable(GL_DEPTH_TEST);</mark>
	- 开启深度测试后，应该在每次渲染迭代前(while循环的一开始)清除深度缓冲，否则会使用上一次渲染迭代写入的深度值与当前渲染迭代的片段的深度值进行对比
		- <mark>glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);</mark>
	- 执行深度测试并丢弃相应的片段，但是**不**更新深度缓冲，即使用一个只读(Read-Only)深度缓冲。禁用深度缓冲的写入只需要设置他的深度掩码(Depth Mask)为GL_FALSE即可。**只有在开启深度测试时才有效**
		- <mark>glDepthMask(GL_FALSE)</mark>
- ###### 深度测试函数
	- 深度测试的比较运算符是可配置的。可以用来控制OpenGL什么时候该通过或丢弃一个片段，什么时候该更新深度缓冲。通过glDepthFunc来设置
		- <mark>glDepthFunc(GL_LESS)</mark>
	- 默认情况下使用GL_LESS，他会丢弃深度值大于等于当前深度缓冲值的所有片段

| 函数        | 描述                                       |
| ----------- | ------------------------------------------ |
| GL_ALWAYS   | **永远**通过深度测试                           |
| GL_NEVER    | **永远不**通过深度测试                         |
| GL_LESS     | 在片段深度值**小于**缓冲的深度值时通过测试     |
| GL_EQUAL    | 在片段深度值**等于**缓冲的深度值时通过测试     |
| GL_LEQUAL   | 在片段深度值**小于等于**缓冲的深度值时通过测试 |
| GL_GREATER  | 在片段深度值**大于**缓冲的深度值时通过测试     |
| GL_NOTEQUAL | 在片段深度值**不等于**缓冲的深度值时通过测试   |
| GL_GEQUAL   | 在片段深度值**大于等于**缓冲的深度值时通过测试                                           |
*注：越小的深度值意味着离摄像机越近*

- ###### 深度值精度
	- 深度值介于0到1之间。观察空间的z值可能是视锥体的近平面和远平面之间的任何值，需要把z值变换到\[0,1]的范围之间
		- **线性**
			- ![[OpenGL_深度值线性变换.png]]
				- near、far是在投影矩阵里设置的near和far值
			- ![[OpenGL_深度值线性图.png]]
			- *实践中几乎永远不会使用，因为线性的方程使得远处的物体和近处的物体使用同样的精度，会导致在物体距离摄像机很近的时候精准度不够*
		- **非线性**
			- 非线性方程是透视矩阵默认使用的方程
			- 与1/z成正比，在z值很小的时候提供非常高的精度，而在z值很远的时候提供更少的精度
			- ![[OpenGL_深度值非线性方程.png]]
			- ![[OpenGL_深度值非线性变换.png]]
- ###### 深度值的可视化
	- 由于使用非线性方程的关系(越近的片段精度越高，越远的越低)，片段的深度值会随着距离的增加而快速增加，从而使得大部分片段的颜色都趋近于白色，只有里相机非常近的片段才会显示灰色
		- ![[OpenGL_深度值非线性方程可视化.png]]
	- 将非线性方程的深度值变换为线性的深度值可以正确的可视化深度值
		- 先将深度值从\[0,1]范围重新变换到\[-1,1]范围的标准化设备坐标(裁剪空间)
			- <mark>float z = depth \* 2.0 - 1.0;</mark>
		- 然后使用获取到的z值，应用逆变换来获取线性的深度值
			- <mark>float linearDepth = (2.0 * near * far) / (far + near - z * (far - near));</mark>
		- 最后在main函数中将变换为线性的深度值除以far以正确的可视化深度值
			- <mark>float depth = linearizeDepth(gl_FragCoord.z)/far;</mark>
- ###### 深度冲突
	- 如果两个三角形或平面离得特别近，并且是平行的，深度缓冲会没有足够的精度来决定他们俩谁应该在上面，结果就是两个三角形不断地切换前后顺序，这个现象叫**深度冲突(Z-fighting)** 。当物体在远处时更明显 (因为非线性方程在远处的精度更是低上加低)。
	- 深度冲突不能完全避免，但可以通过技巧来减轻/规避
		- 永远不要吧多个物体摆的太靠近，以至于他们的一些三角形会重叠
		- 尽可能将近平面设置的远一些 (离摄像机更远)，越远的近平面会使得视锥体内的整体深度值精度越高。但是近平面太远会导致近处的物体被裁剪掉，需要微调来找到合适的近平面距离
		- 使用更高精度的深度缓冲，代价是牺牲掉一些性能。大部分深度缓冲的精度都是24位的，但是大部分的显卡都支持32位的深度缓冲，32位精度可以极大地提高精度。

#### 模板测试(Stencil Test)
- ###### 模板缓冲
	- 片段着色器处理完一个片段后会进行模板测试(Stencil Test)。模板测试和深度测试一样，都有可能会丢弃片段，被保留下来的片段会进行深度测试。
	- 模板测试使用模板缓冲(stencil buffer)来进行测试
		- 通常模板缓冲中每个模板值(stencil value)是8位的，所以每个像素/片段一共能有256种不同的模板值
		- *每个窗口库都需要配置一个模板缓冲，GLFW自动做了这件事，但是其他的库可能不会*
	- **模板测试的步骤**
		- 启用模板缓冲的写入
		- 渲染物体，更新模板缓冲的内容
		- 禁用模板缓冲的写入
		- 渲染(其他)物体，这次根据模板缓冲中已绘制的其他物体的片段来决定是否丢弃特定的片段
- ###### 模板测试设置
	- 通过GL_STENCIL_TEST来启用模板测试。开启后所有的渲染调用都会以某种方式影响着模板缓冲
		- <mark>glEnable(GL_STENCIL_TEST)</mark>
	- 在每次渲染迭代的开始清除模板缓冲，否则将会使用上次迭代的数据
		- <mark>glClear(GL_STENCIL_BUFFER_BIT)</mark>
	- 可以通过glStencilMask来关闭模板缓冲的写入，默认情况下设置的掩码所有位都为1，不影响输出，但如果设置为0X00，则写入缓冲的所有模板值都会变成0，和深度测试中的glDepthMask(GL_FALSE)一样
		- <mark>glStencilMask(0xFF); // 每一位写入模板缓冲时都保持原样 </mark>
		- <mark>glStencilMask(0x00); // 每一位在写入模板缓冲时都会变成0（禁用写入）</mark>
			- **禁用写入不仅会禁止模板缓冲的写入，还会阻止模板缓冲清空(使glClear(GL_STENCIL_BUFFER_BIT)无效)**
		- *大部分情况下只会使用0x00或者0xFF作为模板掩码(Stencil Mask)，但是可以自定义*
- ###### 模板测试函数
	- 有两个函数可以用来配置模板测试，让我们对模板缓冲应该通过还是失败，以及他应该如何影响模板缓冲有一定的控制
		- <mark>glStencilFunc(GLenum func, GLint ref, GLuint mask)</mark>
			- func：设置模板测试函数(Stencil Test Function)。这个测试函数将会应用到已储存的模板值上和glStencilFunc函数的**ref**值上
			- ref：设置了模板测试的参考值(Reference Value)。模板缓冲的内容将会与这个值进行比较
			- mask：设置一个掩码，它将会与参考值和储存的模板值在测试比较它们之前进行与(AND)运算。初始情况下所有位都为1
		- <mark>glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)</mark>
			- sfail：模板测试失败时采取的行为
			- dpfail：模板测试通过，但深度测试失败时采取的行为
			- dppass：模板测试和深度测试都通过时采取的行为

| 模板测试函数 | 描述                                             |
| ------------ | ------------------------------------------------ |
| GL_ALWAYS    | **永远**通过模板测试                                             |
| GL_NEVER     | **永远不**通过模板测试                           |
| GL_LESS      | 在片段模板值**小于**缓冲的模板值时通过测试       |
| GL_LEQUAL    | 在片段的模板值**小于等于**缓冲的模板值时通过测试 |
| GL_GREATER   | 在片段的模板值**大于缓冲**的模板值时通过测试     |
| GL_GEQUAL    | 在片段的模板值**大于等于**缓冲的模板值时通过测试 |
| GL_EQUAL     | 在片段的模板值**等于**缓冲的模板值时通过测试     |
| GL_NOTEQUAL  | 在片段的模板值**不等于**缓冲的模板值时通过测试   |

| glStencilOp函数行为 | 描述                                                                     |   
| ------------------- | ------------------------------------------------------------------------ | 
| GL_KEEP             | 保存当前储存的模板值                                                     |     
| GL_ZERO             | 将模板值设置为0                                                          |     
| GL_REPLACE          | 将模板值设置为glStencilFunc函数的**ref**值                                    |
| GL_INCR             | 如果模板值小于最大值则将模板值加一                                      |
| GL_INCR_WRAP        | 如果模板值小于最大值则将模板值加一，但如果模板值超过了最大值则归零          |
| GL_DECT             | 如果模板值大于最小值则将模板值减一                                         |
| GL_DECR_WRAP        | 如果模板值大于最小值则将模板值减一 ，但如果模板值小于0则将其设置为最大值    |
| GL_INVERT           | 按**位**翻转当前的模板缓冲值                                                                   |
*默认情况下glStencilOp是设置为(GL_KEEP, GL_KEEP,  GL_KEEP)的，即不论任何测试的结果是如何，模板缓冲都会保留他的值。默认的行为不会更新模板缓冲，所以如果想写入模板缓冲的话，需要至少对其中一个选项设置不同的值*

- ###### 模板测试应用
	- 物体轮廓(Object Outlining)，为每一个物体在周围创建一个有色边框。步骤如下：
		- 在绘制（需要添加轮廓的）物体**之前**，将模板函数设置为GL_ALWAYS，每当物体的片段被渲染时，将模板缓冲更新为1
		- 渲染物体
		- **禁用**模板写入以及深度测试
		- 将每个物体缩放一点点
		- 使用一个不同的片段着色器，输出一个单独的边框颜色
		- 再次绘制物体，但是在他们片段的模板值不等于1的时候才绘制
		- 再次启用模板写入和深度测试

#### 混合(Blending)
- ###### Alpha Cut
	- 在片段着色器中直接舍弃alpha值低于阈值的片段
		- <mark>if (texture.a < 0.1) <br>&emsp;discard; </mark>
	- 贴图的上和下，左和右会进行插值，导致贴图的四周会出现边框无法被丢弃，可以通过把贴图环绕方式改为**GL_CLAMP_TO_EDGE**来解决
- ###### Alpha Blend
	- 可以将半透明物体和物体后面的物体的颜色进行混合来达到半透明的效果
	- 通过GL_BLEND开启
		- <mark>glEnable(GL_BLEND)</mark>
	- **OpenGL的混合方程**
		- ![[OpenGL_混合方程.png]]
			- <span style="color:green">C<sub>source</sub></span> ：源颜色向量，源自纹理的颜色向量，**离相机更近的颜色**
			- <span style="color:green">F<sub>source</sub></span> ：源因子值，指定了alpha值对源颜色的影响
			- <span style="color:red">C<sub>destination</sub></span> ：目标颜色向量，当前储存在颜色缓冲中的颜色向量，即**背景(已经渲染过的片段)/里相机更远的颜色**
			- <span style="color:red">F<sub>destination</sub></span> ：目标因子值，指定了alpha值对目标颜色的影响
			- *只有当片段着色器运行完成，且所有测试都通过之后，混合方程才会应用到片段颜色输出与当前颜色缓冲中的值上*
	- **例子**：
		- ![[OpenGL_混合例子.png]]
		- 要将绿色方块绘制到红色方块的上面 (绿块离相机更近，是**源**，红块里相机更远，是**目标**)，结果为：
		-  ![[OpenGL_混合计算过程.png]]
		- ![[OpenGL_混合例子结果.png]]
	- 可以通过glBlendFunc来设置源和目标因子
		- <mark>glBlendFunc(GLenum sfactor, GLenum dfactor)</mark>
			- sfactor：源因子
			- dfactor：目标因子
	- 也可以通过glBlendFuncSeparate来将颜色和alpha设置为不同的选项
		- <mark>glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)</mark>
	- 还可以通过glBlendEquation来改变方程中源和目标之间的运算符
		- <mark>glBlendEquation(GLenum mode)</mark>
			- GL_FUNC_ADD：默认选项，将两个分量相加，Src + Dst
			- GL_FUNC_SUBTRACT：将两个分量相减，Src - Dst
			- GL_FUNC_REVERSE_SUBTRACT：将两个分量调换顺序并相减：Dst - Src

| 混合因子选项                | 值                        |
| --------------------------- | ------------------------- |
| GL_ZERO                     | 因子等于**0**                 |
| GL_ONE                      | 因子等于**1**                 |
| GL_SRC_COLOR                | 因子等于**源颜色**向量        |
| GL_ONE_MINUS_SRC_COLOR      | 因子等于**1-源颜色**向量      |
| GL_DST_COLOR                | 因子等于**目标颜色**向量      |
| GL_ONE_MINUS_DST_COLOR      | 因子等于**1-目标颜色**向量    |
| GL_SRC_ALPHA                | 因子等于**源alpha**分量       |
| GL_ONE_MINUS_SRC_ALPHA      | 因子等于**1-源alpha**分量     |
| GL_DST_ALPHA                | 因子等于**目标alpha**分量     |
| GL_ONE_MINUS_DST_ALPHA      | 因子等于**1-目标alpha**分量   |
| GL_CONSTANT_COLOR           | 因子等于**常数**向量          |
| GL_ONE_MINUS_CONSTANT_COLOR | 因子等于**1-常数**向量        |
| GL_CONSTANT_ALPHA           | 因子等于**常数向量alpha**分量 |
| GL_MINUS_CONSTANT_ALPHA     | 因子等于**1-常数向量alpha**分量                          |

- ###### 渲染半透明物体与不透明物体的顺序问题
	- 当绘制一个由不透明和透明物体的场景时，大体的原则是：
		- 先绘制所有**不透明**物体
		- 对所有透明物体**排序**
		- **按顺序**绘制所有透明的物体(从远到近)
			- 排序代码：
			- ![[OpenGL_透明物体排序1.png]]
			- ![[OpenGL_透明物体排序2.png]]
- ###### 其他问题
	- 如果在渲染半透明物体之前开启了模板测试，需要在渲染半透明物体之前禁用模板测试写入

#### 面剔除(Face Culling)
- 面剔除用来剔除**无法被摄像机看到**的面 (不面向摄像机的面)，这样可以减少非常大的性能开销
- OpenGL通过分析顶点数据的**环绕顺序** (Winding Order)来确定哪些面是**正向面** (Front Face)， 哪些面是**背向面** (Back Face)
- ###### 环绕顺序
	- 环绕顺序用于帮助OpenGL定义三角形的哪个面是正面哪个面是背面
	- 每组组成三角形的三个顶点的**定义顺序**即为该三角形的环绕顺序
	- ![[OpenGL_三角形环绕顺序.png]]
	- 一个三角形只有两种环绕顺序，**顺时针**和**逆时针**
	- 默认情况下**逆时针**顺序定义的顶点将会被处理为**正向**三角形
	- ![[OpenGL_观察者视角三角形正反面.png]]
	- 当一个三角形的两个面都是逆时针顺序定义的时候，**朝外**的一面是**逆时针**，而**朝向物体内部**的一面则是在渲染时是**顺时针**顺序。可以理解为在一张纸上画一个镂空的图案，面向自己的一面的图案是逆时针的，而当把纸水平翻转过来的时候，面向自己的图案就变成了顺时针，这样就可以区分哪面是正面，哪面是背面
- ###### 面剔除
	- 面剔除默认也是禁用状态，可以通过GL_CULL_FACE来开启
		- <mark>glEnable(GL_CULL_FACE)</mark>
	- 可以通过glCullFace来设置要剔除的是正面还是背面，**默认值是GL_BACK**
		- <mark>glCullFace( GL_BACK)</mark>
	- 还可以通过glFrontFace来告诉OpenGL哪个环绕顺序的面是正向面，**默认是GL_CCW**
		- <mark>glFrontFace(GL_CCW)</mark>

| glCullFace 选项   | 描述         |
| ----------------- | ------------ |
| GL_BACK           | 只剔除背向面 |
| GL_FRONT          | 只剔除正向面 |
| GL_FRONT_AND_BACK | 剔除正向面和背向面             |

| glFrontFace 选项 | 描述                               |
| ---------------- | ---------------------------------- |
| GL_CCW           | 将逆时针顺序顶点定义的面设为正向面 |
| GL_CW            | 将顺时针顺序顶点定义的面设为正向面                                   |

#### 帧缓冲(Frame Buffer)
- **注意事项：**
	- **在使用多个帧缓冲的时候(默认缓冲+一个/多个自定义帧缓冲)，清空缓存的操作(glClear()要在绑定了当前Pass要用的帧缓冲之后再进行一次！！！)**
- 帧缓冲是颜色缓冲、深度缓冲、模板缓冲等各种缓冲结合在一起的缓冲
- 一个完整的帧缓冲需要满足一下条件：
	- 附加**至少一个**缓冲(颜色、深度、模板)
	- 至少有一个**颜色附件**(Attachment)
	- 所有的附件都必须是**完整**的(保留了内存)
	- 每个缓冲都应该有**相同**的样本数
- ###### 创建帧缓冲
	- 通过glGenFramebuffers来创建帧缓冲对象
		- <mark>unsigned int fbo;</mark>
		- <mark>glGenFramebuffers(1, &fbo)</mark>
	- 再使用glBindFramebuffer来绑定帧缓冲
		- <mark>glBindFramebuffer(GL_FRAMEBUFFER, fbo)</mark>
	- 绑定之后所有的**读取**和**写入**帧缓冲的操作将会影响当前绑定的帧缓冲
	- 也可以使用<mark>GL_READ_FRAMEBUFFER</mark>或者<mark>GL_DRAW_FRAMEBUFFER</mark>来将一个帧缓冲分别绑定到读取目标和写入目标
		- 绑定到Read的帧缓冲将会使用在所有如glReadPixels的**读取**操作上
		- 绑定到Draw的帧缓冲将会被用作渲染、清楚等**写入**操作的目标上
		- *大部分情况不需要分开绑定*
	- 通过glCheckFramebufferStatus检查帧缓冲是否完整。他会检测**当前绑定**的帧缓冲。如果返回值是**GL_FRAMEBUFFER_COMPLETE**，帧缓冲就是完整的了
		- <mark>if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)</mark>
		- [返回值列表](**https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml**)
	- 当帧缓冲是完整的了之后，所有的渲染操作将会渲染到当前绑定的帧缓冲的附件中
	- 由于自定义的帧缓冲不是默认的帧缓冲，渲染指令不会对窗口的输出有任何影响，通过再次激活默认帧缓冲，将他绑定到0来解决
		- <mark>glBindFramebuffer(GL_FRAMEBUFFER, 0)</mark>
	- 在完成所有帧缓冲操作后删除该帧缓冲对象
		- <mark>glDeleteFramebuffers(1, &fbo)</mark>
- ###### 附件(Attachment)
	- 在完整性检查之前，需要给帧缓冲附加附件
	- 附件是一个内存位置，能够作为帧缓冲的一个缓冲，可以看做为一个图像
	- 创建附件有两个选项
		- **纹理**
		- **渲染缓冲对象(Renderbuffer Object)**
	- 渲染缓冲对象能为帧缓冲对象提供一些优化
	- 通常的规则是，如果不需要从一个缓冲中采样数据，那么对这个缓冲使用渲染缓冲对象，如果需要从缓冲中采样颜色或深度等数据，那么应该使用纹理附件
- ###### 纹理附件
	- 当把纹理附加给帧缓冲时，所有的渲染指令将会写入到这个纹理图像中，**可以在着色器中进行使用**
	- 创建纹理附件和创建一张普通的纹理对象差不多
		- <mark>unsigned int texture; <br>glGenTextures(1, &texture); <br>glBindTexture(GL_TEXTURE_2D, texture); <br>glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); <br>glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); <br>glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);</mark>
		- 与创建一张纹理对象的区别是
			- 纹理附件将**glTexImage2D的宽高设置为了屏幕的宽高**，因为这张纹理将要作为窗口的输出，需要和窗口一样大 (但这并不是必须的)
			- 将glTexImage2D的纹理实际**data参数设为了NULL**，因为填充这个纹理的数据将会在**渲染到帧缓冲之后**来进行
		- *如果想把屏幕渲染到更大或者更小的纹理上，需要在渲染到帧缓冲之前再调用一次glViewport并使用纹理的新维度作为参数，否则只有一小部分的纹理火屏幕会被渲染到这个纹理上*
		- 创建好纹理之后需要将纹理附件附加到帧缓冲上
			- <mark>glFramebufferTexture2D(target, attachment, textarget, texture, level);</mark>
				- `target`：帧缓冲的目标 (绘制，读取，或者都)
				- `attachment`：要附加的附件类型
					- GL_COLOR_ATTACHMENT0
					- GL_DEPTH_ATTACHMENT
					- GL_STENCIL_ATTACHMENT
					- *颜色附件可以有多个，通过类型末尾的数字设置*
				- `textarget`：附加的纹理类型
					- GL_TEXTURE_2D
					- GL_TEXTURE_CUBE_MAP_POSITIVE_X 
					- GL_TEXTURE_CUBE_MAP_NEGATIVE_X 
					- GL_TEXTURE_CUBE_MAP_POSITIVE_Y 
					- GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 
					- GL_TEXTURE_CUBE_MAP_POSITIVE_Z 
					- GL_TEXTURE_CUBE_MAP_NEGATIVE_Z`
				- `texture`：附加的纹理附件对象本身
				- `level`：Mipmap级别，**必须是0**
			- 可以将深度缓冲和模板缓冲附加到同一个单独的纹理中。纹理的每32位数值将包含24位的深度信息和8位的模板信息
				- <mark>glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL );<br> glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);</mark>
- ###### 渲染缓冲对象附件(Renderbuffer Object)
	- 渲染缓冲对象附件会将数据储存为OpenGL原生的渲染格式，他是为离屏渲染到帧缓冲优化过的
	- 渲染缓冲对象直接将所有的渲染数据储存在它的缓冲中，不会做任何针对纹理格式的转换
	- 渲染缓冲对象通常是只写的，不能读取。但是仍然可以用glReadPixels来读取他，但是是从当前绑定的帧缓冲中返回特定区域的像素而不是附加本身
	- 创建并绑定渲染缓冲对象
		- <mark>unsigned int rbo;<br>glGenRenderbuffers(1, &rbo);<br>glBindRenderbuffer(GL_RENDERBUFFER, rbo);</mark>
	- 因为我们只**需要**深度和模板值用于测试，而不需要对他们进行**采样**，所以通常是只写的渲染缓冲对象很适合用于深度和模板附件
	- 通过调用glRenderbufferStorage来创建深度和模板渲染缓冲对象
		- <mark>glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrWidth, scrHeight);</mark>
	- 最后附加该渲染缓冲对象到当前帧缓冲
		- <mark>glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,  rbo);</mark>
- ###### 渲染到纹理
	- 要将场景绘制到纹理上需要以下步骤：
		- 将新的帧缓冲绑定为激活 的帧缓冲，和往常一样渲染场景
		- 绑定默认的帧缓冲
		- 绘制一个横跨整个屏幕的四边形，将帧缓冲的颜色缓冲作为他的纹理
- ###### 基于帧缓冲的一些后处理
	- 反相(Inversion)
		- 1 - 屏幕纹理的颜色值
		- ![[OpenGL_帧缓冲后处理_反相.png]]
	- 灰度(Grayscale)
		- 三种灰度算法
			- Lightness: 在R, G, B中取最大和最小的两个分量然后取他们俩的平均数
				- <mark>(max(R, G, B) + min(R, G, B)) / 2</mark>
			- Average: 将R, G, B的三个分量相加取平均数
				- <mark>(R + G + B) /3 </mark>
			- Luminosity: 由于人眼对绿色更敏感，而对蓝色不那么敏感，所以为了获取物理上更精确的效果，使用加权的通道，即**R的权重为0.2126，G的权重为0.7152，B的权重为0.0722**
				- <mark>0.2126 \* R + 0.7152 \* G + 0.0722 \* B</mark>
			- ![[OpenGL_帧缓冲后处理_灰度.png]]
	- **核效果**
		- 核(Kernel) / 卷积矩阵(Convolution Matrix)是一个类似矩阵的数值数组，这个数组的中心的值是当前的像素对应的核值，周围的核值是该像素周围的像素所对应的核值。核的每个值都与他所对应的像素相乘，并将结果相加，相加的结果为中心像素新的值
		- ![[OpenGL_核.png]]
		- 在上图的核中，当前像素的新值为
			- 2 \* P<sub>11</sub>  +   2 \* P<sub>12</sub>  +   2 \* P<sub>13</sub>  +   2 \* P<sub>21</sub>  +   2 \* P<sub>23</sub>  +   2 \* P<sub>31</sub>  +   2 \* P<sub>32</sub>  +   2 \* P<sub>33</sub>  +   (-15) \* P<sub>22</sub>
		- 大部分的核都是3X3核
		- 大部分的核的**所有权重加起来应该等于1**，如果不等于1，意味着新的像素值会比原来的像素值更亮 (大于1) 或者更暗 (小于1)
		- 周围像素距离中心像素的距离(采样点)可以自定义，越小的采样点最后的结果越细腻
			- ![[OpenGL_帧缓冲后处理_核偏移量.png]]
	- 核效果应用-**锐化**
		- ![[OpenGL_帧缓冲后处理_锐化核.png]]
		- ![[OpenGL_帧缓冲后处理_锐化.png]]
	- 核效果应用-**模糊**
		- 使用更高阶的高斯模糊的时候可以通过先渲染横向模糊到一个FBO，再将横向模糊的纹理在第二个FBO中进行纵向模糊，最终得到完整的高斯模糊。这样做可以极大的减少采样数以减少开销，越高阶的高斯模糊节约的性能越多
		- ![[OpenGL_帧缓冲后处理_模糊核.png]]
		- ![[OpenGL_帧缓冲后处理_模糊.png]]
	- 核效果应用-**边缘检测**
		- ![[OpenGL_帧缓冲后处理_边缘检测核.png]]
		- ![[OpenGL_帧缓冲后处理_边缘检测.png]]

#### Cubemap
- Cubemap是包含了6个2D纹理的纹理，每个2D纹理组成立方体的一个面
- ###### 创建Cubemap
	- <mark>unsigned int textureID; <br>glGenTextures(1, &textureID); <br>glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);</mark>
	- 由于Cubemap包含了6个面，需要对每一个面调用一次<mark>glTexImage2D</mark>函数，一共6次，每次在调用时设定该纹理对应的是Cubemap的哪个面
	- 可以通过循环从<mark>GL_TEXTURE_CUBE_MAP_POSITIVE_X</mark>开始历遍所有的方位
		- ![[OpenGL_Cubemap纹理历遍.png]]
		- textures_faces包含了所有的纹理目标并按顺序排列
	- 设置好6个面的纹理之后需要对Cubemap的**环绕**和**过滤**方式进行设置
		- 唯一与2D纹理不同的是，由于Cubemap是立体的，需要在环绕方式里**增加一个R坐标**来这是Cubemap的Z方向环绕方式

| 纹理目标                       | 方位 |
| ------------------------------ | ---- |
| GL_TEXTURE_CUBE_MAP_POSITIVE_X | 右   |
| GL_TEXTURE_CUBE_MAP_NEGATIVE_X | 左   |
| GL_TEXTURE_CUBE_MAP_POSITIVE_Y | 上   |
| GL_TEXTURE_CUBE_MAP_NEGATIVE_Y | 下   |
| GL_TEXTURE_CUBE_MAP_POSITIVE_Z | 后   |
| GL_TEXTURE_CUBE_MAP_NEGATIVE_Z | 前     |

- ###### 天空盒
	- 使用Cubemap的**顶点**位置作为UV来进行采样(立方体处于原点)
	- 不给天空盒创建M矩阵，以及**使用无视位移的V矩阵**，来使天空盒看起来很远
	- 使用**提前深度测试(Early Depth Test)**，直接不渲染所有被遮挡的部分，而不是在渲染完所有的片段之后再在深度测试中舍弃被遮挡的片段
		- [关于提前深度测试](https://zhuanlan.zhihu.com/p/72956611)
		- 为了让天空盒被其他所有物体遮挡，在**顶点着色器**中将<mark>gl_Position</mark>的**z**分量设为**w**，因为片段的深度值是由<mark>gl_Position</mark>的**z**分量除以**w**分量得到的，将**z**手动设置为**w**可以让天空盒的片段深度值永远是**最大值1**(离相机最远)
- ###### 环境映射-反射
	- 反射是的颜色，**根据观察者的视角**，或多或少的受环境颜色的影响
	- **反射需要**：
		- 视方向的反方向
		- 法线方向
	- 通过**视方向的反方向**和**法线方向**求得**观察方向沿法线方向的反射方向**，并使用该向量作为UV值在Cubemap上进行采样从而获得模型当前片段的镜面反射颜色
	- ![[OpenGL_环境映射_反射.png]]
- ###### 环境映射-折射
	- 折射是由于光线由于**传播介质的改变**而产生的**方向的改变**，折射通过斯涅尔定律 (Snell's Law) 来描述
	- ![[OpenGL_环境映射_折射示意图.png]]
	- 折射需要：
		- 视方向的反方向
		- 法线方向
	- 通过**视方向的反方向**和**法线方向**以及**当前传播介质和目标传播介质的折射率的比值**求得视方向在目标介质中的传播方向
	- ![[OpenGL_环境映射_折射.png]]

#### 高级数据
- ###### OpenGL的一些对缓冲的操作
	- 缓冲只是一个管理特定内存块的对象，在把他绑定到一个缓冲目标时，如GL_ARRAY_BUFFER、GL_ELEMENT_ARRAY_BUFFER，他才有意义
	- 在绑定完缓冲进行数据的填充时，可以把**data**设为**NULL**，这样可以预留特定大小的内存，但是在之后才对这个缓冲进行数据填充
	- 可以通过<mark>glBufferSubData</mark>来对一个缓冲的一部分进行数据填充，但在使用<mark>glBufferSubData</mark>之前必须先调用<mark>glBufferData</mark>来为缓冲分配足够的内存
		- <mark>// 范围： [24, 24 + sizeof(data)]<br>glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data);</mark>
	- 或者通过请求缓冲内存的指针，直接将数据复制到缓冲当中。通过调用<mark>glMapBuffer</mark>函数，OpenGL会返回当前绑定缓冲的内存指针。这样可以直接将数据映射到缓冲中而不用实现将他们储存到临时内存中
		- ![[OpenGL_缓冲内存指针.png]]
- ###### 分批顶点属性
	- 在之前的教程里都是将模型的顶点位置、法线方向、UV、顶点色等数据全部紧密的放置在一起，而另一种做法是将每个数据类型分批打包成个数组，**比如位置数据包含了模型所有的顶点的顶点位置**，**法线数据包含了模型所有的顶点的法线方向**
	- 这两种方法细化那个用哪个
		- ![[OpenGL_顶点数据分批1.png]]
		- ![[OpenGL_顶点数据分批2.png]]
- ###### 复制缓冲
	- 填充好数据之后，可以将缓冲里的数据复制给其他的缓冲，或者与其他缓冲共享数据，可以通过<mark>glCopyBufferSubData</mark>来复制缓冲中的数据
		- <mark>void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset, GLintptr writeoffset, GLsizeiptr size);</mark>
		- `readtarget`：被复制数据的缓冲 (源)
		- `writetarget`：把数据复制过去的目标缓冲 (目标)
		- `readoffset`：读取数据的起始位置
		- `writeoffset`：写入数据的起始位置
			- 由于不能同时绑定两个同样类型的缓冲，如果**源和目标是同样的缓冲类型**，可以需要的缓冲对象绑定到<mark>GL_COPY_READ_BUFFER</mark>和<mark>GL_COPY_WRITE_BUFFER</mark>上，并把他们俩设置为源和目标，<mark>glCopyBufferSubData</mark>会从源目标的其实位置读取数据并在目标的起始位置写入数据
			- ![[OpenGL_复制缓冲1.png]]
		- 也可以只将`writetarget`缓冲绑定为新的缓冲目标类型
			- ![[OpenGL_复制缓冲2.png]]

#### 高级GLSL
- ###### GLSL的内建变量(Built-in Variable)
	- 除了顶点属性，uniform，和sampler以外，还可以通过前缀为**gl_** 的变量来读取/写入着色器之外的数据
	- 最常见的有一直在用的**gl_Position**和在可视化场景深度时用的**gl_FragCoord**
	- [全部的OpenGL内建变量](https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL))
- ###### 顶点着色器变量
	- **gl_Position**：输出变量，裁剪空间的顶点位置向量，想要在屏幕上输出颜色必须的步骤
	- **gl_PointSize**：输出变量，当图元渲染模式设为<mark>GL_POINTS</mark>时，可以在主程序中使用<mark>glPointSize</mark>来设置点的大小，也可以在顶点着色器中通过<mark>gl_PointSize</mark>来设置
		- 与在主程序中设置的区别是，在顶点着色器中可以对每个点设置不同的值
		- 在顶点着色器中修改点图元大小的功能默认是禁用的，需要使用<mark>GL_PROGRAM_POINT_SIZE</mark>来启用
			- <mark>glEnable(GL_PROGRAM_POINT_SIZE);</mark>
		- 使用例：点离相机越远就越大<br>![[OpenGL_gl_PointSize.png]]
	- **gl_VertexID**：输入变量，储存了正在绘制的顶点的ID
		- 使用<mark>glDrawElements</mark>进行索引渲染时，这个变量会储存正在绘制顶点的当前索引
		- 使用<mark>glDrawArrays</mark>进行渲染是，这个变量会储存从渲染调用开始的已处理的顶点数量
- ###### 片段着色器变量
	- **gl_FragCoord**：z分量是当前片段的深度值，x和y分量是当前片段的**窗口空间(Window-Space)** 坐标，原点在窗口的左下角。x的范围是\[0，screen width]，y的范围是\[0，screen height]
		- 根据片段的窗口x位置输出不同的颜色<br>![[OpenGL_gl_FragCoord.png]]
	- **gl_FrontFacing**：bool变量，如果当前片段是正向面则返回`True`，背向面则返回`False`
		- 给正向面和背向面上不同的材质<br>![[OpenGL_gl_FrontFacing.png]]
		- *开了面剔除就看不到被剔除的面了*
	- **gl_FragDepth**：修改当前片段的深度值，范围是\[0, 1]，这个值会覆盖掉gl_FragCoord.z的深度值，如果着色器没有对gl_FragDepth写入值，那么就会自动用gl_FracCoord.z的值
		- 写入gl_FragDepth之后将**禁用所有提前深度测试**
		- **从OpenGL4.2起**， 可以通过在**片段着色器顶部**使用深度条件(Depth Condition)来重新声明gl_FragDepth变量
			- ![[OpenGL_gl_FragDepth.png]]

| 条件 | 描述                                   |
| ---------------------- | -------------------------------------- |
| any                    | 默认值，提前深度测试被禁用             |
| greater                | 手动输入的深度值只能比gl_FragCoord.z大 |
| less                   | 手动输入的深度值只能比gl_FragCoord.z小 |
| unchanged              | 只能将gl_FragCoord.z的值写入gl_FragDepth                                     |

- ###### 接口块
	- 用于在**顶点着色器**和**片段着色器**之间传递数据
	- <mark>out</mark>定义了输出数组，<mark>in</mark>定义了输入数组
	- 片段着色器中的**块名**需要和顶点着色器中的一样(<mark>out/in</mark> 后面跟着的名字)，而整个数组后面的实例名可以是随意的
	- 顶点着色器<br>![[OpenGL_接口快Vert.png]]
	- 片段着色器<br>![[OpenGL_接口快Frag.png]]
- ###### Uniform缓冲对象
	- 用于定义和储存在多个不同的着色器中都相同的**全局Uniform变量**，如V矩阵和P矩阵
	- **使用Uniform缓冲对象代替独立的uniform的好处是**
		- 一次设置很多uniform并让他们在所有着色器中都有用比一个一个设置多个uniform要快很多
		- 比起在多个着色器中重复修改相同的uniform变量，用uniform缓冲修改一次并对所有着色器都起效更容易
		- OpenGL限制了它能够处理的uniform数量，当使用Uniform缓冲对象时，由于每个uniform缓冲对象都包含了很多不同的变量，能够设置的uniform变量总数会变多
	- 只需要在主程序里设置一次相关的Uniform，但是还是手动在每个着色器中设置不同的Uniform
	- Uniform缓冲对象是一个缓冲，所以通过<mark>glGenBuffers</mark>来创建，将他绑定到<mark>GL_UNIFORM_BUFFER</mark>缓冲目标，并将所有的uniform数据存入缓冲
		- ![[OpenGL_创建Uniform块缓冲.png]]
		- OpenGL中定义了一些绑定点(Binding Point)，可以将一个Uniform缓冲链接至绑定点，并把着色器里的Uniform块绑定到相同的绑定点，实现数据的互通
		- ![[OpenGL_绑定点.png]]
		- 一个绑定点可以被**多个**着色器里的Uniform块绑定，前提是这些Uniform块的定义是相同的。但是每个绑定点同时只能绑定**一个**Uniform缓冲对象
		- 先用<mark>glGetUniformBlockIndex</mark>来获取指定着色器中Uniform块的位置值索引，再用<mark>glUniformBlockBinding</mark>来将**着色器中的Uniform块**绑定到绑定点
			- 将着色器中的`Lights`Uniform块绑定到绑定点**2**<br>![[OpenGL_绑定到绑定点.png]]
			- *需要对每个着色器都重复这个步骤*
			- *在OpenGL4.2和之后 的版本，可以在定义Uniform块的时候添加布局标识符，来直接绑定该Uniform块到指定绑定点：
				- <mark>layout(std140, binding = 2) uniform Lights { ... };</mark>
		- 之后还需要绑定Uniform缓冲对象到相同的绑定点上，有两种方法
			- <mark>glBindBufferBase(GL_UNIFORM_BUFFER, 目标绑定点， Uniform缓冲对象)</mark>
			- <mark>glBindBufferRange(GL_UNIFORM_BUFFER，目标绑定点，Uniform缓冲对象，偏移量，要绑定的变量的大小)</mark>
				- 这个函数可以把一个Uniform块里的一部分变量绑定到绑定点中
				- 这个函数可以把多个不同的Uniform块绑定到同一个Uniform缓冲对象上
		- 最后是把数据添加到Uniform缓冲中
			- ![[OpenGL_Uniform缓冲添加数据.png]]
			- *144是该数据的对齐偏移量*
	- 在着色器里访问uniform块里的数据时，不需要加块名做前缀，直接访问就行
	- 顶点着色器中的Uniform块<br>![[OpenGL_顶点着色器Uniform块.png]]
		- <mark>layout (std140)</mark>设置了**Uniform块布局(Uniform Block Layout)**，意味着当前定义的Uniform块对他的内容使用一个特定的内存布局
- ###### Uniform块布局(Uniform Block Layout)
	- Uniform块只是一块预留的内存，需要告诉OpenGL内存的哪个部分对应着着色器中的哪个Uniform变量
	- ![[OpenGL_Uniform块例子.png]]
		- 每个变量都有一个**基准对齐量(Base Alignment)**，他等于一个变量在Uniform块中所占据的空间(包括填充量)。然后对每个变量在计算他的**对齐偏移量(Aligned Offset)**，他是一个变量从块起始位置开始的字节偏移量。一个变量的对齐偏移量**必须**等于基准对齐量的**倍数**
		- 前一个变量的基准对齐量+对齐偏移量等于当前变量的对齐偏移量。如果当前变量的对齐偏移量是16而前一个变量相加完的结果是4，那么当前变量的对齐偏移量必须是16，因为对齐偏移量必须是基准偏移量的倍数
		- 整个Uniform块的大小是最后一个变量的基准对齐量+他的对齐偏移量，在ExampleBlock中这个大小是152 (4 + 148)

| 类型            | 布局规则                                           |
| --------------- | -------------------------------------------------- |
| 标量(int, bool) | 每个标量的基准对齐量为N                            |
| 向量            | 2N或4N。**vec3的基准对齐量是4N**                   |
| 标量/向量的数组 | 每个元素的基准对齐量与vec4的相同                   |
| 矩阵            | 储存为列向量的数据，每个向量的基准对齐量与vec4相同 |
| 结构体          | 等于所有元素根据规则计算后的大小，但会填充到vec4大小的倍数                                                   |
[OpenGL的Uniform缓冲规范](https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt)

- ###### Uniform缓冲的设置顺序
	- 创建并绑定Uniform块到绑定点
	- 创建并绑定Uniform缓冲对象到绑定点
	- 在合适的位置填充Uniform缓冲数据

#### 几何着色器(Geometry Shader)
- 几何着色器在顶点和片段着色器之间，**输入是一个图元(点、线、三角形)的所有顶点**
- 几何着色器可以把这一组顶点变换为完全不同的图元，还可以生成比原来更多的顶点
- ###### 几何着色器的例子
	- ![[OpenGL_几何着色器例子.png]]
	- <mark>layout (points) in;</mark>：声明从顶点着色器输入的图元类型
		- `points`：GL_POINTS (1)
		- `lines`：GL_LINES / GL_LINE_STRIP (2)
		- `line_adjacency`：GL_LINES_ADJACENCY / GL_LINE_STRIP_ADJACENCY (4)
		- `triangles`：GL_TRIANGLES / GL_TRIANGLES_STRIP / GL_TRIANGLE_FAN (3)
		- `triangles_adjacency`：GL_TRIANGLES_ADJACENCY / GL_TRIANGLES_STRIP_ADJACENCY (6)
		- *括号里的数字是 一个图元所包含的最小顶点数*
	- <mark>layout (line_strip, max_vertices = 2) out;</mark> ：指定了机核着色器输出的图元类型，和他最大能够输出的顶点数量(如果超过了这个值，OpenGL将不会绘制多出的顶点)
		- `points`
		- `line_strip`
		- `triangle_strip`
	- 几何着色器还需要顶点着色器阶段的输出，该输出是一个接口块，包含了几个变量。**这个接口块是内建变量，不需要创建**
		- ![[OpenGL_几何着色器接口块.png]]
	- 之后在**main()** 里使用两个函数来生成新的数据
		- ![[OpenGL_几何着色器生成图元.png]]
		- <mark>EmitVertex</mark>：调用时会将当前<mark>gl_Position</mark>中的向量添加到图元中
		- <mark>EndPrimitive</mark>：调用时所有发射出的(Emitted)顶点都会合成为指定的输出渲染图元。重复调用<mark>EndPrimitive</mark>可以生成多个图元
		- <mark>gl_in[0]</mark>：0表示的当前顶点在当前图元中的索引。因为几何着色器的输入是一个图元的所有顶点，所以会有多个顶点，通过括号里的数字来索引顶点
- ###### 几何着色器的一些注意事项
	-  **几何着色器接受的位置向量是观察空间的坐标**，如果需要在几何着色器中涉及到法线的操作，需要在顶点着色器中把顶点位置和法线变换到观察空间(只乘M和V矩阵)，然后再在几何着色器中乘P矩阵
	- <mark>layout (points) in</mark>里的图元选项是在**主程序**里的DrawCall设置的，而<mark>layout (line_strip, max_vertices = 2) out</mark>里的图元选项决定了整个着色器最后的输出的是什么图元
	- [TA百人的几何着色器和曲面细分](https://zhuanlan.zhihu.com/p/479792793)

#### 实例化(Instancing)
- 用于绘制大量相同模型的场景(模型相同，这是进行了不同的世界空间变换)，如草、植被、粒子。**基本上只要场景中有很多重复的形状，都能使用实例化渲染来提高性能**
- 一次性把模型数据发给GPU，然后通过<mark>glDrawArraysInstanced</mark>或者<mark>glDrawElementInstanced</mark>来让GPU进行实例化渲染，GPU会直接渲染这些实例，而不用不断的和CPU进行通信
	- 这两个函数相较于原版，需要一个额外的叫做**实例数量**的额外参数
- 使用实例化渲染的时候，顶点着色器里有一个叫做<mark>gl_InstanceID</mark>的内建变量，在实例化渲染调用时，<mark>gl_InstanceID</mark>会从0开始计数，在每个实例被渲染的时候加1
- ###### 实例化数组
	- 使用**实例化数组(Instanced Array)** 来避免在渲染非常大量的实例化渲染的时候超过能发给着色器的Uniform的上限
	- 实例化数组被定义为一个**顶点属性**，只有在顶点着色器渲染一个新的实例时才会更新
	- 通过<mark>glVertexAttribDivisor(2, 1)</mark>来设置什么时候该更新顶点属性的内容至下一组数据
		- 第一个参数是需要更新的顶点属性的**location**
		- 第二个参数是属性除数(Attribute Divisor)
			- `0`：每次顶点着色器运行的时候更新
			- `1`：每次渲染新一个实例的时候更新
			- `2`：每两次渲染一个新实例的时候更新
			- `n`：每**n**次渲染一个新实例的时候更新
	- 在顶点着色器中使用实例化数组传入矩阵的时候，由于一个顶点属性最大只支持vec4的大小，mat4的矩阵需要在VAO里被拆成4个vec4的向量，每一个向量使用一个顶点属性位置

#### 抗锯齿(Anti-Aliasing)
- [文刀秋二知乎的AA回答(里面有带一嘴为什么MSAA不适合延迟渲染)](请问FXAA、FSAA与MSAA有什么区别？效果和性能上哪个好？ - 文刀秋二的回答 - 知乎 https://www.zhihu.com/question/20236638/answer/44821615)
- 抗锯齿用来缓解形成模型边缘的锯齿，从而产生更加平滑的边缘
- ###### 锯齿的形成
	- 顶点在顶点着色器(几何着色器)中完成处理来到片段着色器之前，会被光栅器光栅化。**光栅化的目的**是吧只是数据的顶点位置映射成屏幕坐标
	- 屏幕的每一个**像素的中心**有一个采样点。如果一个三角形(图元)覆盖到了这个采样点，那么该像素就会生成一个片段。即使三角形(图元)覆盖了一个像素的一部分，但是没有覆盖到采样点，该像素也不会成为这个三角形的一个片段
	- 结果就是，由于屏幕像素总量的限制，图元边缘的一部分像素会被渲染出来，而另一部分则不会，所以导致出现了边缘锯齿
- ###### 多重采样(Multisampling)
	- 多重采样就是把原本每个像素一个采样点，变成每个像素**多个以特定图案排列的采样点**
	- 颜色缓冲的大小会随着子采样点的增加而增加
	- ![[OpenGL_MSAA.png]]
		- *多重采样的采样点是任意的，更多的采样点能带来更精确的覆盖率*
	- **MSAA**的工作方式
		- 无论三角形覆盖了一个像素的多少个子采样点，每个图元中的每个像素都只会运行**一次**片段着色器
		- 传入片段着色器的顶点数据会被插值到像素的**中心**
		- 片段着色器的结果会被储存在该像素所有**被覆盖的**子采样点中，而另外的子采样点的颜色会被其他图元或者背景的颜色所填满。当一个像素的所有子采样点都被颜色填满时，该像素的颜色将会是他的四个采样点的**颜色的平均值**
			- *片段着色器给到边缘的每个像素的颜色是假设他们被完全覆盖的情况下的颜色*
			- MSAA采样点覆盖例子<br>![[OpenGL_MSAA采样点.png]]
			- MSAA结果例子<br>![[OpenGL_MSAA采样结果.png]]
		- 深度/模板测试也会使用多个采样点，深度/模板缓冲的大小也会乘以子采样点的个数
			- 深度测试：每个顶点的深度值会在运行深度测试之前被插值到各个子样本中
			- 模板测试：会对每个子采样点，而不是每个像素，储存一个模板值
		- OpenGL中的MSAA
			- 由于需要给一个像素的每个采样点都储存一个颜色，所以需要一个能在每个像素中储存**大于一个**颜色值的颜色缓冲，叫做**多重采样缓冲(Multisample Buffer)**，他用来在开启MSAA时代替原本的颜色缓冲
			- 通过<mark>glfwWindowHint(GLFW_SAMPLES, 4)</mark>来提示OpenGL要使用子采样点数量为N的多重采样缓冲
			- 通过<mark>glEnable(GL_MULTISAMPLE)</mark>来启用MSAA
				- *大多数OpenGL的驱动上，MSAA都是默认启用的，但是为了保险还是调用一下*
			- 没有MSAA：<br>![[OpenGl_MSAA例子1.png]]
			- 有MSAA：<br>![[OpenGL_MSAA例子2.png]]
- ###### 离屏MSAA(Off-screen MSAA)
	- 如果想使用自己的帧缓冲来进行离屏渲染，而不是使用默认的帧缓冲，那就需要自己动手生成多重采样缓冲
	- 多重采样纹理附件
		- 使用<mark>glTexImage2DMultisample</mark>来代替<mark>glTexImage2D</mark>，来使纹理附件支持储存多个采样点的纹理，他绑定的目标是GL_TEXTURE_2D_MULTISAMPLE
			- <mark>glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex); <br>glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE); <br>glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);</mark>
				- `samples`：纹理所拥有的的样本个数
				- `GL_TRUE`：如果设置为`True`，图像会为每个纹素使用相同的样本位置以及相同数量的子采样点个数
		- 使用<mark>glFramebufferTexture2D</mark>将多重采样纹理附加到帧缓冲上，但是纹理类型使用<mark>GL_TEXTURE_2D_MULTISAMPLE</mark>
			- <mark>glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);</mark>
	- 多重采样渲染缓冲对象
	- 只需要再指定(当前绑定的)渲染缓冲的内存存储时，将<mark>glRenderbufferStorage</mark>的调用改为<mark>glRenderbufferStorageMultisample</mark>
		- <mark>glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);</mark>
			- `4`：子采样点的数量
	- 渲染到多重采样帧缓冲
		- 只要在帧缓冲绑定时绘制任何东西，光栅器就会负责所有的多重采样运算，但是不能将运算过后的缓冲图像用于其他运算，因为多重采样的图像比普通图像包含更多的信息，所以需要**缩小/还原(Resolve)** 图像。
		- 还原的工作通常通过<mark>glBlitFramebuffer</mark>来完成，它能够将一个帧缓冲中的某个区域复制到另一个缓冲中，并且将多重采样缓冲还原
			- 处理的思路是，将帧缓冲的读取和写入分别绑定给不同的缓冲对象 (读取绑定到自定义的帧缓冲，也就是储存MSAA结果的缓冲；写入绑定到默认缓冲)，然后使用<mark>glBlitFramebuffer</mark>来讲图像**位块传送()** 到默认的帧缓冲中，以此将多重采样的帧缓冲传送到屏幕上
			- ![[OpenGL_Blit.png]]
			- 如果要对经过MSAA的图像进行后处理，需要先把MSAA的结果发送给另一个帧缓冲，在该帧缓冲中对图像进行后处理，然后再发送给默认缓冲来显示到屏幕上