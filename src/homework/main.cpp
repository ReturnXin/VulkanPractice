#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <tool/shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

float angle[][3] = {{24, 73, -142}};
int main(void)
{

    for (int i = 0; i < 18; i++)
    {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, angle[i][2], glm::vec3(0.0, 0.0, 1.0));
        trans = glm::rotate(trans, angle[i][1], glm::vec3(1.0, 0.0, 0.0));
        trans = glm::rotate(trans, angle[i][0], glm::vec3(0.0, 1.0, 0.0));

        trans = glm::rotate(trans, 30.0f, glm::vec3(1.0, 0.0, 0.0));
        
    }
}