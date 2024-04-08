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
    REPLACE_DISAPPEAR_TYPE
};

enum DrawingCurveType {
    INVALID_CURVE_TYPE = 0,
    SPRING = 1,
    LINEAR = 2,
    FRICTION = 3,
    SHARP = 4,
};

enum DrawingCommonSubType {
    UP = 0,
    DOWN = 1,
};

struct DrawingPiecewiseParameter {
    DrawingCurveType curveType;
    std::map<std::string, double> curveArgs;
    uint32_t duration;
    int delay;
    std::map<std::string, std::vector<double>> properties;
};

struct DrawingAnimationPara {
    uint32_t animationMode = 0; // 0 is default value, is byLayer effect
    DrawingCommonSubType commonSubType = DrawingCommonSubType::UP;
    std::vector<std::vector<DrawingPiecewiseParameter>> groupParameters;
};

struct DrawingAnimationInfo {
    DrawingAnimationType animationType;
    std::vector<DrawingAnimationPara> animationParas;
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
    REPLACE_DISAPPEAR
};

struct DrawingSymbolLayers {
    uint32_t symbolGlyphId;
    std::vector<std::vector<size_t>> layers;
    std::vector<DrawingRenderGroup> renderGroups;
};

enum DrawingSymbolRenderingStrategy {
    SINGLE = 0,
    MULTIPLE_COLOR = 1,
    MULTIPLE_OPACITY = 2,
};

struct DrawingSymbolLayersGroups {
    uint32_t symbolGlyphId;
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