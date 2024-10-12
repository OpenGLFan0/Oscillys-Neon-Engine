#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// Function to compile shader and check for errors
static unsigned int CompileShader(const std::string& source, unsigned int type) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << endl;
        cout << message << endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

// Function to create a shader program
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void) {
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1080, 800, "OpenGL Renderer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        cout << "Error initializing GLEW!" << endl;
        return -1;
    }

    cout << glGetString(GL_VERSION) << endl;

    // Define vertices (x, y, texX, texY)
    float positions[] = {
    // Back face (Z = -1.0)
    -0.5f, -0.5f, -1.0f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -1.0f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f, -1.0f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f, -1.0f,   0.0f, 1.0f,  // Top-left

    // Front face (Z = 1.0)
    -0.5f, -0.5f,  1.0f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  1.0f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  1.0f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  1.0f,   0.0f, 1.0f,  // Top-left

    // Left face (X = -0.5)
    -0.5f, -0.5f,  1.0f,   1.0f, 0.0f,  // Bottom-left (front)
    -0.5f, -0.5f, -1.0f,   0.0f, 0.0f,  // Bottom-left (back)
    -0.5f,  0.5f, -1.0f,   0.0f, 1.0f,  // Top-left (back)
    -0.5f,  0.5f,  1.0f,   1.0f, 1.0f,  // Top-left (front)

    // Right face (X = 0.5)
     0.5f, -0.5f,  1.0f,   1.0f, 0.0f,  // Bottom-right (front)
     0.5f, -0.5f, -1.0f,   0.0f, 0.0f,  // Bottom-right (back)
     0.5f,  0.5f, -1.0f,   0.0f, 1.0f,  // Top-right (back)
     0.5f,  0.5f,  1.0f,   1.0f, 1.0f,  // Top-right (front)

    // Bottom face (Y = -0.5)
    -0.5f, -0.5f, -1.0f,   0.0f, 1.0f,  // Bottom-left (back)
     0.5f, -0.5f, -1.0f,   1.0f, 1.0f,  // Bottom-right (back)
     0.5f, -0.5f,  1.0f,   1.0f, 0.0f,  // Bottom-right (front)
    -0.5f, -0.5f,  1.0f,   0.0f, 0.0f,  // Bottom-left (front)

    // Top face (Y = 0.5)
    -0.5f,  0.5f, -1.0f,   0.0f, 1.0f,  // Top-left (back)
     0.5f,  0.5f, -1.0f,   1.0f, 1.0f,  // Top-right (back)
     0.5f,  0.5f,  1.0f,   1.0f, 0.0f,  // Top-right (front)
    -0.5f,  0.5f,  1.0f,   0.0f, 0.0f   // Top-left (front)
};

    unsigned int indices[] = {
         // Back face
        0, 1, 2,
        2, 3, 0,

        // Front face
        4, 5, 6,
        6, 7, 4,

        // Left face
        0, 4, 7,
        7, 3, 0,

        // Right face
        1, 5, 6,
        6, 2, 1,

        // Bottom face
        0, 1, 5,
        5, 4, 0,

        // Top face
        3, 2, 6,
        6, 7, 3
    };

    // Create and bind VAO
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO (Vertex Buffer Object) and send data
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    // Define the layout of the vertex data (position: 0, texcoord: 1)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position attribute

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // TexCoord attribute

    // Create IBO (Index Buffer Object) and send data
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Load texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cout << "Failed to load texture" << endl;
    }
    stbi_image_free(data);

    // Shader sources
    string vertexShader =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 transform;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * transform * vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

    string fragmentShader =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    // Transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1080.0f / 800.0f, 0.1f, 100.0f);

    // Get uniform locations
    unsigned int transformLoc = glGetUniformLocation(shader, "transform");
    unsigned int viewLoc = glGetUniformLocation(shader, "view");
    unsigned int projLoc = glGetUniformLocation(shader, "projection");

    // Set uniform values for view and projection
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    float angle = 0.0f; // Rotation angle

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update rotation angle
        angle += 0.01f; // Adjust rotation speed

        // Create rotation transformation
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis rotation
        trans = glm::rotate(trans, angle * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis rotation
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        // Bind the texture and VAO, and draw
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}