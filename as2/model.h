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
#include <unordered_map>

// Struct for data from obj files
struct Obj {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
};


class Model {
    public:
        std::vector<Obj> objs;

        // Constructor, expects a vector of paths to obj files
        Model(std::vector<std::string> paths) {
            for (int i = 0; i < paths.size(); i++)
                objs.push_back(loadObjs(paths[i].c_str()));
        }

        // Process the mesh corresponding to the nth obj
        Mesh processObj(int n) {
            return loadMesh(n);
        }

        // Blendshape with given weights
        Mesh blendObjs(std::vector<float> weights) {
            return blendMesh(weights);
        }
        
    
    private:
        // Load a mesh with tinyobjloader, and return the necessary info to further process it
        Obj loadObjs(const char *path) {
            // To initialize using tinyobjloader
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;
            bool bTriangulate = true;

            // Load the objs
            bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, nullptr, bTriangulate);
            if(!bSuc) {
                std::cout << "tinyobj error: " << err.c_str() << std::endl;
            }

            // Put data in an Obj struct
            Obj obj;
            obj.attrib = attrib;
            obj.shapes = shapes;
            return obj;
        }        
        
        // Load the mesh corresponding to the nth obj
        Mesh loadMesh(int n) {
            // Load positions
            std::vector<float> positions;
            for (float coord : objs[n].attrib.vertices) {
                positions.push_back(coord);
            }
            // Load normals
            std::vector<float> normals;
            for (float coord : objs[n].attrib.normals) {
                normals.push_back(coord);
            }
            return processMesh(positions, normals, objs[n].shapes);
        }

        // Blendshape operation with given weights
        Mesh blendMesh(std::vector<float> weights) {
            Obj base = objs[0];

            // New positions [v0.x, v0.y, v0.z, v1.x, v1.y, v1.z...]
            std::vector<float> new_positions;
            // For each vertex position in the base obj file
            uint32_t i, j;
            for (i = 0; i < base.attrib.vertices.size(); i++) {
                // The coordinate of the vertex in the base
                float base_coord = base.attrib.vertices[i];
                // Initialize the new coordinate to the cooridnate of the base
                float new_coord = base_coord;

                // For each obj file
                for (j = 1; j < objs.size(); j++) {
                    // The coordinate of the vertex to add
                    float add_coord = objs[j].attrib.vertices[i];
                    new_coord += weights[j-1] * (add_coord - base_coord);
                }
                new_positions.push_back(new_coord);
            }

            // Use the normals of the base
            std::vector<float> normals;
            for (float coord : base.attrib.normals) {
                normals.push_back(coord);
            }

            return processMesh(new_positions, normals, base.shapes);
        }

        // Process mesh data
        Mesh processMesh(std::vector<float> positions, std::vector<float> normals, std::vector<tinyobj::shape_t> shapes) {
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

                    // Get vertex position
                    position.x = positions[index.vertex_index * 3];
                    position.y = positions[index.vertex_index * 3 + 1];
                    position.z = positions[index.vertex_index * 3 + 2];
                    // Get vertex normals
                    normal.x = normals[index.normal_index * 3];
                    normal.y = normals[index.normal_index * 3 + 1];
                    normal.z = normals[index.normal_index * 3 + 2];
                    // Set vertex
                    vertex.Position = position;
                    vertex.Normal = normal;

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

            return Mesh(vertices, indices);
        }
};
#endif
