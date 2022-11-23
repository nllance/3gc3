#ifndef MESH_H
#define MESH_H
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <../stb_image.h>

#include <shader.h>

#include <vector>


// Struct for vertices
struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // Texture
    glm::vec2 TexCoord;

    // Overload the equality operator
    bool operator==(const Vertex& other) const {
        return (Position == other.Position && Normal == other.Normal) && TexCoord == other.TexCoord;
    }
};


// Hash function for vertices, to be used with unordered_map
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const noexcept {
            return ((hash<glm::vec3>()(vertex.Position) ^
                   (hash<glm::vec3>()(vertex.Normal) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.TexCoord) << 1);
        }
    };
}


class Mesh {
    public:
        // Mesh data
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        const char *texPath;
        unsigned int texture;

        // Constructor
        Mesh (std::vector<Vertex> vertices, std::vector<uint32_t> indices, const char *texPath) {
            this->vertices = vertices;
            this->indices = indices;
            this->texPath = texPath;

            setupMesh();
            setupTexture();
        }

        // Render the mesh
        void Draw(Shader &shader) {
            glActiveTexture(GL_TEXTURE0);
            shader.setInt("texture", 0);
            glBindTexture(GL_TEXTURE_2D, this->texture);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        

    private:
        // Vertex array object, Vertex buffer object and element buffer object
        unsigned int VAO, VBO, EBO;

        // Initialize all the buffer objects/arrays
        void setupMesh() {
            // Create all the buffers/arrays
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            // Binding
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // The memory layout of structs are sequential
            // Vertex v = [Position[0], Position[1], Position[2], Normal[0], Normal[1]...]
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

            // Set the vertex attribute pointers
            // Vertex positions
            glEnableVertexAttribArray(0);	
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
            // Vertex normals
            glEnableVertexAttribArray(1);	
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));
            // Vertex texcoords
            glEnableVertexAttribArray(2);	
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, TexCoord));

            // Unbind the VAO
            glBindVertexArray(0);
        }

        // Initialize the texture
        void setupTexture() {
            // Load the texture
            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char *data = stbi_load(texPath, &width, &height, &nrChannels, 0);

            // Activate the texture unit, generate and bind the texture
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            // Set the texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Define the texture using image data
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            // Release the image data
            stbi_image_free(data);
        }
};
#endif
