#pragma once

#include "render/DeckGeometry.h"

namespace cyberdeck::render {

void InitializeDeckAnimation(DeckSceneObject& object);
void AdvanceDeckAnimation(DeckSceneObject& object, float delta_seconds);

}  // namespace cyberdeck::render
