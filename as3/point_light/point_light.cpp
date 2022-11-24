#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>
#include <fstream>

#include <model.h>


static unsigned int ss_id = 0;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height);


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
    GLFWwindow* window = glfwCreateWindow(width, height, "point_light", NULL, NULL);
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

    // Face culling
    glEnable(GL_CULL_FACE);
    // Depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(50.0f, 100.0f, 200.0f), glm::vec3(0.0f, 80.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
    // Transform the normals to world space
    // Because the calculations in the fragment shader are all done in world space
    glm::mat3 normal = glm::inverseTranspose(glm::mat3(model));

    // Compile and use shaders
    Shader shader("../point_light/vshader.vs", "../point_light/fshader.fs");
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat3("normal", normal);

    // Lighting
    shader.setFloat("light.constant", 1.0f);
    shader.setFloat("light.linear", 0.007f * pow(10, -4));
    shader.setFloat("light.quadratic", 0.0002f * pow(10, -4));
    shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setVec3("light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));

    // Load model
    std::vector<std::string> objPaths;
    std::vector<std::string> texPaths;
    objPaths.push_back("../data/timmy.obj");
    texPaths.push_back("../data/timmy.png");
    Model src(objPaths, texPaths);

    float theta = 0;

    // Rendering loop, keeps running until told to stop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);
        // Background colour
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        // Clear the colour buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move the light source
        shader.setVec3("light.position", glm::vec3(sin(theta) * 150, 100.0f, cos(theta) * 150));
        theta += 0.05f;

        // Draw the mesh
        src.Draw(shader);

        // Swap the colour buffer during this render iteration and show to the screen
        glfwSwapBuffers(window);
        // Check if any events are triggered (keyboard etc)
        glfwPollEvents();
    }

   // Clean everything
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
    std::string file = "../results/timmy_point2.ppm";
    std::ofstream fout(file);
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
