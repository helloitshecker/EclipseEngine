#include "ModelLoader.hpp"
#include <EclipseUtils/Logger.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace e;

et::model loader::loadobj(const char* file_location) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_location)) {
        LOG::SEND("Failed to load OBJ file!");
        throw std::runtime_error("Failed to load OBJ file");
    }

    std::vector<float> interleaved;
    std::vector<et::u32> indices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            // Extract vertex positions
            int vertex_index = 3 * index.vertex_index;
            interleaved.push_back(attrib.vertices[vertex_index]);
            interleaved.push_back(attrib.vertices[vertex_index + 1]);
            interleaved.push_back(attrib.vertices[vertex_index + 2]);

            // Extract normals if available
            if (!attrib.normals.empty() && index.normal_index >= 0) {
                int normal_index = 3 * index.normal_index;
                interleaved.push_back(attrib.normals[normal_index]);
                interleaved.push_back(attrib.normals[normal_index + 1]);
                interleaved.push_back(attrib.normals[normal_index + 2]);
            }
            else {
                // Fill with default normal if missing
                interleaved.push_back(0.0f);
                interleaved.push_back(0.0f);
                interleaved.push_back(0.0f);
            }

            // Store indices
            indices.push_back(indices.size());
        }
    }

    et::model model{ interleaved, indices };
    return model;
}