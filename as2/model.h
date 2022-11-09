#ifndef MODEL_H
#define MODEL_H
#define TINYOBJLOADER_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

#include <mesh.h>
#include <shader.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>


// Define "<" for glm::vec3
struct KeyFunc {
    bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const {
        return lhs.x < rhs.x ||
           (lhs.x == rhs.x && lhs.y < rhs.y) || 
           (lhs.x == rhs.x && (lhs.y == rhs.y && lhs.z < rhs.z));
    }
};
// Map type to keep track of unique vertices and respective indices
typedef std::map<glm::vec3, uint32_t, KeyFunc> MyMap;


class Model {
    public:
        std::vector<Mesh> meshes;

        // Constructor, expects a vector of paths to obj files
        Model(std::vector<std::string> paths) {
            for (int i = 0; i < paths.size(); i++)
                loadMesh(paths[i].c_str());
        }

        // Draw the model
        void Draw(Shader &shader) {
            for (int i = 0; i < meshes.size(); i++)
                meshes[i].Draw(shader);
        }
    
    private:
        // Load a mesh with tinyobjloader, and store the results in the meshes vector
        void loadMesh(const char *path) {
            // To initialize using tinyobjloader
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;
            bool bTriangulate = true;

            // Load the model
            bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, nullptr, bTriangulate);
            if(!bSuc) {
                std::cout << "tinyobj error: " << err.c_str() << std::endl;
            }
            Mesh mesh = processMesh(attrib, shapes);
            meshes.push_back(mesh);
        }

        // Process mesh data
        Mesh processMesh(tinyobj::attrib_t attrib, std::vector<tinyobj::shape_t> shapes) {
            // Data
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            // Keep track of unique vertices and respective indices
            // If a vertex already exists, we do not add it to the list of vertices again
            MyMap uniqueVertices;

            int size = 0;

             // For each face
            for (auto face : shapes[0].mesh.indices) {
                Vertex vertex;
                glm::vec3 position, normal;

                // Get vertex position
                position.x = attrib.vertices[face.vertex_index * 3];
                position.y = attrib.vertices[face.vertex_index * 3 + 1];
                position.z = attrib.vertices[face.vertex_index * 3 + 2];

                // Add the vertex to the vertices list if not already present
                if (uniqueVertices.count(position) == 0) {
                    // Get vertex normals
                    normal.x = attrib.normals[face.normal_index * 3];
                    normal.y = attrib.normals[face.normal_index * 3 + 1];
                    normal.z = attrib.normals[face.normal_index * 3 + 2];
                    // Set vertex
                    vertex.Position = position;
                    vertex.Normal = normal;
                    // Set the index of the new vertex
                    uniqueVertices[position] = static_cast<uint32_t> (vertices.size());
                    vertices.push_back(vertex);
                }

                // Add the index to the indices list
                indices.push_back(uniqueVertices[position]);
            }
            
            return Mesh(vertices, indices);
        }
};
#endif
