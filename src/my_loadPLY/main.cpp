#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cmath>

#include <tool/shader.h>
#include <tool/camera.h>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

#include <tool/gui.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
bool loadPLY(const char *filename);
void createVBO();
std::string Shader::dirName;

int verTexNum, faceNum; // 顶点个数，面个数
GLfloat *vertices;
GLuint *indices;
unsigned int VAO, VBO, EBO;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
// int SCREEN_WIDTH = 1600;
// int SCREEN_HEIGHT = 1200;

// camera value
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

Camera camera(glm::vec3(0.0, 0.0, 5.0));

int main(int argc, char *argv[])
{
  Shader::dirName = argv[1];
  glfwInit();
  // 设置主要和次要版本
  const char *glsl_version = "#version 330";

  // 片段着色器将作用域每一个采样点（采用4倍抗锯齿，则每个像素有4个片段（四个采样点））
  // glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 窗口对象
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // -----------------------
  // 创建imgui上下文
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // 设置样式
  ImGui::StyleColorsDark();
  // 设置平台和渲染器
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // -----------------------

  // 设置视口
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LESS);

  // 鼠标键盘事件
  // 1.注册窗口变化监听
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // 2.鼠标事件
  glfwSetCursorPosCallback(window, mouse_callback);

  // 载入ply模型
  if (!loadPLY("./src/my_loadPLY/ply/bun_zipper.ply"))
  {
    return -1;
  };
  // 创建VAO
  createVBO();

  Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl","./shader/geometry.glsl");
  // Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");
  Shader lightObjectShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl");

  SphereGeometry sphereGeometry(0.1, 10.0, 10.0);

  ourShader.use();

  float factor = 0.0;

  // 旋转矩阵
  glm::mat4 ex = glm::eulerAngleX(45.0f);
  glm::mat4 ey = glm::eulerAngleY(45.0f);
  glm::mat4 ez = glm::eulerAngleZ(45.0f);

  glm::mat4 qularXYZ = glm::eulerAngleXYZ(45.0f, 45.0f, 45.0f);

  float fov = 45.0f; // 视锥体的角度
  glm::vec3 view_translate = glm::vec3(0.0, 0.0, -5.0);
  ImVec4 clear_color = ImVec4(200.0 / 255.0, 200.0 / 255.0, 200.0 / 255.0, 1.0); // 25, 25, 25

  // 光照信息
  glm::vec3 lightPosition = glm::vec3(0.8, 0.25, 0.0); // 光照位置
  ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
  ourShader.setFloat("ambientStrength", 0.9);

  // 传递材质属性
  ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
  ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
  ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
  ourShader.setFloat("material.shininess", 16.0f);

  // 设置光照属性
  ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
  ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
  ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;

    // 在标题中显示帧率信息

    int fps_value = (int)round(ImGui::GetIO().Framerate);
    int ms_value = (int)round(1000.0f / ImGui::GetIO().Framerate);

    std::string FPS = std::to_string(fps_value);
    std::string ms = std::to_string(ms_value);
    std::string newTitle = "LearnOpenGL - " + ms + " ms/frame " + FPS;
    glfwSetWindowTitle(window, newTitle.c_str());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::Begin("controls");
    // ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    // ImGui::End();

    // 渲染指令
    // ...
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    factor = glfwGetTime();
    ourShader.setFloat("factor", -factor * 0.3);

    // 修改光源颜色
    // glm::vec3 lightColor;
    // lightColor.x = sin(glfwGetTime() * 2.0f);
    // lightColor.y = sin(glfwGetTime() * 0.7f);
    // lightColor.z = sin(glfwGetTime() * 1.3f);

    // glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);   // 降低影响
    // glm::vec3 ambientColor = diffuseColor * glm::vec3(1.0f); // 很低的影响

    // ourShader.setVec3("material.ambient", ambientColor);
    // ourShader.setVec3("material.diffuse", diffuseColor);

    float radius = 6.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));

    glm::vec3 lightPos = glm::vec3(lightPosition.x * glm::sin(glfwGetTime()), lightPosition.y, lightPosition.x * glm::cos(glfwGetTime()));

    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    ourShader.setMat4("model", model);

    ourShader.setVec3("lightPos", lightPos);
    ourShader.setVec3("viewPos", camera.Position);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faceNum * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 绘制灯光物体
    lightObjectShader.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);

    lightObjectShader.setMat4("model", model);
    lightObjectShader.setMat4("view", view);
    lightObjectShader.setMat4("projection", projection);
    glBindVertexArray(sphereGeometry.VAO);
    glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

    // 渲染 gui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

// 窗口变动监听
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// 键盘输入监听
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

// 载入ply文件
bool loadPLY(const char *filename)
{
  fstream file;
  file.open(filename, ios::in);
  string str;
  if (!file)
  {
    cout << "Cannot open the file" << endl;
    return false;
  }
  while (!file.eof())
  {
    file >> str;
    // cout << str;
    if (str == "ply" || str == "comment" || str == "format" || str == "property")
    {
      getline(file, str, '\n');
    }
    else if (str == "element")
    {
      file >> str;
      if (str == "vertex")
      {
        file >> verTexNum;
        cout << "First we get " << verTexNum << " vertexs" << endl;
        vertices = new GLfloat[verTexNum * 5];
        getline(file, str, '\n');
      }
      else if (str == "face")
      {
        file >> faceNum;
        cout << "Second we get " << faceNum << " faces" << endl;
        indices = new GLuint[faceNum * 3];
        getline(file, str, '\n');
      }
    }
    else if (str == "end_header")
    {
      getline(file, str, '\n');
      int count;
      for (int i = 0; i < verTexNum * 5;)
      {
        file >> vertices[i++];
        file >> vertices[i++];
        file >> vertices[i++];
        file >> vertices[i++];
        file >> vertices[i++];
      }
      for (int i = 0; i < faceNum * 3;)
      {
        file >> count;
        file >> indices[i++];
        file >> indices[i++];
        file >> indices[i++];
      }
      break;
    }
  }
  file.close();
  return true;
}

// 初始化VBO
void createVBO()
{
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verTexNum * 5, vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faceNum * 3, indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}