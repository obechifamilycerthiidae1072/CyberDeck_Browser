#pragma once

#include "render/DeckGeometry.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace cyberdeck::render {

enum class DeckLayoutMode {
    HexRing,
    CubeOrbit,
    GridDeck,
};

struct DeckLayoutItem {
    Vec3 position;
    Vec3 rotation_degrees;
    Vec3 scale{1.0f, 1.0f, 1.0f};
    bool selected = false;
    float front_factor = 0.0f;
};

const char* ToLayoutModeString(DeckLayoutMode mode);
DeckLayoutMode NextLayoutMode(DeckLayoutMode mode);
DeckLayoutMode DeckLayoutModeFromString(std::string_view value);
std::vector<DeckLayoutItem> BuildDeckLayout(DeckLayoutMode mode, std::size_t node_count);
std::vector<DeckLayoutItem> BuildHexRingLayout(std::size_t node_count);
std::vector<DeckLayoutItem> BuildCubeOrbitLayout(std::size_t node_count);
std::vector<DeckLayoutItem> BuildGridDeckLayout(std::size_t node_count);

}  // namespace cyberdeck::render
