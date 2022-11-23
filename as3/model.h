#ifndef MODEL_H
#define MODEL_H
#define TINYOBJLOADER_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <../tiny_obj_loader.h>

#include <mesh.h>
#include <shader.h>

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>


class Model {
    public:
        std::vector<Mesh> meshes;

        // Constructor, expects a vector of paths to obj files, and a vector of paths to texture files
        Model(std::vector<std::string> objPaths, std::vector<std::string> texPaths) {
            for (int i = 0; i < objPaths.size(); i++) {
                assert(objPaths.size() == texPaths.size());
                meshes.push_back(loadMesh(objPaths[i].c_str(), texPaths[i].c_str()));
            }
        }

        // Draw the model, and thus all its meshes
        void Draw(Shader &shader) {
            for(unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].Draw(shader);
        }
        
    
    private:
        // Load an obj file with tinyobjloader, and load a texture from an image
        Mesh loadMesh(const char *objPath, const char *texPath) {
            // To initialize using tinyobjloader
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;
            bool bTriangulate = true;

            // Load the objs
            bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath, nullptr, bTriangulate);
            if(!bSuc) {
                std::cout << "tinyobj error: " << err.c_str() << std::endl;
            }
            

            // Data
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            // Keep track of unique vertices and respective indices
            // If a vertex already exists, we do not add it to the list of vertices again
            std::unordered_map<Vertex, uint32_t> uniqueVertices;

            // For each index
            for (auto shape : shapes) {
                for (auto index : shape.mesh.indices) {
                    Vertex vertex;
                    glm::vec3 position, normal;
                    glm::vec2 textcoord;

                    // Get vertex position
                    position.x = attrib.vertices[index.vertex_index * 3];
                    position.y = attrib.vertices[index.vertex_index * 3 + 1];
                    position.z = attrib.vertices[index.vertex_index * 3 + 2];
                    // Get vertex normals
                    normal.x = attrib.normals[index.normal_index * 3];
                    normal.y = attrib.normals[index.normal_index * 3 + 1];
                    normal.z = attrib.normals[index.normal_index * 3 + 2];
                    // Get vertex texcoords
                    textcoord.x = attrib.texcoords[index.texcoord_index * 2];
                    textcoord.y = attrib.texcoords[index.texcoord_index * 2 + 1];
                    // Set vertex
                    vertex.Position = position;
                    vertex.Normal = normal;
                    vertex.TexCoord = textcoord;

                    // Add the vertex to the vertices list if not already present
                    if (uniqueVertices.count(vertex) == 0) {
                        // Set the index of the new vertex
                        uniqueVertices[vertex] = static_cast<uint32_t> (vertices.size());
                        vertices.push_back(vertex);
                    }

                    // Add the index to the indices list
                    indices.push_back(uniqueVertices[vertex]);
                }
            }

            return Mesh(vertices, indices, texPath);
        }
};
#endif
