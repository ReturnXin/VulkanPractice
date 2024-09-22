#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <map>

#include <tool/shader.h>
#include <tool/camera.h>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

#include <tool/gui.h>
#include <tool/mesh.h>
#include <tool/model.h>

#include <random>

using namespace std;

const int SCREEN_WIDTH = 720, SCREEN_HEIGHT = 720;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 25.0f);
Camera camera(cameraPos);

float deltaTime = 0.0f, lastFrame = 0.0f;

float lastX = SCREEN_WIDTH / 2, lastY = SCREEN_HEIGHT / 2;
float yaw, pitch;
float fov = 45.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void resize(GLFWwindow *window, int width, int height);
unsigned int loadTexture(char const *path);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void drawMesh(BufferGeometry geometry);
void renderSphere();

std::string Shader::dirName;
int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PBR", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetFramebufferSizeCallback(window, resize);

    // positions
    vector<glm::vec3> lightPositions{
        glm::vec3(-10.0f, 10.0f, 10.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    // colors
    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
    lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
    lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
    lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
    SphereGeometry objectGeometry(1.0, 64.0, 64.0);      // 圆球
    SphereGeometry pointLightGeometry(0.17, 64.0, 64.0); // 点光源位置显示

    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    Shader shader("./shader/object_vertex.glsl", "./shader/object_fragment.glsl");
    Shader lightShader("./shader/light_vertex.glsl", "./shader/light_fragment.glsl");

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
    shader.setFloat("ao", 1.0f);

    lightShader.use();
    lightShader.setMat4("projection", projection);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = camera.GetViewMatrix();

        // 绘制物体
        shader.use();
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setVec3("viewPos", camera.Position);

        float radius = 10.0f;
        glm::vec3 newPos = lightPositions[0] + glm::vec3(sin(glfwGetTime()) * radius, cos(glfwGetTime()) * radius, 0.0);
        shader.setVec3("lightPositions[" + std::to_string(0) + "]", newPos);
        shader.setVec3("lightColors[" + std::to_string(0) + "]", lightColors[0]);

        for (int i = 1; i < lightPositions.size(); i++)
        {
            newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; row++)
        {
            shader.setFloat("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; col++)
            {
                shader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3((col - (nrColumns / 2)) * spacing,
                                                        (row - (nrRows / 2)) * spacing,
                                                        0.0f));
                shader.setMat4("model", model);
                // renderSphere();
                drawMesh(objectGeometry);
            }
        }

        // 绘制光源
        lightShader.use();
        lightShader.setMat4("view", view);
        newPos = lightPositions[0] + glm::vec3(sin(glfwGetTime()) * radius, cos(glfwGetTime()) * radius, 0.0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", lightColors[0]);
        renderSphere();
        for (int i = 1; i < lightPositions.size(); i++)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", lightColors[i]);
            renderSphere();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
void resize(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
unsigned int loadTexture(char const *path)
{
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        cout << "Failed to load image resource: " << path << endl;
        stbi_image_free(data);
    }
    return texture;
}
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // 相机按键控制
    // 相机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// 鼠标移动监听
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
// 绘制物体
void drawMesh(BufferGeometry geometry)
{
    glBindVertexArray(geometry.VAO);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}
