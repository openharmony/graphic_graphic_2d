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
    SCALE_EFFECT = 1,
    VARIABLE_COLOR = 2,
};

enum DrawingAnimationSubType {
    INVALID_ANIMATION_SUB_TYPE = 0,
    UNIT = 1,
    VARIABLE_3_GROUP = 2,
    VARIABLE_4_GROUP = 3,
};

enum DrawingCurveType {
    INVALID_CURVE_TYPE = 0,
    SPRING = 1,
    LINEAR = 2,
};

struct DrawingPiecewiseParameter {
    DrawingCurveType curveType;
    std::map<std::string, double> curveArgs;
    uint32_t duration;
    uint32_t delay;
    std::map<std::string, std::vector<double>> properties;
};

struct DrawingAnimationPara {
    uint32_t animationMode;
    DrawingAnimationSubType subType;
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
    uint32_t animationIndex;
};

struct DrawingAnimationSetting {
    DrawingAnimationType animationType;
    DrawingAnimationSubType animationSubType;
    uint32_t animationMode;
    std::vector<DrawingGroupSetting> groupSettings;
};

struct DrawingRenderGroup {
    std::vector<DrawingGroupInfo> groupInfos;
    DrawingSColor color;
};

enum DrawingEffectStrategy {
    INVALID_EFFECT_STRATEGY = 0,
    NONE = 1,
    SCALE = 2,
    HIERARCHICAL = 3,
};

struct DrawingSymbolLayers {
    uint32_t symbolGlyphId;
    std::vector<std::vector<size_t>> layers;
    std::vector<DrawingRenderGroup> renderGroups;
    DrawingEffectStrategy effect;
    DrawingAnimationSetting animationSetting;
};

enum DrawingSymbolRenderingStrategy {
    INVALID_RENDERING_STRATEGY = 0,
    SINGLE = 1,
    MULTIPLE_COLOR = 2,
    MULTIPLE_OPACITY = 3,
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