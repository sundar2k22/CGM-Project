#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include <iostream>
#include <vector>
#include <cmath>

// Vertex Shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main()
    {
        FragColor = vec4(color, 1.0f);
    }
)";

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int compileShader(unsigned int type, const char* source);
unsigned int createShaderProgram(const char* vertexShader, const char* fragmentShader);
std::vector<float> createSphereVertices(float radius, int sectors, int stacks);

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Earth and Satellite", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Create and compile our GLSL program from the shaders
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Set up vertex data for Earth
    std::vector<float> earthVertices = createSphereVertices(0.5f, 36, 18);

    // Set up vertex data for Satellite
    std::vector<float> satelliteVertices = createSphereVertices(0.05f, 12, 6);

    // Set up vertex data for Constellation
    std::vector<float> constellationVertices = {
        -0.8f, 0.8f, 0.0f,
        -0.6f, 0.9f, 0.0f,
        -0.4f, 0.7f, 0.0f,
        -0.2f, 0.8f, 0.0f,
        0.0f, 0.9f, 0.0f,
        0.2f, 0.7f, 0.0f
    };

    // Earth VAO and VBO
    unsigned int earthVAO, earthVBO;
    glGenVertexArrays(1, &earthVAO);
    glGenBuffers(1, &earthVBO);
    glBindVertexArray(earthVAO);
    glBindBuffer(GL_ARRAY_BUFFER, earthVBO);
    glBufferData(GL_ARRAY_BUFFER, earthVertices.size() * sizeof(float), earthVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Satellite VAO and VBO
    unsigned int satelliteVAO, satelliteVBO;
    glGenVertexArrays(1, &satelliteVAO);
    glGenBuffers(1, &satelliteVBO);
    glBindVertexArray(satelliteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, satelliteVBO);
    glBufferData(GL_ARRAY_BUFFER, satelliteVertices.size() * sizeof(float), satelliteVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Constellation VAO and VBO
    unsigned int constellationVAO, constellationVBO;
    glGenVertexArrays(1, &constellationVAO);
    glGenBuffers(1, &constellationVBO);
    glBindVertexArray(constellationVAO);
    glBindBuffer(GL_ARRAY_BUFFER, constellationVBO);
    glBufferData(GL_ARRAY_BUFFER, constellationVertices.size() * sizeof(float), constellationVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader program
        glUseProgram(shaderProgram);

        // Create transformations
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Pass transformation matrices to the shader
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Earth
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "color");
        glUniform3f(colorLoc, 0.0f, 0.5f, 1.0f);  // Blue color for Earth

        glBindVertexArray(earthVAO);
        glDrawArrays(GL_TRIANGLES, 0, earthVertices.size() / 3);

        // Satellite
        model = glm::mat4(1.0f);
        float satelliteOrbitRadius = 1.0f;
        float satelliteAngle = (float)glfwGetTime() * 2.0f;
        model = glm::translate(model, glm::vec3(
            satelliteOrbitRadius * cos(satelliteAngle),
            0.0f,
            satelliteOrbitRadius * sin(satelliteAngle)
        ));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);  // Red color for Satellite

        glBindVertexArray(satelliteVAO);
        glDrawArrays(GL_TRIANGLES, 0, satelliteVertices.size() / 3);

        // Constellation
        model = glm::mat4(1.0f);
        float constellationOffset = fmodf((float)glfwGetTime() * 0.5f, 2.0f) - 1.0f;
        model = glm::translate(model, glm::vec3(constellationOffset, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);  // White color for Constellation

        glBindVertexArray(constellationVAO);
        glDrawArrays(GL_LINE_STRIP, 0, constellationVertices.size() / 3);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources
    glDeleteVertexArrays(1, &earthVAO);
    glDeleteVertexArrays(1, &satelliteVAO);
    glDeleteVertexArrays(1, &constellationVAO);
    glDeleteBuffers(1, &earthVBO);
    glDeleteBuffers(1, &satelliteVBO);
    glDeleteBuffers(1, &constellationVBO);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwTerminate();
    return 0;
}

// Process all input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// GLFW: whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Compile shader
unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}

