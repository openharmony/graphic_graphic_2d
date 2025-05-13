/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROSEN_TEXT_SYMBOL_ANIMATION_CONFIG_H
#define ROSEN_TEXT_SYMBOL_ANIMATION_CONFIG_H

#include "draw/path.h"
#include "draw/color.h"
#include "text/hm_symbol.h"
#include "common/rs_vector4.h"


namespace OHOS {
namespace Rosen {
namespace TextEngine {

struct NodeLayerInfo {
    Drawing::Path path;
    Drawing::DrawingSColor color;
};

struct SymbolNode {
    std::vector<NodeLayerInfo> pathsInfo;
    Vector4f nodeBoundary;
    Drawing::DrawingHMSymbolData symbolData;
    int animationIndex = 0;
    bool isMask = false;
};

struct TextEffectElement {
    Drawing::Path path;
    Drawing::Point offset;
    uint64_t uniqueId = 0;
    float width = 0.0;
    float height = 0.0;
    int delay = 0;
};

struct SymbolAnimationConfig {
    std::vector<SymbolNode> symbolNodes;
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    TextEffectElement effectElement;
    Drawing::Color color;
    uint32_t numNodes = 0;
    Drawing::DrawingEffectStrategy effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    uint64_t symbolSpanId = 0;
    uint16_t animationMode = 0;
    int repeatCount = 1;
    bool animationStart = false;
    Drawing::DrawingCommonSubType commonSubType = Drawing::DrawingCommonSubType::DOWN;
    bool currentAnimationHasPlayed = false;
};
}
}
}


#endif