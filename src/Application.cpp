#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glew.h>
#include <glfw3.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
struct ShaderProgramSources
{
    std::string VertexSource;
    std::string FragmentSource;
};
static ShaderProgramSources ParseShader(const std::string& filePath)
{
    std::ifstream stream (filePath);
    if(!stream.is_open())
        std::cout << "Failed to open: " << filePath << std::endl;
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line))
    {
        if(line.find("#shader") != std::string::npos )
        {
            if(line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if(line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n' ;
        }
    }
    return {ss[0].str(), ss[1].str()};

}
static unsigned int CompileShader(unsigned int type,const std::string& source )
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src ,nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader! " << 
                    (type == GL_VERTEX_SHADER ? "vertex ": "fragment")
                    << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}
static unsigned int CreateShader(const std::string& vertexShader , const std::string& fragmentShader)
{
    unsigned int program = 0;
    GLCall(program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER , vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER , fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //删掉了旧的配置，采用core profile

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);    //VSync垂直同步

    if (glewInit() != GLEW_OK)
    {
        std::cout << "ERROR" << std::endl;
    }
    
    float position [] = {
        -0.5f,-0.5f,
         0.5f,-0.5f,
         0.5f, 0.5f,
        -0.5f, 0.5f
    };

    unsigned int indices[]
    {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;       //定义vao vertex array object去存储vbo的信息
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    VertexBuffer vb(position, 4* 2 *sizeof(float));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT ,GL_FALSE, sizeof(float) * 2, 0));


    IndexBuffer ib(indices, 6);



    ShaderProgramSources source = ParseShader("res/shaders/Basic.shader");
    std::cout << "Vertex" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    int location = glGetUniformLocation(shader, "u_Color");
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));


    GLCall(glBindVertexArray(0));   //取消绑定vao 下面的vbo处理就与原来的vao无关了，vao已经记录下来了，但是vao不存储信息，他只会记录配置
    GLCall(glUseProgram(0));        //下列处理与vao无关
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));


    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
//        glDrawArrays(GL_TRIANGLES, 0, 6);


        GLCall(glUseProgram(shader));          //绑定之后就可以设置uniform了
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f)); //uniform是整体的，不能让同一时间画出来的正方形一半是一种颜色另一半是另外一种
        
        GLCall(glBindVertexArray(vao)); //每次需要画图就调用vao的配置信息来进行画图
        ib.Bind();

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if(r > 1.0f)
        {
            increment = -0.05f;
        }
        else if(r <= 0.0f)
        {
            increment = 0.05f;
        }
        
        r += increment;
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // glDeleteProgram(shader);

    glfwTerminate();    //glfwTerminate会直接影响我们程序的关闭，因为类的析构发生在作用域之外
    return 0;
}
