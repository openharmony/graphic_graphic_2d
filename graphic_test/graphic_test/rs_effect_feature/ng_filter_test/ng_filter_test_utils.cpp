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

#include "ng_filter_test_utils.h"

namespace OHOS::Rosen {

using MaskCreator = std::function<std::shared_ptr<RSNGMaskBase>()>;
using FilterCreator = std::function<std::shared_ptr<RSNGFilterBase>()>;

static std::unordered_map<RSNGEffectType, MaskCreator> creatorMask = {
    {RSNGEffectType::DOUBLE_RIPPLE_MASK,
     [] {
         return std::make_shared<RSNGDoubleRippleMask>();
     }}
};

static std::unordered_map<RSNGEffectType, FilterCreator> creatorFilter = {
    {RSNGEffectType::DISPLACEMENT_DISTORT,
     [] {
         return std::make_shared<RSNGDispDistortFilter>();
     }},
    {RSNGEffectType::COLOR_GRADIENT,
     [] {
         return std::make_shared<RSNGColorGradientFilter>();
     }},
    {RSNGEffectType::FROSTED_GLASS,
     [] {
         return std::make_shared<RSNGFrostedGlassFilter>();
     }},
    {RSNGEffectType::GRID_WARP,
     [] {
         return std::make_shared<RSNGGridWarpFilter>();
     }}
};

std::shared_ptr<RSNGMaskBase> CreateMask(RSNGEffectType type)
{
    auto it = creatorMask.find(type);
    return it != creatorMask.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGFilterBase> CreateFilter(RSNGEffectType type)
{
    auto it = creatorFilter.find(type);
    return it != creatorFilter.end() ? it->second() : nullptr;
}
}  // namespace OHOS::Rosen