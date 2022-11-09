#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <shader.h>

#include <vector>


struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
};


class Mesh {
    public:
        // Mesh data
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
        // Vertex array object
        unsigned int VAO;

        // Constructor
        Mesh (std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
            this->vertices = vertices;
            this->indices = indices;

            // Set up the vertex buffers and attribute pointers
            setupMesh();
        }

        // Render the mesh
        void Draw(Shader &shader) {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

    private:
        // Vertex buffer object and element buffer object
        unsigned int VBO, EBO;

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

            // Unbind the VAO
            glBindVertexArray(0);
        }
};
#endif
