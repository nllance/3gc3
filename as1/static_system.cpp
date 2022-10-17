#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

static unsigned int ss_id = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height);


// Vertex shader
const char *vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColour;"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 ourColour;\n"
    "void main() {\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   ourColour = aColour;\n"
    "}\0";

// Fragment shader
const char *fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 fragColour;\n"
    "in vec3 ourColour;\n"
    "void main() {\n"
    "   fragColour = vec4(ourColour, 1.0);\n"
    "}\0";


int main(void) {
    // Initialize GLFW
    glfwInit();
    // Set opengl version to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Screen size
    int width = 1024;
    int height = 768;

    // Create a window object
    GLFWwindow* window = glfwCreateWindow(width, height, "static_system", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make the context of `window` the main context on the current thread
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Size of the rendering window
    // These come after the initialization of GLAD
    glViewport(0, 0, width, height);
    // Call on every window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Check if shader compilation is successful
    int success;
    char error_msg[512];

    // Vertex shader
    // Create a shader object
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attach the shader source code, and compile
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, error_msg);
        std::cout << "Vertex Shader Failed: " << error_msg << std::endl;
    }

    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, error_msg);
        std::cout << "Fragment Shader Failed: " << error_msg << std::endl;
    }

    // Shader Program object - final linked versin of multiple shaders combined
    unsigned int shaderProgram = glCreateProgram();
    // Attach shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_msg);
        std::cout << "Shader Program Failed: " << error_msg << std::endl;
    }
    // Delete shader objects once they are linked into the program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Local coordinates for the cube
    float vertices[] = {
        // Positions         Colours
        // Front face
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
        // Back face
         1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        // Left face
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        // Right face
         1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f,
        // Top face
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        // Bottom face
        -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    };

    // Generate a vertex array object (VAO)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // Generate a vertex buffer object (VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    // Colour attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Projection matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(30.0f), (float) width/height, 0.1f, 1000.0f);

    // Get uniform locations
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int camLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    // Rendering loop, keeps running until told to stop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Face culling
        glEnable(GL_CULL_FACE);
        // Depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Background colour
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        // Clear the colour buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View matrix
        glm::vec3 camPos = glm::vec3(-60.0f, 50.0f, 40.0f);
        glm::vec3 camTarget = glm::vec3(20.0f, 0.0f, 0.0f);
        glm::mat4 view;
        // Last param = up vector in world space
        view = glm::lookAt(camPos, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));

        // Pass the view matrix and the projection matrix to the shader
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(camLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);

        // The sun - model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // The earth - model matrix
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(20.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-23.4f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // The moon - model matrix
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(30.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Swap the colour buffer during this render iteration and show to the screen
        glfwSwapBuffers(window);
        // Check if any events are triggered (keyboard etc)
        glfwPollEvents();
    }

   // Clean everything
   glDeleteVertexArrays(1, &VAO);
   glDeleteBuffers(1, &VBO);
   glDeleteProgram(shaderProgram);
   glfwTerminate();
   return 0;
}


// When the user resizes the window, the viewport should also be resized
// This is called each time the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    // Press esc to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // Press p to capture screen
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("pic", buffer_width, buffer_height);
    }
}

void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte * pixels = new GLubyte [totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id ++;
    delete [] pixels;
    fout.flush();
    fout.close();
}