// Create shader program
unsigned int createShaderProgram(const char* vertexShader, const char* fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// Create sphere vertices
std::vector<float> createSphereVertices(float radius, int sectors, int stacks)
{
    std::vector<float> vertices;
    float x, y, z, xy;
    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j)
        {
            sectorAngle = j * sectorStep;
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    std::vector<float> sphereVertices;
    for (int i = 0; i < stacks; ++i)
    {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                sphereVertices.push_back(vertices[3 * k1]);
                sphereVertices.push_back(vertices[3 * k1 + 1]);
                sphereVertices.push_back(vertices[3 * k1 + 2]);
                sphereVertices.push_back(vertices[3 * k2]);
                sphereVertices.push_back(vertices[3 * k2 + 1]);
                sphereVertices.push_back(vertices[3 * k2 + 2]);
                sphereVertices.push_back(vertices[3 * (k1 + 1)]);
                sphereVertices.push_back(vertices[3 * (k1 + 1) + 1]);
                sphereVertices.push_back(vertices[3 * (k1 + 1) + 2]);
            }

            if (i != (stacks - 1))
            {
                sphereVertices.push_back(vertices[3 * k2]);
                sphereVertices.push_back(vertices[3 * k2 + 1]);
                sphereVertices.push_back(vertices[3 * k2 + 2]);
                sphereVertices.push_back(vertices[3 * (k1 + 1)]);
                sphereVertices.push_back(vertices[3 * (k1 + 1) + 1]);
                sphereVertices.push_back(vertices[3 * (k1 + 1) + 2]);
                sphereVertices.push_back(vertices[3 * (k2 + 1)]);
                sphereVertices.push_back(vertices[3 * (k2 + 1) + 1]);
                sphereVertices.push_back(vertices[3 * (k2 + 1) + 2]);
            }
        }
    }

    return sphereVertices;
}

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include "glm/glm/glm.hpp"
// #include "glm/glm/gtc/matrix_transform.hpp"
// #include "glm/glm/gtc/type_ptr.hpp"
// #include <iostream>
// #include <vector>
// #include <cmath>
// #include <SOIL/src/SOIL.h> // Include SOIL library for loading textures

// // Vertex Shader source code
// const char* vertexShaderSource = R"(
//     #version 330 core
//     layout (location = 0) in vec3 aPos;
//     layout (location = 1) in vec2 aTexCoord;
//     uniform mat4 model;
//     uniform mat4 view;
//     uniform mat4 projection;
//     out vec2 TexCoord;
//     void main()
//     {
//         gl_Position = projection * view * model * vec4(aPos, 1.0);
//         TexCoord = aTexCoord;
//     }
// )";

// // Fragment Shader source code
// const char* fragmentShaderSource = R"(
//     #version 330 core
//     out vec4 FragColor;
//     in vec2 TexCoord;
//     uniform sampler2D texture1;
//     void main()
//     {
//         FragColor = texture(texture1, TexCoord);
//     }
// )";

// // Window dimensions
// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// // Function prototypes
// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow* window);
// unsigned int compileShader(unsigned int type, const char* source);
// unsigned int createShaderProgram(const char* vertexShader, const char* fragmentShader);
// std::vector<float> createSphereVertices(float radius, int sectors, int stacks);

// int main()
// {
//     // Initialize GLFW
//     if (!glfwInit())
//     {
//         std::cout << "Failed to initialize GLFW" << std::endl;
//         return -1;
//     }

//     // Configure GLFW
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     // Create a GLFW window
//     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Earth and Satellite", NULL, NULL);
//     if (window == NULL)
//     {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     // Initialize GLAD
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         return -1;
//     }

//     // Enable depth testing and anti-aliasing
//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_MULTISAMPLE);

//     // Load and create a texture 
//     unsigned int earthTexture;
//     glGenTextures(1, &earthTexture);
//     glBindTexture(GL_TEXTURE_2D, earthTexture);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     int width, height, nrChannels;
//     unsigned char *data = SOIL_load_image("earth.jpg", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "Failed to load texture" << std::endl;
//     }
//     SOIL_free_image_data(data);

//     // Create and compile our GLSL program from the shaders
//     unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

//     // Set up vertex data for Earth with texture coordinates
//     std::vector<float> earthVertices = createSphereVertices(0.5f, 36, 18);

