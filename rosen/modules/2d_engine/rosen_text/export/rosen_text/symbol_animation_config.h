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
#include "text/hm_symbol.h"
#include "common/rs_vector4.h"


namespace OHOS {
namespace Rosen {
namespace TextEngine {


enum SymbolAnimationEffectStrategy {
    SYMBOL_NONE = 0,
    SYMBOL_SCALE = 1,
    SYMBOL_VARIABLE_COLOR = 2,
    SYMBOL_APPEAR = 3,
    SYMBOL_DISAPPEAR = 4,
    SYMBOL_BOUNCE = 5,
    SYMBOL_PULSE = 6,
    SYMBOL_REPLACE_APPEAR = 7
};

using SymbolNode = struct SymbolNode {
    Drawing::Path path;
    Drawing::DrawingSColor color;
    Vector4f nodeBoundary;
    Drawing::DrawingHMSymbolData symbolData;
    int animationIndex = 0;
    bool isMask = false;
};
using SymbolAnimationConfig = struct SymbolAnimationConfig {
    std::vector<SymbolNode> SymbolNodes;
    uint32_t numNodes = 0;
    SymbolAnimationEffectStrategy effectStrategy = SymbolAnimationEffectStrategy::SYMBOL_NONE;
    uint64_t symbolSpanId = 0;
    uint16_t animationMode = 0;
    int repeatCount = 1;
    bool aminationStart = false;
    Drawing::DrawingCommonSubType commonSubType = Drawing::DrawingCommonSubType::UP;
};
}
}
}


#endif