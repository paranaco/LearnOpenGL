#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream> 
#include<string>
#include<sstream>

//����ṹ��洢��ɫ��
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
                //����Ϊ������ɫ��ģʽ 
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
            //����ΪƬԪ��ɫ��ģʽ
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}

//�����������
static unsigned int CompileShader(unsigned int type,const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    //ָ����ɫ��
    glShaderSource(id,1,&src,nullptr);
    //ָ���������
    glCompileShader(id);


    int result;
    //������
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << (type == GL_VERTEX_SHADER ? "vertex":"fragment")<< "��ɫ������ʧ�ܣ�" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

//���ӵ���������ɫ�����򣬷���int
static unsigned int CreatShader(const std::string& verxShader, const std::string& fragmentShader)
{
    //��ȡ����ֵ
    unsigned int program = glCreateProgram();
    //����������ɫ������
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,verxShader);
    //����ƬԪ��ɫ������
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //������ɫ��������
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    //���ӳ���
    glLinkProgram(program);
    //��֤����
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


    //���嶥�㻺��������
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

    //�����޷��ű����洢������ID
    unsigned int bufferId;
    //���ɻ���������,����һΪ����������Ϊ������ID��ַ 
    glGenBuffers(1,&bufferId);
    //�󶨻�����
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    //ָ�����ݸ�������,����һΪ���㻺�壬������Ϊָ���������洢�ֽڴ�С��������Ϊdataָ��
    glBufferData(GL_ARRAY_BUFFER, 6 *2*sizeof(float), positions,GL_STATIC_DRAW);


    //���������� 
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 *  sizeof(unsigned int), indices, GL_STATIC_DRAW);

    //���ö�����������,����Ϊ0
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    //��ȡ��ɫ���ļ�
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