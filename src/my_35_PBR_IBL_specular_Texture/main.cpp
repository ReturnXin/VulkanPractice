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
unsigned int loadHDRTexture(char const *path);

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
    BoxGeometry boxGeometry(5.0, 5.0, 5.0);
    PlaneGeometry quadGeometry(2.0, 2.0);

    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    int hdrTexture = loadHDRTexture("./static/texture/hdr/newport_loft.hdr");

    // glod
    int goldAlbedoMap = loadTexture("./static/texture/pbr/gold/albedo.png");
    int goldAoMap = loadTexture("./static/texture/pbr/gold/ao.png");
    int goldMetallicMap = loadTexture("./static/texture/pbr/gold/metallic.png");
    int goldNormalMap = loadTexture("./static/texture/pbr/gold/normal.png");
    int goldRoughnessMap = loadTexture("./static/texture/pbr/gold/roughness.png");

    // grass
    int grassAlbedoMap = loadTexture("./static/texture/pbr/grass/albedo.png");
    int grassAoMap = loadTexture("./static/texture/pbr/grass/ao.png");
    int grassMetallicMap = loadTexture("./static/texture/pbr/grass/metallic.png");
    int grassNormalMap = loadTexture("./static/texture/pbr/grass/normal.png");
    int grassRoughnessMap = loadTexture("./static/texture/pbr/grass/roughness.png");

    // plastic
    int plasticAlbedoMap = loadTexture("./static/texture/pbr/plastic/albedo.png");
    int plasticAoMap = loadTexture("./static/texture/pbr/plastic/ao.png");
    int plasticMetallicMap = loadTexture("./static/texture/pbr/plastic/metallic.png");
    int plasticNormalMap = loadTexture("./static/texture/pbr/plastic/normal.png");
    int plasticRoughnessMap = loadTexture("./static/texture/pbr/plastic/roughness.png");

    // rusted_iron
    int ironAlbedoMap = loadTexture("./static/texture/pbr/rusted_iron/albedo.png");
    int ironAoMap = loadTexture("./static/texture/pbr/rusted_iron/ao.png");
    int ironMetallicMap = loadTexture("./static/texture/pbr/rusted_iron/metallic.png");
    int ironNormalMap = loadTexture("./static/texture/pbr/rusted_iron/normal.png");
    int ironRoughnessMap = loadTexture("./static/texture/pbr/rusted_iron/roughness.png");

    // wall
    int wallAlbedoMap = loadTexture("./static/texture/pbr/wall/albedo.png");
    int wallAoMap = loadTexture("./static/texture/pbr/wall/ao.png");
    int wallMetallicMap = loadTexture("./static/texture/pbr/wall/metallic.png");
    int wallNormalMap = loadTexture("./static/texture/pbr/wall/normal.png");
    int wallRoughnessMap = loadTexture("./static/texture/pbr/wall/roughness.png");

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

    // 创建帧缓冲
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // 创建环境立方体贴图
    unsigned int envCubeMap;
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // 创建辐照度贴图
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
                     GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 预滤波HDR环境贴图
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // 按顺序把模糊后的结果存储在mipmap中

    // BRDF 卷积结果
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // shader
    Shader shader("./shader/object_vertex.glsl", "./shader/object_fragment.glsl");
    Shader lightShader("./shader/light_vertex.glsl", "./shader/light_fragment.glsl");
    Shader equiretangularToCubemapShader("./shader/hdr_vertex.glsl", "./shader/hdr_fragment.glsl");
    Shader cubemapShader("./shader/cubeMap_vertex.glsl", "./shader/cubeMap_fragment.glsl");
    Shader irradianceShader("./shader/irradiance_vertex.glsl", "./shader/irradiance_fragment.glsl");
    Shader prefilterShader("./shader/prefilter_vertex.glsl", "./shader/prefilter_fragment.glsl");
    Shader brdfLUTShader("./shader/brdf_vertex.glsl", "./shader/brdf_fragment.glsl");

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
    shader.setFloat("ao", 1.0f);
    shader.setInt("irradianceMap", 0);
    shader.setInt("prefilterMap", 1);
    shader.setInt("brdfLUT", 2);
    shader.setInt("albedoMap", 3);
    shader.setInt("aoMap", 4);
    shader.setInt("metallicMap", 5);
    shader.setInt("normalMap", 6);
    shader.setInt("roughnessMap", 7);

    lightShader.use();
    lightShader.setMat4("projection", projection);

    equiretangularToCubemapShader.use();
    equiretangularToCubemapShader.setMat4("projection", captureProjection);
    equiretangularToCubemapShader.setInt("equiretangularMap", 0);

    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);

    cubemapShader.use();
    cubemapShader.setMat4("projection", projection);
    cubemapShader.setInt("environmentMap", 0);

    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);

    // 绘制环境
    equiretangularToCubemapShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; i++)
    {
        equiretangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubeMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawMesh(boxGeometry);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 绘制辐照度贴图
    irradianceShader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawMesh(boxGeometry);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 在多个mipmap级别上以不同的粗糙度预过滤环境贴图
    prefilterShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; mip++)
    {
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);

        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);

        for (unsigned int i = 0; i < 6; i++)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            drawMesh(boxGeometry);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // BRDF
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    glViewport(0, 0, 512, 512);
    brdfLUTShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMesh(quadGeometry);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);                 // 环境贴图必备
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // 在立方体贴图的面之间进行正确过滤

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
        shader.setMat4("view", view);
        shader.setVec3("viewPos", camera.Position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

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

        // gold
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, goldAlbedoMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, goldAoMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, goldMetallicMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, goldNormalMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, goldRoughnessMap);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
        shader.setMat4("model", model);
        drawMesh(objectGeometry);

        // grass
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, grassAlbedoMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, grassAoMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, grassMetallicMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, grassNormalMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, grassRoughnessMap);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
        shader.setMat4("model", model);
        drawMesh(objectGeometry);

        // plastic
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, plasticAlbedoMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, plasticAoMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, plasticMetallicMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, plasticNormalMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, plasticRoughnessMap);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
        shader.setMat4("model", model);
        drawMesh(objectGeometry);

        // rusted_iron
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ironAlbedoMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, ironAoMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ironMetallicMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, ironNormalMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, ironRoughnessMap);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
        shader.setMat4("model", model);
        drawMesh(objectGeometry);

        // wall
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, wallAlbedoMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, wallAoMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, wallMetallicMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, wallNormalMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, wallRoughnessMap);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
        shader.setMat4("model", model);
        drawMesh(objectGeometry);

        // 绘制光源
        lightShader.use();
        lightShader.setMat4("view", view);
        newPos = lightPositions[0] + glm::vec3(sin(glfwGetTime()) * radius, cos(glfwGetTime()) * radius, 0.0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", lightColors[0]);
        drawMesh(pointLightGeometry);
        for (int i = 1; i < lightPositions.size(); i++)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", lightColors[i]);
            drawMesh(pointLightGeometry);
        }

        cubemapShader.use();
        cubemapShader.setMat4("view", view);
        cubemapShader.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        drawMesh(boxGeometry);

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
unsigned int loadHDRTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // 图像y轴翻转
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
        stbi_image_free(data);
    }

    return textureID;
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