#include "render/DeckAnimation.h"

#include <algorithm>
#include <cmath>

namespace cyberdeck::render {
namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = kPi * 2.0f;
constexpr float kMaximumDeltaSeconds = 0.25f;

Vec3 Add(Vec3 left, Vec3 right) {
    return {left.x + right.x, left.y + right.y, left.z + right.z};
}

Vec3 Scale(Vec3 value, float amount) {
    return {value.x * amount, value.y * amount, value.z * amount};
}

Vec3 Lerp(Vec3 current, Vec3 target, float amount) {
    return {
        current.x + (target.x - current.x) * amount,
        current.y + (target.y - current.y) * amount,
        current.z + (target.z - current.z) * amount,
    };
}

float DegreesToRadians(float degrees) {
    return degrees * kPi / 180.0f;
}

float Pulse01(float phase_radians) {
    return 0.5f + 0.5f * std::sin(phase_radians);
}

float ResponseBlend(float response, float delta_seconds) {
    if (response <= 0.0f) {
        return 1.0f;
    }
    return std::clamp(1.0f - std::exp(-response * delta_seconds), 0.0f, 1.0f);
}

float WrapDegrees(float degrees) {
    while (degrees >= 360.0f) {
        degrees -= 360.0f;
    }
    while (degrees <= -360.0f) {
        degrees += 360.0f;
    }
    return degrees;
}

Vec3 OrbitPosition(const DeckSceneObject& object, float orbit_angle_degrees) {
    if (object.animation.orbit_radius == 0.0f) {
        return object.animation_state.base_position;
    }

    const float radians = DegreesToRadians(orbit_angle_degrees);
    return Add(
        object.animation_state.base_position,
        {std::cos(radians) * object.animation.orbit_radius, 0.0f, std::sin(radians) * object.animation.orbit_radius});
}

void SyncStaticState(DeckSceneObject& object) {
    object.animation_state.base_position = object.transform.position;
    object.animation_state.target_position = object.transform.position;
    object.animation_state.rendered_position = object.transform.position;
    object.animation_state.rendered_scale = object.transform.scale;
    object.animation_state.hover_pulse = 0.0f;
    object.animation_state.selected_pulse = 0.0f;
    object.animation_state.glow_pulse = 0.0f;
    object.animation_state.initialized = true;
}

}  // namespace

void InitializeDeckAnimation(DeckSceneObject& object) {
    object.animation_state.initialized = true;
    object.animation_state.base_position = object.transform.position;
    object.animation_state.orbit_angle_degrees = object.animation.orbit_phase_degrees;
    object.animation_state.hover_phase_radians = 0.0f;
    object.animation_state.selected_phase_radians = kPi * 0.35f;
    object.animation_state.hover_pulse = 0.0f;
    object.animation_state.selected_pulse = 0.0f;
    object.animation_state.glow_pulse = 0.0f;

    if (!object.animation.enabled) {
        SyncStaticState(object);
        return;
    }

    object.animation_state.target_position = OrbitPosition(object, object.animation_state.orbit_angle_degrees);
    object.animation_state.rendered_position = object.animation_state.target_position;
    object.animation_state.rendered_scale = object.transform.scale;
}

void AdvanceDeckAnimation(DeckSceneObject& object, float delta_seconds) {
    if (!object.animation_state.initialized) {
        InitializeDeckAnimation(object);
    }

    if (!object.animation.enabled || object.animation.speed_scale <= 0.0f) {
        SyncStaticState(object);
        return;
    }

    const float scaled_delta =
        std::clamp(delta_seconds, 0.0f, kMaximumDeltaSeconds) * std::max(0.0f, object.animation.speed_scale);

    object.transform.rotation_degrees.y = WrapDegrees(
        object.transform.rotation_degrees.y + object.animation.idle_rotation_degrees_per_second * scaled_delta);

    object.animation_state.orbit_angle_degrees = WrapDegrees(
        object.animation_state.orbit_angle_degrees + object.animation.orbit_degrees_per_second * scaled_delta);
    object.animation_state.target_position = OrbitPosition(object, object.animation_state.orbit_angle_degrees);
    object.animation_state.rendered_position = Lerp(
        object.animation_state.rendered_position,
        object.animation_state.target_position,
        ResponseBlend(object.animation.transition_response, scaled_delta));

    object.animation_state.hover_phase_radians += object.animation.hover_pulse_hz * kTwoPi * scaled_delta;
    object.animation_state.selected_phase_radians += object.animation.selected_pulse_hz * kTwoPi * scaled_delta;

    const float hover_target = object.hovered ? Pulse01(object.animation_state.hover_phase_radians) *
                                                    std::max(0.0f, object.animation.hover_pulse_scale)
                                              : 0.0f;
    const float selected_target = object.selected ? Pulse01(object.animation_state.selected_phase_radians) *
                                                       std::max(0.0f, object.animation.selected_pulse_scale)
                                                 : 0.0f;
    const float pulse_blend = ResponseBlend(12.0f, scaled_delta);
    object.animation_state.hover_pulse =
        object.animation_state.hover_pulse + (hover_target - object.animation_state.hover_pulse) * pulse_blend;
    object.animation_state.selected_pulse =
        object.animation_state.selected_pulse + (selected_target - object.animation_state.selected_pulse) * pulse_blend;
    object.animation_state.glow_pulse = object.animation_state.hover_pulse + object.animation_state.selected_pulse;
    object.animation_state.rendered_scale = Scale(object.transform.scale, 1.0f + object.animation_state.glow_pulse);
}

}  // namespace cyberdeck::render
