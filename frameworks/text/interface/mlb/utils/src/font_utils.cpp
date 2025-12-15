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

#include "font_utils.h"
#include "typography_types.h"

namespace OHOS::MLB {
namespace {
    const std::unordered_map<int, int> g_weightMap = {
        {100, static_cast<int>(Rosen::FontWeight::W100)},
        {200, static_cast<int>(Rosen::FontWeight::W200)},
        {300, static_cast<int>(Rosen::FontWeight::W300)},
        {400, static_cast<int>(Rosen::FontWeight::W400)},
        {500, static_cast<int>(Rosen::FontWeight::W500)},
        {600, static_cast<int>(Rosen::FontWeight::W600)},
        {700, static_cast<int>(Rosen::FontWeight::W700)},
        {800, static_cast<int>(Rosen::FontWeight::W800)},
        {900, static_cast<int>(Rosen::FontWeight::W900)}
    };
}

bool FindFontWeight(int weight, std::pair<int32_t, int32_t>& result)
{
    auto iter = g_weightMap.find(weight);
    if (iter != g_weightMap.end()) {
        result.first = iter->first;
        result.second = iter->second;
        return true;
    }
    return false;
}

int RegularWeight(int weight)
{
    constexpr int minFontWeight = 100;
    constexpr int maxFontWeight = 900;
    constexpr int weightStep = 100;
    constexpr int roundingHalfStep = 50;
    int clampWeight = std::max(minFontWeight, std::min(maxFontWeight, weight));
    int roundedWeight = (clampWeight + roundingHalfStep) / weightStep * weightStep;
    return std::min(maxFontWeight, roundedWeight);
}
} // namespace OHOS::MLB
