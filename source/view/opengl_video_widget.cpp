#include "view/opengl_video_widget.hpp"

// 顶点着色器 - 处理每个顶点的位置和纹理坐标
const char* v_string = R"(
    /// @brief 输入顶点位置 (x, y, z, w)
    /// @note attribute关键字用于声明顶点属性，这些数据来自CPU端传入的顶点数据
    /// @note vec4表示4维向量，通常用于表示3D空间中的位置坐标
    attribute vec4 vertexIn;
    
    /// @brief 输入纹理坐标 (u, v)
    /// @note 纹理坐标用于确定从纹理图像中采样哪个像素
    /// @note vec2表示2维向量，u和v坐标范围通常在[0,1]之间
    attribute vec2 textureIn;
    
    /// @brief 输出纹理坐标到片段着色器
    /// @note varying关键字用于声明一个变量，它的值在顶点着色器中计算，在片段着色器中插值后使用
    /// @note 用于传递从顶点着色器到片段着色器的数据，GPU会自动在顶点之间进行插值
    /// @note vec2 是 GLSL 中的一个类型，用于表示二维向量，由两个 float 组成
    varying vec2 textureOut;
    
    void main(void)
    {
        /// @brief 设置顶点在裁剪空间中的位置
        /// @note gl_Position是顶点着色器的内置输出变量，表示顶点在裁剪空间中的位置
        /// @note 裁剪空间是GPU进行视锥体裁剪的坐标系
        gl_Position = vertexIn;
        
        /// @brief 将纹理坐标传递到片段着色器
        /// @note 这些坐标将在片段着色器中用于纹理采样
        textureOut = textureIn;
    }
)";

// 片段着色器 - 处理每个像素的颜色计算
const char* t_string = R"(
    /// @brief 从顶点着色器接收的纹理坐标
    /// @note varying关键字声明的变量，在顶点之间进行插值后传递给片段着色器
    /// @note 每个片段都会获得插值后的纹理坐标，用于纹理采样
    varying vec2 textureOut;
    
    /// @brief YUV纹理采样器 - Y分量（亮度）
    /// @note uniform关键字用于声明一个变量，它的值在渲染过程中是常量，且在绘制多个图形时保持不变
    /// @note 通常用于传递不随顶点变化而变化的参数，如纹理、光源位置等
    /// @note 在着色器中，uniform变量的值通常在主机代码（如C++）中设置，并在着色器执行期间保持不变
    /// @note sampler2D是GLSL中的一个类型，用于表示二维纹理采样器
    /// @note 它允许在着色器中访问和使用二维纹理数据，实际上是一个句柄，用于引用在主机代码中绑定到该uniform变量的具体纹理
    uniform sampler2D texture_y;
    
    /// @brief YUV纹理采样器 - U分量（色度）
    /// @note U分量表示蓝色色度信息，范围通常在[-0.5, 0.5]之间
    uniform sampler2D texture_u;
    
    /// @brief YUV纹理采样器 - V分量（色度）
    /// @note V分量表示红色色度信息，范围通常在[-0.5, 0.5]之间
    uniform sampler2D texture_v;

    void main(void)
    {
        /// @brief YUV颜色空间的三分量
        /// @note YUV是一种颜色编码方法，Y表示亮度，U和V表示色度
        /// @note 这种格式常用于视频压缩，因为人眼对亮度更敏感
        vec3 yuv;
        
        /// @brief RGB颜色空间的三分量
        /// @note RGB是标准的颜色表示方法，R(红)、G(绿)、B(蓝)
        vec3 rgb;
        
        /// @brief 从Y纹理中采样亮度值
        /// @note texture2D函数用于从纹理中采样颜色值
        /// @note .r表示取红色分量，在Y纹理中红色分量存储的是Y值
        yuv.x = texture2D(texture_y, textureOut).r;
        
        /// @brief 从U纹理中采样蓝色色度值
        /// @note .r表示取红色分量，在U纹理中红色分量存储的是U值
        /// @note -0.5是为了将U分量从[0,1]范围转换到[-0.5,0.5]范围
        yuv.y = texture2D(texture_u, textureOut).r - 0.5;
        
        /// @brief 从V纹理中采样红色色度值
        /// @note .r表示取红色分量，在V纹理中红色分量存储的是V值
        /// @note -0.5是为了将V分量从[0,1]范围转换到[-0.5,0.5]范围
        yuv.z = texture2D(texture_v, textureOut).r - 0.5;
        
        /// @brief YUV到RGB的颜色空间转换矩阵
        /// @note 这是一个3x3矩阵，用于将YUV颜色空间转换为RGB颜色空间
        /// @note 矩阵系数基于BT.601标准，适用于标准清晰度视频
        /// @note 矩阵乘法将YUV向量转换为RGB向量
        /// @note 转换公式：
        /// @note R = Y + 1.13983 * V
        /// @note G = Y - 0.39465 * U - 0.58060 * V  
        /// @note B = Y + 2.03211 * U
        rgb = mat3(1.0, 1.0, 1.0,      // Y分量系数
            0.0, -0.39465, 2.03211,     // U分量系数
            1.13983, -0.58060, 0.0) * yuv; // V分量系数
        
        /// @brief 设置片段的最终颜色
        /// @note gl_FragColor是片段着色器的内置输出变量，表示片段的最终颜色
        /// @note vec4(rgb, 1.0)将RGB值转换为RGBA，其中A(alpha)设为1.0表示完全不透明
        gl_FragColor = vec4(rgb, 1.0);
    }
)";

OpenGLVideoWidget::OpenGLVideoWidget(QWidget* parent) :QOpenGLWidget(parent)
{
}
OpenGLVideoWidget::~OpenGLVideoWidget() {}

void OpenGLVideoWidget::initializeGL()
{
    DANEJOE_LOG_TRACE("default", "QtOpenGL", "initializeGL");
    /// @brief 初始化OpenGL函数(QOpenGLFunctions继承)
    initializeOpenGLFunctions();

    qDebug() << "======begin======";
    /// @brief m_program加载shader(顶点和片元)脚本
    /// @note 片元(像素)着色器
    qDebug() << m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, t_string);
    /// @note 顶点着色器
    qDebug() << m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, v_string);

    /// @brief 链接着色器程序
    if (!m_program.link())
    {
        qDebug() << "Failed to link shader program:" << m_program.log();
    }
    else
    {
        qDebug() << "Shader program linked successfully";
    }
    qDebug() << "=======end======";
}

void OpenGLVideoWidget::paintGL()
{
    DANEJOE_LOG_TRACE("default", "QtOpenGL", "paintGL");
}

void OpenGLVideoWidget::resizeGL(int w, int h)
{
    DANEJOE_LOG_TRACE("default", "QtOpenGL", "resizeGL: width{} height{}", w, h);
}