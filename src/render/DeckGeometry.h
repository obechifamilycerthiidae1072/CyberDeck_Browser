#pragma once

#include "render/NeonMaterial.h"

#include <vector>

namespace cyberdeck::render {

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct MeshFace {
    std::vector<int> indices;
};

struct DeckMesh {
    std::vector<Vec3> vertices;
    std::vector<MeshFace> faces;
};

enum class DeckShape {
    HexPrism,
    Cube,
    BeveledTile,
};

struct DeckTransform {
    Vec3 position;
    Vec3 rotation_degrees;
    Vec3 scale{1.0f, 1.0f, 1.0f};
};

struct DeckAnimation {
    bool enabled = true;
    float speed_scale = 1.0f;
    float idle_rotation_degrees_per_second = 0.0f;
    float orbit_radius = 0.0f;
    float orbit_degrees_per_second = 0.0f;
    float orbit_phase_degrees = 0.0f;
    float transition_response = 7.0f;
    float hover_pulse_hz = 0.8f;
    float hover_pulse_scale = 0.035f;
    float selected_pulse_hz = 0.55f;
    float selected_pulse_scale = 0.05f;
};

struct DeckAnimationState {
    bool initialized = false;
    Vec3 base_position;
    Vec3 target_position;
    Vec3 rendered_position;
    Vec3 rendered_scale{1.0f, 1.0f, 1.0f};
    float orbit_angle_degrees = 0.0f;
    float hover_phase_radians = 0.0f;
    float selected_phase_radians = 0.0f;
    float hover_pulse = 0.0f;
    float selected_pulse = 0.0f;
    float glow_pulse = 0.0f;
};

struct DeckSceneObject {
    DeckShape shape = DeckShape::HexPrism;
    DeckTransform transform;
    NeonMaterialId material = NeonMaterialId::NeonGreen;
    bool hovered = false;
    bool selected = false;
    DeckAnimation animation;
    DeckAnimationState animation_state;
};

DeckMesh GenerateCubeMesh(float size = 1.0f);
DeckMesh GenerateHexPrismMesh(float radius = 0.75f, float depth = 0.46f);
DeckMesh GenerateBeveledTileMesh(float width = 1.45f, float height = 0.9f, float depth = 0.22f, float bevel = 0.16f);

}  // namespace cyberdeck::render
