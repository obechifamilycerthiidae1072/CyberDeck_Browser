#pragma once

namespace cyberdeck::render {

enum class NeonMaterialId {
    NeonGreen,
    YellowHighlight,
    RedDanger,
    DimInactiveGreen,
    VoidBlack,
};

struct NeonMaterial {
    const char* name = "";
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
    float alpha = 1.0f;
    float halo_red = 0.0f;
    float halo_green = 0.0f;
    float halo_blue = 0.0f;
    float halo_alpha = 0.25f;
    float line_width = 1.0f;
};

const NeonMaterial& Material(NeonMaterialId id);
void ApplyMaterial(NeonMaterialId id, float intensity = 1.0f, float alpha = 1.0f);
void ApplyLineMaterial(NeonMaterialId id, float intensity = 1.0f, float alpha = 1.0f);
void ApplyHaloMaterial(NeonMaterialId id, float alpha_scale = 1.0f);

}  // namespace cyberdeck::render
