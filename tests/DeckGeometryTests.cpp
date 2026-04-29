#include "render/DeckGeometry.h"

#include <cstdlib>
#include <iostream>

namespace {

bool Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

bool FacesReferenceValidVertices(const cyberdeck::render::DeckMesh& mesh) {
    for (const cyberdeck::render::MeshFace& face : mesh.faces) {
        if (face.indices.size() < 3) {
            return false;
        }
        for (int index : face.indices) {
            if (index < 0 || index >= static_cast<int>(mesh.vertices.size())) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace

int main() {
    bool passed = true;

    const auto cube = cyberdeck::render::GenerateCubeMesh();
    passed = Expect(cube.vertices.size() == 8, "Cube should have 8 vertices.") && passed;
    passed = Expect(cube.faces.size() == 6, "Cube should have 6 faces.") && passed;
    passed = Expect(FacesReferenceValidVertices(cube), "Cube faces should reference valid vertices.") && passed;

    const auto hex = cyberdeck::render::GenerateHexPrismMesh();
    passed = Expect(hex.vertices.size() == 12, "Hex prism should have 12 vertices.") && passed;
    passed = Expect(hex.faces.size() == 8, "Hex prism should have 8 faces.") && passed;
    passed = Expect(FacesReferenceValidVertices(hex), "Hex prism faces should reference valid vertices.") && passed;

    const auto tile = cyberdeck::render::GenerateBeveledTileMesh();
    passed = Expect(tile.vertices.size() == 16, "Beveled tile should have 16 vertices.") && passed;
    passed = Expect(tile.faces.size() == 10, "Beveled tile should have 10 faces.") && passed;
    passed = Expect(FacesReferenceValidVertices(tile), "Beveled tile faces should reference valid vertices.") && passed;

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
