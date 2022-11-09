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
struct KeyFuncs {
    bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const {
        return lhs.x < rhs.x ||
           (lhs.x == rhs.x) && (lhs.y < rhs.y) || 
           (lhs.y == rhs.y) && (lhs.z < rhs.z);
    }
};
// Map type to keep track of unique vertices and respective indices
typedef std::map<glm::vec3, uint32_t, KeyFuncs> MyMap;


class Model {
    public:
        std::vector<Mesh> meshes;

        // Constructor, expects a filepath to a model and the number of obj files to load
        Model(std::string path, int num_of_meshes) {
            // When only given 1 mesh, the input path should be the full path to an .obj file
            if (num_of_meshes == 1) {
                loadMesh(path.c_str());
            }
            // When multiple meshes, calculate the full path to each
            else {
                for (int i = 0; i < num_of_meshes; i++) {
                    std::string obj_path = path + std::to_string(i) + ".obj";
                    const char *char_obj_path = obj_path.c_str();
                    loadMesh(char_obj_path);
                }
            }
        }

        // Draw the model
        void Draw(Shader &shader) {
            for (unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].Draw(shader);
        }
    
    private:
        // Load a mesh with tinyobjloader, and store the results in the meshes vector
        void loadMesh(const char *path) {
            // To initialize using tinyobloader
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

            // For each shape
            for (auto shape : shapes) {
                // For each face
                for (auto face : shape.mesh.indices) {
                    Vertex vertex;
                    glm::vec3 position, normal;

                    // Get vertex position
                    position.x = attrib.vertices[face.vertex_index * 3];
                    position.y = attrib.vertices[face.vertex_index * 3 + 1];
                    position.z = attrib.vertices[face.vertex_index * 3 + 2];

                    // Add the vertex to the vertices list if not already present
                    if (uniqueVertices.count(position) == 0) {
                        // Set the index of the new vertex
                        uniqueVertices[position] = static_cast<uint32_t> (vertices.size());
                        // Get vertex normals
                        normal.x = attrib.normals[face.normal_index * 3];
                        normal.y = attrib.normals[face.normal_index * 3 + 1];
                        normal.z = attrib.normals[face.normal_index * 3 + 2];
                        // Set vertex
                        vertex.Position = position;
                        vertex.Normal = normal; 
                        vertices.push_back(vertex);
                    }

                    // Add the index to the indices list
                    indices.push_back(uniqueVertices[position]);
                }
            }
            return Mesh(vertices, indices);
        }
};
#endif
