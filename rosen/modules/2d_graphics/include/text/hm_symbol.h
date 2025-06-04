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

#ifndef HM_SYMBOL_H
#define HM_SYMBOL_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "draw/path.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
typedef unsigned U8CPU;

enum DrawingAnimationType {
    INVALID_ANIMATION_TYPE = 0,
    SCALE_TYPE = 1,
    VARIABLE_COLOR_TYPE = 2,
    APPEAR_TYPE = 3,
    DISAPPEAR_TYPE = 4,
    BOUNCE_TYPE = 5,
    PULSE_TYPE = 6,
    REPLACE_APPEAR_TYPE = 7,
    REPLACE_DISAPPEAR_TYPE = 8,
    DISABLE_TYPE = 9,
    QUICK_REPLACE_APPEAR_TYPE = 10,
    QUICK_REPLACE_DISAPPEAR_TYPE = 11,
};

enum DrawingCurveType {
    INVALID_CURVE_TYPE = 0,
    SPRING = 1,
    LINEAR = 2,
    FRICTION = 3,
    SHARP = 4,
};

enum DrawingCommonSubType {
    DOWN = 0,
    UP = 1,
};

struct DrawingPiecewiseParameter {
    DrawingCurveType curveType = DrawingCurveType::INVALID_CURVE_TYPE;
    std::map<std::string, float> curveArgs;
    uint32_t duration = 0;
    int delay = 0;
    std::map<std::string, std::vector<float>> properties;
};

struct DrawingAnimationPara {
    uint16_t animationMode = 0; // 0 is default value, is byLayer effect
    DrawingCommonSubType commonSubType = DrawingCommonSubType::DOWN;
    std::vector<std::vector<DrawingPiecewiseParameter>> groupParameters;
};

struct DrawingAnimationInfo {
    DrawingAnimationType animationType = DrawingAnimationType::INVALID_ANIMATION_TYPE;
    std::map<uint32_t, DrawingAnimationPara> animationParas;
};

struct DrawingSColor {
    float a = 1.f;
    U8CPU r = 0;
    U8CPU g = 0;
    U8CPU b = 0;
};

struct DrawingGroupInfo {
    std::vector<size_t> layerIndexes;
    std::vector<size_t> maskIndexes;
};

struct DrawingGroupSetting {
    std::vector<DrawingGroupInfo> groupInfos;
    int animationIndex = -1; // -1 is default value, the level has no effecet
};

struct DrawingAnimationSetting {
    std::vector<DrawingAnimationType> animationTypes;
    std::vector<DrawingGroupSetting> groupSettings;
    double slope = 0.0;
    DrawingCommonSubType commonSubType = DrawingCommonSubType::DOWN;
};

struct DrawingRenderGroup {
    std::vector<DrawingGroupInfo> groupInfos;
    DrawingSColor color;
};

enum DrawingEffectStrategy {
    NONE = 0,
    SCALE = 1,
    VARIABLE_COLOR = 2,
    APPEAR = 3,
    DISAPPEAR = 4,
    BOUNCE = 5,
    PULSE = 6,
    REPLACE_APPEAR = 7,
    REPLACE_DISAPPEAR = 8,
    DISABLE = 9,
    QUICK_REPLACE_APPEAR = 10,
    QUICK_REPLACE_DISAPPEAR = 11,

    TEXT_FLIP = 100, // text type start from 100
};

struct DrawingSymbolLayers {
    uint16_t symbolGlyphId = 0;
    std::vector<std::vector<size_t>> layers;
    std::vector<DrawingRenderGroup> renderGroups;
};

enum DrawingSymbolRenderingStrategy {
    SINGLE = 0,
    MULTIPLE_COLOR = 1,
    MULTIPLE_OPACITY = 2,
};

struct DrawingSymbolLayersGroups {
    uint16_t symbolGlyphId = 0;
    std::vector<std::vector<size_t>> layers;
    std::map<DrawingSymbolRenderingStrategy, std::vector<DrawingRenderGroup>> renderModeGroups;
    std::vector<DrawingAnimationSetting> animationSettings;
};

struct DrawingHMSymbolData {
public:
    DrawingSymbolLayers symbolInfo_;
    Path path_;
    uint64_t symbolId = 0; // span id in paragraph
};

class DRAWING_API DrawingHMSymbol {
public:
    static void PathOutlineDecompose(const Path& path, std::vector<Path>& paths);

    static void MultilayerPath(const std::vector<std::vector<size_t>>& multMap,
        const std::vector<Path>& paths, std::vector<Path>& multPaths);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif