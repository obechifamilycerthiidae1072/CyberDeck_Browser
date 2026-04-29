#include "render/DeckGeometry.h"

#include <algorithm>
#include <cmath>
#include <initializer_list>

namespace cyberdeck::render {
namespace {

constexpr float kPi = 3.14159265358979323846f;

MeshFace Face(std::initializer_list<int> indices) {
    return MeshFace{std::vector<int>(indices)};
}

}  // namespace

DeckMesh GenerateCubeMesh(float size) {
    const float h = size * 0.5f;
    DeckMesh mesh;
    mesh.vertices = {
        {-h, -h, h},
        {h, -h, h},
        {h, h, h},
        {-h, h, h},
        {-h, -h, -h},
        {h, -h, -h},
        {h, h, -h},
        {-h, h, -h},
    };
    mesh.faces = {
        Face({0, 1, 2, 3}),
        Face({5, 4, 7, 6}),
        Face({4, 0, 3, 7}),
        Face({1, 5, 6, 2}),
        Face({3, 2, 6, 7}),
        Face({4, 5, 1, 0}),
    };
    return mesh;
}

DeckMesh GenerateHexPrismMesh(float radius, float depth) {
    const float half_depth = depth * 0.5f;
    DeckMesh mesh;
    mesh.vertices.reserve(12);
    for (int i = 0; i < 6; ++i) {
        const float angle = kPi / 6.0f + static_cast<float>(i) * kPi / 3.0f;
        mesh.vertices.push_back({std::cos(angle) * radius, std::sin(angle) * radius, half_depth});
    }
    for (int i = 0; i < 6; ++i) {
        const float angle = kPi / 6.0f + static_cast<float>(i) * kPi / 3.0f;
        mesh.vertices.push_back({std::cos(angle) * radius, std::sin(angle) * radius, -half_depth});
    }

    mesh.faces.push_back(Face({0, 1, 2, 3, 4, 5}));
    mesh.faces.push_back(Face({11, 10, 9, 8, 7, 6}));
    for (int i = 0; i < 6; ++i) {
        const int next = (i + 1) % 6;
        mesh.faces.push_back(Face({i, next, next + 6, i + 6}));
    }
    return mesh;
}

DeckMesh GenerateBeveledTileMesh(float width, float height, float depth, float bevel) {
    const float half_width = width * 0.5f;
    const float half_height = height * 0.5f;
    const float half_depth = depth * 0.5f;
    const float b = std::min(bevel, std::min(half_width, half_height) * 0.75f);

    const Vec3 front[] = {
        {-half_width + b, -half_height, half_depth},
        {half_width - b, -half_height, half_depth},
        {half_width, -half_height + b, half_depth},
        {half_width, half_height - b, half_depth},
        {half_width - b, half_height, half_depth},
        {-half_width + b, half_height, half_depth},
        {-half_width, half_height - b, half_depth},
        {-half_width, -half_height + b, half_depth},
    };

    DeckMesh mesh;
    mesh.vertices.reserve(16);
    for (const Vec3& vertex : front) {
        mesh.vertices.push_back(vertex);
    }
    for (const Vec3& vertex : front) {
        mesh.vertices.push_back({vertex.x, vertex.y, -half_depth});
    }

    mesh.faces.push_back(Face({0, 1, 2, 3, 4, 5, 6, 7}));
    mesh.faces.push_back(Face({15, 14, 13, 12, 11, 10, 9, 8}));
    for (int i = 0; i < 8; ++i) {
        const int next = (i + 1) % 8;
        mesh.faces.push_back(Face({i, next, next + 8, i + 8}));
    }
    return mesh;
}

}  // namespace cyberdeck::render
