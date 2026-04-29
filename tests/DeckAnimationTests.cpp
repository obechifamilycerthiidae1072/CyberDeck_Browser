#include "render/DeckAnimation.h"

#include <cmath>
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

bool Near(float actual, float expected, float epsilon = 0.001f) {
    return std::fabs(actual - expected) <= epsilon;
}

}  // namespace

int main() {
    bool passed = true;

    cyberdeck::render::DeckSceneObject rotating{};
    rotating.animation.idle_rotation_degrees_per_second = 180.0f;
    cyberdeck::render::InitializeDeckAnimation(rotating);
    cyberdeck::render::AdvanceDeckAnimation(rotating, 0.25f);
    passed = Expect(Near(rotating.transform.rotation_degrees.y, 45.0f), "Idle rotation should scale with seconds.") &&
             passed;

    cyberdeck::render::DeckSceneObject orbiting{};
    orbiting.transform.position = {2.0f, 0.0f, -1.0f};
    orbiting.animation.orbit_radius = 0.5f;
    orbiting.animation.orbit_degrees_per_second = 360.0f;
    orbiting.animation.orbit_phase_degrees = 0.0f;
    orbiting.animation.transition_response = 0.0f;
    cyberdeck::render::InitializeDeckAnimation(orbiting);
    passed = Expect(Near(orbiting.animation_state.rendered_position.x, 2.5f), "Orbit should offset the initial x position.") &&
             passed;
    cyberdeck::render::AdvanceDeckAnimation(orbiting, 0.25f);
    passed = Expect(Near(orbiting.animation_state.rendered_position.x, 2.0f), "Orbit should move by delta-time angle.") &&
             passed;
    passed = Expect(Near(orbiting.animation_state.rendered_position.z, -0.5f), "Orbit should update z from the orbit angle.") &&
             passed;

    cyberdeck::render::DeckSceneObject selected{};
    selected.selected = true;
    selected.animation.selected_pulse_scale = 0.08f;
    selected.animation.selected_pulse_hz = 0.5f;
    cyberdeck::render::InitializeDeckAnimation(selected);
    cyberdeck::render::AdvanceDeckAnimation(selected, 0.25f);
    passed = Expect(selected.animation_state.glow_pulse > 0.0f, "Selected object should produce a glow pulse.") &&
             passed;
    passed = Expect(
                 selected.animation_state.rendered_scale.x > selected.transform.scale.x,
                 "Selected pulse should increase rendered scale.") &&
             passed;
    passed =
        Expect(selected.animation_state.glow_pulse <= selected.animation.selected_pulse_scale + 0.001f,
               "Selected glow pulse should stay within its configured scale.") &&
        passed;

    cyberdeck::render::DeckSceneObject disabled{};
    disabled.transform.position = {1.0f, 2.0f, 3.0f};
    disabled.animation.enabled = false;
    disabled.animation.idle_rotation_degrees_per_second = 90.0f;
    cyberdeck::render::InitializeDeckAnimation(disabled);
    cyberdeck::render::AdvanceDeckAnimation(disabled, 1.0f);
    passed = Expect(Near(disabled.transform.rotation_degrees.y, 0.0f), "Disabled animation should not rotate.") && passed;
    passed = Expect(
                 Near(disabled.animation_state.rendered_position.x, 1.0f) &&
                     Near(disabled.animation_state.rendered_position.y, 2.0f) &&
                     Near(disabled.animation_state.rendered_position.z, 3.0f),
                 "Disabled animation should render at the static transform.") &&
             passed;

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
