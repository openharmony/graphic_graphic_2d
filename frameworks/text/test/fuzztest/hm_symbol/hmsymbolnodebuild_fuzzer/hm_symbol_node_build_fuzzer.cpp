/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "hm_symbol_node_build_fuzzer.h"

#include <cstddef>
#include <fuzzer/FuzzedDataProvider.h>

#include "symbol_engine/hm_symbol_node_build.h"

namespace OHOS {
namespace Rosen {

void HmSymbolNodeBuildFuzzTestInnerSetAttributes(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    float offsetX = fdp.ConsumeFloatingPoint<float>();
    float offsetY = fdp.ConsumeFloatingPoint<float>();
    std::pair<float, float> offset = {offsetX, offsetY};

    int pathX1 = fdp.ConsumeIntegral<uint16_t>();
    int pathY1 = fdp.ConsumeIntegral<uint16_t>();
    int radius1 = fdp.ConsumeIntegral<uint8_t>();
    int pathX2 = fdp.ConsumeIntegral<uint16_t>();
    int pathY2 = fdp.ConsumeIntegral<uint16_t>();
    int radius2 = fdp.ConsumeIntegral<uint8_t>();
    RSPath path;
    path.AddCircle(pathX1, pathY1, radius1);
    path.AddCircle(pathX2, pathY2, radius2, Drawing::PathDirection::CCW_DIRECTION);

    RSHMSymbolData symbolData;
    symbolData.path_ = path;
    symbolData.symbolInfo_.layers = {{0}, {1}};
    symbolData.symbolInfo_.renderGroups = {{{{{0, 1}}}}};
    RSEffectStrategy effectStrategy = static_cast<RSEffectStrategy>(fdp.ConsumeIntegral<uint8_t>());

    Drawing::DrawingAnimationSetting animationSettingOne = {
        // animationTypes
        {
            Drawing::DrawingAnimationType::SCALE_TYPE,
            Drawing::DrawingAnimationType::APPEAR_TYPE,
            Drawing::DrawingAnimationType::DISAPPEAR_TYPE,
            Drawing::DrawingAnimationType::BOUNCE_TYPE
        },
        // groupSettings
        {
            // {0, 1}: layerIndes, 0: animationIndex
            {{{{0, 1}}}, 0}
        }
    };

    auto symbolNodeBuild = SPText::SymbolNodeBuild(animationSettingOne, symbolData, effectStrategy, offset);

    uint16_t animationMode = fdp.ConsumeIntegral<uint16_t>();
    symbolNodeBuild.SetAnimationMode(animationMode);

    int repeatCount = fdp.ConsumeIntegral<int>();
    symbolNodeBuild.SetRepeatCount(repeatCount);

    bool animationStart = fdp.ConsumeBool();
    symbolNodeBuild.SetAnimationStart(animationStart);

    bool currentAnimationHasPlayed = fdp.ConsumeBool();
    symbolNodeBuild.SetCurrentAnimationHasPlayed(currentAnimationHasPlayed);

    uint16_t commonSubType = fdp.ConsumeIntegral<uint16_t>();
    constexpr int enumSubTypeRange = 2;
    symbolNodeBuild.SetCommonSubType(static_cast<Drawing::DrawingCommonSubType>(commonSubType % enumSubTypeRange));
}

bool SetSymbolAnimationOne(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig == nullptr) {
        return false;
    }

    // check the size is 1 of nodes config
    if (symbolAnimationConfig->numNodes == 1 && symbolAnimationConfig->symbolNodes.size() == 1) {
        return true;
    }
    return false;
}

void HmSymbolNodeBuildFuzzTestInnerDecomposeSymbol(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    float offsetX = fdp.ConsumeFloatingPoint<float>();
    float offsetY = fdp.ConsumeFloatingPoint<float>();
    std::pair<float, float> offset = {offsetX, offsetY};

    int pathX1 = fdp.ConsumeIntegral<uint16_t>();
    int pathY1 = fdp.ConsumeIntegral<uint16_t>();
    int radius1 = fdp.ConsumeIntegral<uint8_t>();
    int pathX2 = fdp.ConsumeIntegral<uint16_t>();
    int pathY2 = fdp.ConsumeIntegral<uint16_t>();
    int radius2 = fdp.ConsumeIntegral<uint8_t>();
    RSPath path;
    path.AddCircle(pathX1, pathY1, radius1);
    path.AddCircle(pathX2, pathY2, radius2, Drawing::PathDirection::CCW_DIRECTION);

    RSHMSymbolData symbolData;
    symbolData.path_ = path;
    symbolData.symbolInfo_.layers = {{0}, {1}};
    symbolData.symbolInfo_.renderGroups = {{{{{0, 1}}}}};
    RSEffectStrategy effectStrategy = static_cast<RSEffectStrategy>(fdp.ConsumeIntegral<uint8_t>());

    Drawing::DrawingAnimationSetting animationSettingOne = {
        // animationTypes
        {
            Drawing::DrawingAnimationType::SCALE_TYPE,
            Drawing::DrawingAnimationType::APPEAR_TYPE,
            Drawing::DrawingAnimationType::DISAPPEAR_TYPE,
            Drawing::DrawingAnimationType::BOUNCE_TYPE
        },
        // groupSettings
        {
            // {0, 1}: layerIndes, 0: animationIndex
            {{{{0, 1}}}, 0}
        }
    };

    auto symbolNodeBuild1 = SPText::SymbolNodeBuild(animationSettingOne, symbolData, effectStrategy, offset);
    symbolNodeBuild1.SetAnimation(&SetSymbolAnimationOne);

    uint64_t symbolId = fdp.ConsumeIntegral<uint64_t>();
    symbolNodeBuild1.SetSymbolId(symbolId);
    uint16_t animationMode = fdp.ConsumeIntegral<uint16_t>();
    symbolNodeBuild1.SetAnimationMode(animationMode);
    symbolNodeBuild1.DecomposeSymbolAndDraw();

    symbolData.symbolInfo_.renderGroups = {};
    auto symbolNodeBuild2 = SPText::SymbolNodeBuild(animationSettingOne, symbolData, effectStrategy, offset);
    symbolNodeBuild2.DecomposeSymbolAndDraw();

    std::vector<Drawing::DrawingRenderGroup> renderGroupsOneMask = {{{{{0, 2}, {1, 3}}}}};
    symbolData.symbolInfo_.renderGroups = renderGroupsOneMask;
    auto symbolNodeBuild3 = SPText::SymbolNodeBuild(animationSettingOne, symbolData, effectStrategy, offset);
    symbolNodeBuild3.DecomposeSymbolAndDraw();
    symbolNodeBuild3.DecomposeSymbolAndDraw();

    symbolNodeBuild1.ClearAnimation();
}

bool HmSymbolNodeBuildFuzzTest01(const uint8_t* data, size_t size)
{
    // test interface functins of symbol
    HmSymbolNodeBuildFuzzTestInnerSetAttributes(data, size);
    HmSymbolNodeBuildFuzzTestInnerDecomposeSymbol(data, size);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::HmSymbolNodeBuildFuzzTest01(data, size);
    return 0;
}