//     // Earth VAO and VBO
//     unsigned int earthVAO, earthVBO;
//     glGenVertexArrays(1, &earthVAO);
//     glGenBuffers(1, &earthVBO);
//     glBindVertexArray(earthVAO);
//     glBindBuffer(GL_ARRAY_BUFFER, earthVBO);
//     glBufferData(GL_ARRAY_BUFFER, earthVertices.size() * sizeof(float), earthVertices.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);

//     // Render loop
//     while (!glfwWindowShouldClose(window))
//     {
//         // Input
//         processInput(window);

//         // Render
//         glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // Dark gradient background
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         // Use our shader program
//         glUseProgram(shaderProgram);

//         // Create transformations
//         glm::mat4 view = glm::mat4(1.0f);
//         glm::mat4 projection = glm::mat4(1.0f);
//         view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
//         projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

//         // Pass transformation matrices to the shader
//         unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
//         unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
//         glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//         glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

//         // Earth
//         glm::mat4 model = glm::mat4(1.0f);
//         model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
//         unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
//         glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

//         glActiveTexture(GL_TEXTURE0);
//         glBindTexture(GL_TEXTURE_2D, earthTexture);
//         glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

//         glBindVertexArray(earthVAO);
//         glDrawArrays(GL_TRIANGLES, 0, earthVertices.size() / 5);

//         // Swap buffers and poll IO events
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     // De-allocate all resources
//     glDeleteVertexArrays(1, &earthVAO);
//     glDeleteBuffers(1, &earthVBO);
//     glDeleteProgram(shaderProgram);
//     glDeleteTextures(1, &earthTexture);

//     // Terminate GLFW
//     glfwTerminate();
//     return 0;
// }

// // Process all input
// void processInput(GLFWwindow* window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// // GLFW: whenever the window size changed, this callback function executes
// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }
// std::vector<float> createSphereVertices(float radius, int sectors, int stacks)
// {
//     std::vector<float> vertices;
//     float x, y, z, xy;                            
//     float nx, ny, nz, lengthInv = 1.0f / radius;   
//     float s, t;                                    
//     float sectorStep = 2 * M_PI / sectors;
//     float stackStep = M_PI / stacks;
//     float sectorAngle, stackAngle;

//     for (int i = 0; i <= stacks; ++i)
//     {
//         stackAngle = M_PI / 2 - i * stackStep;        
//         xy = radius * cosf(stackAngle);             
//         z = radius * sinf(stackAngle);              

//         for (int j = 0; j <= sectors; ++j)
//         {
//             sectorAngle = j * sectorStep;           

//             x = xy * cosf(sectorAngle);             
//             y = xy * sinf(sectorAngle);             
//             vertices.push_back(x);
//             vertices.push_back(y);
//             vertices.push_back(z);

//             s = (float)j / sectors;
//             t = (float)i / stacks;
//             vertices.push_back(s);
//             vertices.push_back(t);
//         }
//     }

//     return vertices;
// }
// void drawOrbit(float radius, int segments)
// {
//     std::vector<float> orbitVertices;
//     float angleStep = 2.0f * M_PI / segments;
//     for (int i = 0; i <= segments; i++)
//     {
//         float angle = i * angleStep;
//         orbitVertices.push_back(radius * cosf(angle));
//         orbitVertices.push_back(radius * sinf(angle));
//         orbitVertices.push_back(0.0f);
//     }

//     unsigned int orbitVAO, orbitVBO;
//     glGenVertexArrays(1, &orbitVAO);
//     glGenBuffers(1, &orbitVBO);
//     glBindVertexArray(orbitVAO);
//     glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
//     glBufferData(GL_ARRAY_BUFFER, orbitVertices.size() * sizeof(float), &orbitVertices[0], GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     glBindVertexArray(orbitVAO);
//     glDrawArrays(GL_LINE_LOOP, 0, segments);

//     glDeleteVertexArrays(1, &orbitVAO);
//     glDeleteBuffers(1, &orbitVBO);
// }
// glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

// void processInput(GLFWwindow *window)
// {
//     float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//         cameraPos += cameraSpeed * cameraFront;
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//         cameraPos -= cameraSpeed * cameraFront;
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
// }
// glDeleteVertexArrays(1, &earthVAO);
// glDeleteBuffers(1, &earthVBO);
// glDeleteTextures(1, &earthTexture);
// glDeleteProgram(shaderProgram);