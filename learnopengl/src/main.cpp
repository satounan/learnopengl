#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <malloc.h>
#include <sstream>
#include <string>
#include "Renderer.h"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

namespace{
    void processInput(GLFWwindow *window)
    {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    }

    ShaderProgramSource ParseShader(const std::string& filepath)
    {
        std::ifstream stream(filepath);
        enum class ShaderType
        {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        while (std::getline(stream, line)) 
        {
            if (line.find("#shader") != std::string::npos) 
            {
                if (line.find("vertex") != std::string::npos) 
                {
                    type = ShaderType::VERTEX;
                } 
                else if (line.find("fragment") != std::string::npos) 
                {
                    type = ShaderType::FRAGMENT;
                } 
            } 
            else
            {
                ss[(int)type] << line << '\n';
            }
        }
        return {ss[0].str(), ss[1].str()};
    }

    unsigned int CompileShader(unsigned int type, const std::string& source)
    {
        unsigned int id = glCreateShader(type);
        const char * src = source.c_str();
        GLCall(glShaderSource(id, 1, &src, nullptr));
        GLCall(glCompileShader(id));

        int result;
        GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE) {
            int length;
            GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            char* message = (char*)alloca(length * sizeof(char));
            GLCall(glGetShaderInfoLog(id, length, &length, message));
            std::cerr << "Failed to compile : " << (type == GL_VERTEX_SHADER ? "vertex" : "frament") << std::endl;
            std::cerr << message << std::endl;
            GLCall(glDeleteShader(id));
            return 0;
        }
        return id;
    }
}
int main(){
    // 初始化GLFW库
    glfwInit();
    // 设置OpenGL版本为3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 设置使用OpenGL核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建一个800x600像素的窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "hellotriangle", nullptr, nullptr);
    // 如果窗口创建失败，输出错误信息并退出程序
    if(window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 将窗口的上下文设置为当前线程的上下文
    glfwMakeContextCurrent(window);
    // 设置帧缓冲区大小回调函数
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int, int)->void { GLCall(glViewport(0,0,800,600));});

    // 初始化GLAD库，如果失败，输出错误信息并退出程序
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 创建Shader对象
    Shader ourShader("../shaders/3-3.vs","../shaders/3-3.fs");

    // 定义顶点数据
    float vertices[] = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };

    // 定义索引数据
    unsigned int indices[] = 
    {
        0, 1, 3,
        1,2,3
        // 1, 2, 3
    };

    // 生成VAO、VBO和EBO对象
    unsigned int VAO,VBO,EBO;
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &EBO));

    // 绑定VAO对象
    GLCall(glBindVertexArray(VAO));

    // 绑定VBO对象，并将顶点数据复制到缓冲区中
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // 绑定EBO对象，并将索引数据复制到缓冲区中
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // 配置顶点属性指针
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
    GLCall(glEnableVertexAttribArray(0));
    
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 *sizeof(float))));
    GLCall(glEnableVertexAttribArray(1));
    
    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 *sizeof(float))));//这个6是为什么呢//偏移量，从第六个开始
    GLCall(glEnableVertexAttribArray(2));
    
    // 生成纹理对象
    unsigned int texture1, texture2;//纹理id
    GLCall(glGenTextures(1, &texture1));//纹理对象id

    // 绑定纹理对象到纹理单元0
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, texture1));//绑定id和纹理对象

    // 设置纹理参数
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));//设置样式环绕(wrap)方式,repeat是重复
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));//设置纹理过滤(filter),(liner)线性过滤糊但是色去平均,nearset即临近过滤是什么就取什么,有像素感;

    // 加载纹理图像
    int width, height, nrChannels;
    // unsigned char* data = stbi_load("../asset/container.jpg", &width, &height, &nrChannels, 0);

    unsigned char* data =stbi_load(std::filesystem::path("../asset/container.jpg").string().c_str(), &width, &height, &nrChannels, 0);
    if(data)
    {
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));//生成纹理
    }
    else 
    {
        std::cerr << "Failed to load texture: container" << std::endl;
    }

    // 释放图像内存
    stbi_image_free(data);

    // 生成第二个纹理对象
    GLCall(glGenTextures(1, &texture2));//纹理对象id

    // 绑定纹理对象到纹理单元1
    GLCall(glBindTexture(GL_TEXTURE_2D, texture2));//绑定id和纹理对象

    // 设置纹理参数
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));//设置样式环绕(wrap)方式,repeat是重复
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));//设置纹理过滤(filter),(liner)线性过滤糊但是色去平均,nearset即临近过滤是什么就取什么,有像素感;

    // 加载第二个纹理图像
    data = stbi_load(std::filesystem::path("../asset/awesomeface.png").string().c_str(), &width, &height, &nrChannels, 0);
    if(data)
    {
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));//生成纹理
    }
    else 
    {
        std::cerr << "Failed to load texture: awesomeface " << std::endl;
    }

    // 释放图像内存
    stbi_image_free(data);

    // 使用Shader对象
    ourShader.use();
    GLCall(glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0));
    ourShader.setInt("texture2", 1);

    // 主循环
    while (!glfwWindowShouldClose(window)) 
    {
        // 处理输入
        processInput(window);

        // 清除颜色缓冲区
        GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        // 绑定纹理对象
        GLCall(glActiveTexture(GL_TEXTURE0));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
        GLCall(glActiveTexture(GL_TEXTURE1));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2));

        // 使用Shader对象
        ourShader.use();

        // 绑定VAO对象并绘制三角形
        GLCall(glBindVertexArray(VAO));
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        // 交换缓冲区并处理事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 删除VAO、VBO和EBO对象
    GLCall(glDeleteVertexArrays(1, &VAO));
    GLCall(glDeleteBuffers(1, &VBO));
    GLCall(glDeleteBuffers(1, &EBO));
    // GLCall(glDeleteProgram(ourShader.ID));

    // 终止GLFW库
    glfwTerminate();

    // 输出消息
    std::cout << "Hello, from hellotriangle!\n";

    // 返回0
    return 0;
}

