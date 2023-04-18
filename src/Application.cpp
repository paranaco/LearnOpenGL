#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream> 
#include<string>
#include<sstream>

//定义结构体存储着色器
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;

};


static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE=-1,VERTEX=0,FRAGMENT=1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("shader") != std::string::npos)
        {  
            
            if (line.find("vertex") != std::string::npos)
                //设置为顶点着色器模式 
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
            //设置为片元着色器模式
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}

//抽象进函数中
static unsigned int CompileShader(unsigned int type,const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    //指定着色器
    glShaderSource(id,1,&src,nullptr);
    //指定编译对象
    glCompileShader(id);


    int result;
    //错误处理
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << (type == GL_VERTEX_SHADER ? "vertex":"fragment")<< "着色器编译失败！" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

//链接到独立的着色器程序，返回int
static unsigned int CreatShader(const std::string& verxShader, const std::string& fragmentShader)
{
    //获取返回值
    unsigned int program = glCreateProgram();
    //创建顶点着色器对象
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,verxShader);
    //创建片元着色器对象
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //附加着色器到程序
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    //连接程序
    glLinkProgram(program);
    //验证程序
    glValidateProgram(program);

    //
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
 
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;




    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;


    //定义顶点缓冲区数组
    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f, 
         0.5f,  0.5f,
        -0.5f,  0.5f,

    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };

    //定义无符号变量存储缓冲区ID
    unsigned int bufferId;
    //生成缓冲区对象,参数一为数量参数二为缓冲区ID地址 
    glGenBuffers(1,&bufferId);
    //绑定缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    //指定数据给缓冲区,参数一为顶点缓冲，参数二为指定缓冲区存储字节大小，参数三为data指针
    glBufferData(GL_ARRAY_BUFFER, 6 *2*sizeof(float), positions,GL_STATIC_DRAW);


    //索引缓冲区 
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 *  sizeof(unsigned int), indices, GL_STATIC_DRAW);

    //启用顶点属性数组,索引为0
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    //读取着色器文件
    ShaderProgramSource source = ParseShader("res/shader/Basic.shader");
    std::cout << "Vertex" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment" << std::endl;
    std::cout << source.FragmentSource << std::endl;
    unsigned int shader = CreatShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);
   
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}