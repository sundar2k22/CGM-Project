#include <GL/glew.h>   
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <OpenGL/gl.h>    
#include <GLFW/glfw3.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include <cmath>
#include <vector>
#include <iostream>
#include <random>

// Vertex Shader Source
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 fragTexCoord;
uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    fragTexCoord = texCoord; // Pass texture coordinates to fragment shader
}
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 color;

in vec2 fragTexCoord;
uniform sampler2D texture1;

void main() {
    color = texture(texture1, fragTexCoord); // Use the texture color
}
)";

// Function to generate sphere vertices and texture coordinates
void generateSphere(float radius, int segments, int rings, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    for (int y = 0; y <= rings; ++y) {
        for (int x = 0; x <= segments; ++x) {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)rings;

            float xPos = radius * cos(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);
            float yPos = radius * cos(ySegment * M_PI);
            float zPos = radius * sin(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            // Texture coordinates
            vertices.push_back(xSegment); // S
            vertices.push_back(ySegment); // T
        }
    }

    for (int y = 0; y < rings; ++y) {
        for (int x = 0; x < segments; ++x) {
            int first = (y * (segments + 1)) + x;
            int second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

// Function to generate random star positions
void generateStars(int numStars, std::vector<glm::vec3>& stars) {
    for (int i = 0; i < numStars; ++i) {
        // Random distance from the center (you can adjust the range)
        float distance = 8.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f; // Range from 8 to 10
        // Random initial angle
        float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * M_PI; // Random angle in radians

        // Push the generated star position into the vector
        stars.push_back(glm::vec3(distance * cos(angle), distance * sin(angle), 0.0f)); // Using the proper constructor
    }
}

// Function to compile shaders
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

// Function to load texture
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    return textureID;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Set GLFW context version (OpenGL 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Textured Sphere with Stars", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    // Set viewport
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Generate sphere vertices and indices
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSphere(1.0f, 64, 32, vertices, indices);

    // Create Vertex Array Object and Vertex Buffer Objects
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Define the vertex data layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Compile shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Cleanup shaders as they are now linked
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Load texture
    GLuint earthTexture = loadTexture("earth_texture.jpg"); // Ensure you have the Earth texture image in the same directory

        // Generate stars with their original positions
    std::vector<glm::vec3> stars;
    generateStars(300, stars); // Generate 300 stars within a range of 10.0 units

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);

        // Create transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mvp = projection * view * model;

        // Set the MVP uniform
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // Use texture unit 0

        // Draw the sphere
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
       float starDistance = 3.0f; // Adjust star distance if necessary
       for (size_t i = 0; i < stars.size(); ++i) {
	   // Calculate angle for revolution
	   float angle = glfwGetTime() + (i * (2.0f * M_PI / stars.size())); // Offset each star's angle
	   
	   // Position stars in a circular path around the sphere
	   float x = starDistance * cos(angle); // Circular motion on x-axis
	   float z = starDistance * sin(angle); // Circular motion on z-axis
	   glm::vec3 starPosition(x, stars[i].y, z); // Keep the original y position of the star

	   // Create the star model matrix
	   glm::mat4 starModel = glm::translate(glm::mat4(1.0f), starPosition);
	   starModel = glm::scale(starModel, glm::vec3(0.05f)); // Scale stars down
	   glm::mat4 starMVP = projection * view * starModel;

	   glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(starMVP));

	   // Draw a point for the star
	   glDrawArrays(GL_POINTS, 0, 1); // Drawing a single point
       }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}
