#include "render/DeckLayout.h"

#include <algorithm>
#include <cmath>

namespace cyberdeck::render {
namespace {

constexpr float kPi = 3.14159265358979323846f;

float RingRadius(std::size_t node_count) {
    if (node_count <= 1) {
        return 0.0f;
    }
    return std::clamp(1.65f + static_cast<float>(node_count) * 0.055f, 2.2f, 7.0f);
}

std::size_t IntegerCeilSqrt(std::size_t value) {
    std::size_t side = 1;
    while (side * side < value) {
        ++side;
    }
    return side;
}

}  // namespace

const char* ToLayoutModeString(DeckLayoutMode mode) {
    switch (mode) {
        case DeckLayoutMode::HexRing:
            return "hex-ring";
        case DeckLayoutMode::CubeOrbit:
            return "cube-orbit";
        case DeckLayoutMode::GridDeck:
            return "grid-deck";
    }
    return "hex-ring";
}

DeckLayoutMode NextLayoutMode(DeckLayoutMode mode) {
    switch (mode) {
        case DeckLayoutMode::HexRing:
            return DeckLayoutMode::CubeOrbit;
        case DeckLayoutMode::CubeOrbit:
            return DeckLayoutMode::GridDeck;
        case DeckLayoutMode::GridDeck:
            return DeckLayoutMode::HexRing;
    }
    return DeckLayoutMode::HexRing;
}

DeckLayoutMode DeckLayoutModeFromString(std::string_view value) {
    if (value == "cube-orbit" || value == "cube") {
        return DeckLayoutMode::CubeOrbit;
    }
    if (value == "grid-deck" || value == "grid") {
        return DeckLayoutMode::GridDeck;
    }
    return DeckLayoutMode::HexRing;
}

std::vector<DeckLayoutItem> BuildDeckLayout(DeckLayoutMode mode, std::size_t node_count) {
    switch (mode) {
        case DeckLayoutMode::HexRing:
            return BuildHexRingLayout(node_count);
        case DeckLayoutMode::CubeOrbit:
            return BuildCubeOrbitLayout(node_count);
        case DeckLayoutMode::GridDeck:
            return BuildGridDeckLayout(node_count);
    }
    return BuildHexRingLayout(node_count);
}

std::vector<DeckLayoutItem> BuildHexRingLayout(std::size_t node_count) {
    std::vector<DeckLayoutItem> layout;
    layout.reserve(node_count);

    if (node_count == 0) {
        return layout;
    }

    const float radius = RingRadius(node_count);
    for (std::size_t index = 0; index < node_count; ++index) {
        const float angle = node_count == 1
                                ? kPi * 0.5f
                                : kPi * 0.5f + (static_cast<float>(index) / static_cast<float>(node_count)) * kPi * 2.0f;
        const float x = node_count == 1 ? 0.0f : std::cos(angle) * radius;
        const float z = node_count == 1 ? 1.15f : std::sin(angle) * radius;
        const float front_factor = (std::sin(angle) + 1.0f) * 0.5f;
        const bool selected = index == 0;
        const float scale = selected ? 1.28f : 0.68f + front_factor * 0.24f;

        layout.push_back({
            .position = {x, selected ? 0.28f : -0.05f + static_cast<float>(index % 5) * 0.035f, z},
            .rotation_degrees = {0.0f, -angle * 180.0f / kPi + 90.0f, 0.0f},
            .scale = {scale, scale, scale},
            .selected = selected,
            .front_factor = front_factor,
        });
    }

    return layout;
}

std::vector<DeckLayoutItem> BuildCubeOrbitLayout(std::size_t node_count) {
    std::vector<DeckLayoutItem> layout;
    layout.reserve(node_count);
    if (node_count == 0) {
        return layout;
    }

    for (std::size_t index = 0; index < node_count; ++index) {
        if (index == 0) {
            layout.push_back({
                .position = {0.0f, 0.36f, 1.45f},
                .rotation_degrees = {0.0f, 0.0f, 0.0f},
                .scale = {1.22f, 1.22f, 1.22f},
                .selected = true,
                .front_factor = 1.0f,
            });
            continue;
        }

        const std::size_t orbit_index = index - 1;
        const std::size_t layer = orbit_index / 12;
        const std::size_t slot = orbit_index % 12;
        const float radius = 2.1f + static_cast<float>(layer) * 1.35f;
        const float angle = kPi * 0.5f + (static_cast<float>(slot) / 12.0f) * kPi * 2.0f +
                            static_cast<float>(layer % 2) * (kPi / 12.0f);
        const float front_factor = (std::sin(angle) + 1.0f) * 0.5f;
        const float scale = 0.58f + front_factor * 0.26f - static_cast<float>(std::min<std::size_t>(layer, 4)) * 0.035f;

        layout.push_back({
            .position = {std::cos(angle) * radius, -0.1f + static_cast<float>(layer) * 0.42f, std::sin(angle) * radius},
            .rotation_degrees = {0.0f, -angle * 180.0f / kPi + 90.0f, 0.0f},
            .scale = {std::max(0.38f, scale), std::max(0.38f, scale), std::max(0.38f, scale)},
            .selected = false,
            .front_factor = front_factor,
        });
    }

    return layout;
}

std::vector<DeckLayoutItem> BuildGridDeckLayout(std::size_t node_count) {
    std::vector<DeckLayoutItem> layout;
    layout.reserve(node_count);
    if (node_count == 0) {
        return layout;
    }

    const std::size_t columns = std::clamp<std::size_t>(IntegerCeilSqrt(node_count), 4, 12);
    const float spacing_x = 1.38f;
    const float spacing_z = 1.02f;
    const float center = (static_cast<float>(columns) - 1.0f) * 0.5f;

    for (std::size_t index = 0; index < node_count; ++index) {
        const std::size_t row = index / columns;
        const std::size_t column = index % columns;
        const bool selected = index == 0;
        const float scale = selected ? 1.08f : 0.82f;

        layout.push_back({
            .position = {
                (static_cast<float>(column) - center) * spacing_x,
                selected ? 0.24f : -0.18f,
                1.15f - static_cast<float>(row) * spacing_z,
            },
            .rotation_degrees = {-62.0f, 0.0f, 0.0f},
            .scale = {scale, scale, scale},
            .selected = selected,
            .front_factor = selected ? 1.0f : std::max(0.0f, 1.0f - static_cast<float>(row) * 0.08f),
        });
    }

    return layout;
}

}  // namespace cyberdeck::render
