#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

#include <model.h>


static unsigned int ss_id = 0;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height);

std::vector<float> get_weights(const char *path);


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
    GLFWwindow* window = glfwCreateWindow(width, height, "blendshape", NULL, NULL);
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
    // Wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Load models
    std::vector<std::string> paths;
    paths.push_back("../data/faces/base.obj");
    for (int i = 0; i < 35; i++) {
        std::string path = "../data/faces/" + std::to_string(i) + ".obj";
        paths.push_back(path);
    }
    Model src_model(paths);

    // Load weights
    std::vector<float> weights = get_weights("../data/weights/11.weights");

    // Blendshape
    std::vector<Vertex>   new_vertices;
    std::vector<uint32_t> new_indices;
    // The way faces are constructed by vertices is the same across all shapes
    // So we can just use the indices of the base
    new_indices = src_model.meshes[0].indices;
    // Vertices of the base
    std::vector<Vertex> base_vertices = src_model.meshes[0].vertices;

    // Iterate through each vertex of the base
    unsigned int i, j;
    for (i = 0; i < base_vertices.size(); i++) {
        Vertex new_vertex;
        glm::vec3 base_position = base_vertices[i].Position;
        // Initialize the new position to that of the base
        glm::vec3 new_position = base_position;
        // Use the normals of the base
        new_vertex.Normal = base_vertices[i].Normal;

        // Iterate through each mesh to get the corresponding vertex
        for (j = 1; j < src_model.meshes.size(); j++) {
            // Vertices of the jth mesh
            std::vector<Vertex> add_vertices = src_model.meshes[j].vertices;
            // Check if vertex index is out of range
            if (i >= add_vertices.size()) continue;  
            // Blending computation
            new_position.x += weights[j-1] * (add_vertices[i].Position.x - base_position.x);
            new_position.y += weights[j-1] * (add_vertices[i].Position.y - base_position.y);
            new_position.z += weights[j-1] * (add_vertices[i].Position.z - base_position.z);
        }

        // Set new position for the vertex
        new_vertex.Position = new_position;
        new_vertices.push_back(new_vertex);
    }

    // New Mesh to draw
    Mesh new_mesh = Mesh(new_vertices, new_indices);

    // Transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 100, 100), glm::vec3(0, 100, 0), glm::vec3(0, 1, 0));
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

    // Compile and use shaders
    Shader shader("../vshader.vs", "../fshader.fs");
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Rendering loop, keeps running until told to stop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);
        // Background colour
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        // Clear the colour buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the mesh
        new_mesh.Draw();

        // Swap the colour buffer during this render iteration and show to the screen
        glfwSwapBuffers(window);
        // Check if any events are triggered (keyboard etc)
        glfwPollEvents();
    }

   // Clean everything
   glfwTerminate();
   return 0;
}


// Function to read weights from a file, takes the path to the file
std::vector<float> get_weights(const char *path) {
    std::vector<float> weights;
    // Read from file
    std::ifstream infile(path);
    float x;
    while (infile >> x) {
        weights.push_back(x);
    }

    return weights;
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
    std::string file = "../images/blended11.ppm";
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
