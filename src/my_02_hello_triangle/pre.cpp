// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <iostream>

// using namespace std;

// const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
// const char *vertexShaderSource =
//     "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "void main()\n"
//     "{\n"
//     "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);"
//     "gl_PointSize = 20.0f;"
//     "}\n";
// const char *fragmentShaderSource =
//     "#version 330 core\n"
//     "out vec4 FragColor;\n"
//     "void main()\n"
//     "{"
//     "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
//     "}";

// void resize(GLFWwindow *window, int width, int height);
// void processInput(GLFWwindow *window);

// int main(void)
// {
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "hello_window", NULL, NULL);
//     if (window == NULL)
//     {
//         cout << "Failed to create GLFW window" << endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);

//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         cout << "Failed to initialize GLAD" << endl;
//     }

//     glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

//     glfwSetFramebufferSizeCallback(window, resize);

//     float vertices[] = {
//         -0.5f, -0.5f, 0.0f,
//         0.5f, -0.5f, 0.0f,
//         0.0f, 0.5f, 0.0f};

//     unsigned int VBO, VAO;
//     glGenBuffers(1, &VBO);
//     glGenVertexArrays(1, &VAO);
//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);

//     int success;
//     char infoLog[512];
//     unsigned int vertexShader;
//     vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//     glCompileShader(vertexShader);
//     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//     if (!success)
//     {
//         glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//         cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
//              << infoLog << endl;
//     }
//     unsigned int fragmentShader;
//     fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//     glCompileShader(fragmentShader);
//     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//     if (!success)
//     {
//         glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//         cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
//              << infoLog << endl;
//     }
//     unsigned int shaderProgram;
//     shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);
//     glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//     if (!success)
//     {
//         glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//         cout << "ERROR:SHADER_PROGRAM_LINK_FAILED\n"
//              << infoLog << endl;
//     }
//     glUseProgram(shaderProgram);
//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);

//     while (!glfwWindowShouldClose(window))
//     {
//         processInput(window);

//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         glUseProgram(shaderProgram);
//         glBindVertexArray(VAO);
//         glDrawArrays(GL_TRIANGLES, 0, 3);

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glfwTerminate();
//     return 0;
// }
// void resize(GLFWwindow *window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }
// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }