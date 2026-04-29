#include "render/DeckLayout.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

bool Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

bool Near(float actual, float expected, float epsilon = 0.001f) {
    return std::fabs(actual - expected) <= epsilon;
}

bool LayoutIsFiniteAndPositive(const std::vector<cyberdeck::render::DeckLayoutItem>& layout) {
    for (const auto& item : layout) {
        if (!std::isfinite(item.position.x) || !std::isfinite(item.position.y) || !std::isfinite(item.position.z)) {
            return false;
        }
        if (item.scale.x <= 0.0f || item.scale.y <= 0.0f || item.scale.z <= 0.0f) {
            return false;
        }
    }
    return true;
}

}  // namespace

int main() {
    bool passed = true;

    const auto empty = cyberdeck::render::BuildHexRingLayout(0);
    passed = Expect(empty.empty(), "Empty layout should have no items.") && passed;

    const auto single = cyberdeck::render::BuildHexRingLayout(1);
    passed = Expect(single.size() == 1, "Single-node layout should have one item.") && passed;
    if (!single.empty()) {
        passed = Expect(single[0].selected, "Single Node should be selected.") && passed;
        passed = Expect(Near(single[0].position.x, 0.0f), "Single Node should be centered on x.") && passed;
        passed = Expect(single[0].position.z > 0.0f, "Single Node should be in the front lane.") && passed;
        passed = Expect(single[0].scale.x > 1.0f, "Selected Node should be enlarged.") && passed;
    }

    const auto ring = cyberdeck::render::BuildHexRingLayout(6);
    passed = Expect(ring.size() == 6, "Ring layout should preserve Node count.") && passed;
    if (ring.size() == 6) {
        passed = Expect(ring[0].selected, "First ring Node should be the selected/front Node.") && passed;
        passed = Expect(ring[0].position.z > ring[3].position.z, "Selected Node should sit at the front of the ring.") &&
                 passed;
        passed = Expect(ring[0].scale.x > ring[1].scale.x, "Selected Node should be larger than neighbors.") && passed;
    }

    const auto orbit = cyberdeck::render::BuildCubeOrbitLayout(24);
    passed = Expect(orbit.size() == 24, "Cube Orbit should preserve 20+ Node count.") && passed;
    if (orbit.size() == 24) {
        passed = Expect(orbit[0].selected, "Cube Orbit should keep the first Node selected/front.") && passed;
        passed = Expect(orbit[12].position.y >= orbit[1].position.y, "Cube Orbit should use layered rings.") && passed;
    }

    const auto grid = cyberdeck::render::BuildGridDeckLayout(24);
    passed = Expect(grid.size() == 24, "Grid Deck should preserve 20+ Node count.") && passed;
    if (grid.size() == 24) {
        passed = Expect(grid[0].selected, "Grid Deck should keep the first Node selected.") && passed;
        passed = Expect(grid[0].position.z > grid.back().position.z, "Grid Deck should progress into rows.") && passed;
        passed = Expect(grid[0].rotation_degrees.x < 0.0f, "Grid Deck should tilt tiles toward the camera.") && passed;
    }

    const cyberdeck::render::DeckLayoutMode modes[] = {
        cyberdeck::render::DeckLayoutMode::HexRing,
        cyberdeck::render::DeckLayoutMode::CubeOrbit,
        cyberdeck::render::DeckLayoutMode::GridDeck,
    };
    for (const auto mode : modes) {
        const auto many = cyberdeck::render::BuildDeckLayout(mode, 100);
        passed = Expect(many.size() == 100, "Each layout should remain stable at 100 Nodes.") && passed;
        passed = Expect(LayoutIsFiniteAndPositive(many), "100 Node layout positions/scales should stay usable.") &&
                 passed;
    }

    passed = Expect(
                 cyberdeck::render::DeckLayoutModeFromString("grid") == cyberdeck::render::DeckLayoutMode::GridDeck,
                 "Legacy grid setting should map to Grid Deck.") &&
             passed;
    passed = Expect(
                 cyberdeck::render::NextLayoutMode(cyberdeck::render::DeckLayoutMode::GridDeck) ==
                     cyberdeck::render::DeckLayoutMode::HexRing,
                 "Layout cycling should wrap to Hex Ring.") &&
             passed;

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
