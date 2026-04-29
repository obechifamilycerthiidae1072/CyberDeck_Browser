#include "render/NeonMaterial.h"

#include <windows.h>
#include <gl/GL.h>

#include <algorithm>

namespace cyberdeck::render {
namespace {

constexpr NeonMaterial kNeonGreen{
    "neon-green",
    0.0f,
    1.0f,
    0.0f,
    1.0f,
    0.0f,
    1.0f,
    0.0f,
    0.28f,
    2.0f,
};
constexpr NeonMaterial kYellowHighlight{
    "yellow-highlight",
    1.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.92f,
    0.0f,
    0.22f,
    2.0f,
};
constexpr NeonMaterial kRedDanger{
    "red-danger",
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    0.0f,
    0.25f,
    2.0f,
};
constexpr NeonMaterial kDimInactiveGreen{
    "dim-inactive-green",
    0.0f,
    0.32f,
    0.12f,
    1.0f,
    0.0f,
    0.5f,
    0.18f,
    0.12f,
    1.0f,
};
constexpr NeonMaterial kVoidBlack{
    "void-black",
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
};

float Clamp01(float value) {
    return std::clamp(value, 0.0f, 1.0f);
}

}  // namespace

const NeonMaterial& Material(NeonMaterialId id) {
    switch (id) {
        case NeonMaterialId::NeonGreen:
            return kNeonGreen;
        case NeonMaterialId::YellowHighlight:
            return kYellowHighlight;
        case NeonMaterialId::RedDanger:
            return kRedDanger;
        case NeonMaterialId::DimInactiveGreen:
            return kDimInactiveGreen;
        case NeonMaterialId::VoidBlack:
        default:
            return kVoidBlack;
    }
}

void ApplyMaterial(NeonMaterialId id, float intensity, float alpha) {
    const NeonMaterial& material = Material(id);
    glColor4f(
        Clamp01(material.red * intensity),
        Clamp01(material.green * intensity),
        Clamp01(material.blue * intensity),
        Clamp01(material.alpha * alpha));
}

void ApplyLineMaterial(NeonMaterialId id, float intensity, float alpha) {
    const NeonMaterial& material = Material(id);
    glLineWidth(material.line_width);
    ApplyMaterial(id, intensity, alpha);
}

void ApplyHaloMaterial(NeonMaterialId id, float alpha_scale) {
    const NeonMaterial& material = Material(id);
    glColor4f(
        Clamp01(material.halo_red),
        Clamp01(material.halo_green),
        Clamp01(material.halo_blue),
        Clamp01(material.halo_alpha * alpha_scale));
}

}  // namespace cyberdeck::render
