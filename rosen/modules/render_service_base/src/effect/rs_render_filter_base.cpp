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

#include "effect/rs_render_filter_base.h"

#include <unordered_map>

#include "ge_visual_effect.h"
#include "ge_visual_effect_impl.h"
#include "ge_visual_effect_container.h"

#include "effect/rs_render_mask_base.h"
#include "platform/common/rs_log.h"
#include "render/rs_effect_luminance_manager.h"

namespace OHOS {
namespace Rosen {
using FilterCreator = std::function<std::shared_ptr<RSNGRenderFilterBase>()>;

static std::unordered_map<RSNGEffectType, FilterCreator> creatorLUT = {
    {RSNGEffectType::BLUR, [] {
            return std::make_shared<RSNGRenderBlurFilter>();
        }
    },
    {RSNGEffectType::DISPLACEMENT_DISTORT, [] {
            return std::make_shared<RSNGRenderDispDistortFilter>();
        }
    },
    {RSNGEffectType::SOUND_WAVE, [] {
            return std::make_shared<RSNGRenderSoundWaveFilter>();
        }
    },
    {RSNGEffectType::DISPERSION, [] {
            return std::make_shared<RSNGRenderDispersionFilter>();
        }
    },
    {RSNGEffectType::EDGE_LIGHT, [] {
            return std::make_shared<RSNGRenderEdgeLightFilter>();
        }
    },
    {RSNGEffectType::DIRECTION_LIGHT, [] {
            return std::make_shared<RSNGRenderDirectionLightFilter>();
        }
    },
    {RSNGEffectType::COLOR_GRADIENT, [] {
            return std::make_shared<RSNGRenderColorGradientFilter>();
        }
    },
    {RSNGEffectType::MASK_TRANSITION, [] {
            return std::make_shared<RSNGRenderMaskTransitionFilter>();
        }
    },
    {RSNGEffectType::VARIABLE_RADIUS_BLUR, [] {
            return std::make_shared<RSNGRenderVariableRadiusBlurFilter>();
        }
    },
    {RSNGEffectType::BEZIER_WARP, [] {
            return std::make_shared<RSNGRenderBezierWarpFilter>();
        }
    }
};

std::shared_ptr<RSNGRenderFilterBase> RSNGRenderFilterBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

[[nodiscard]] bool RSNGRenderFilterBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderFilterBase>& val)
{
    std::shared_ptr<RSNGRenderFilterBase> head = nullptr;
    auto current = head;
    for (size_t filterCount = 0; filterCount <= EFFECT_COUNT_LIMIT; ++filterCount) {
        RSNGEffectTypeUnderlying type = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            ROSEN_LOGE("RSNGRenderFilterBase: Unmarshalling type failed");
            return false;
        }

        if (type == END_OF_CHAIN) {
            val = head;
            return true;
        }

        auto filter = Create(static_cast<RSNGEffectType>(type));
        if (filter && !filter->OnUnmarshalling(parcel)) {
            ROSEN_LOGE("RSNGRenderFilterBase: Unmarshalling filter failed with type %{public}d", type);
            return false;
        }
        if (!current) {
            head = filter; // init head
        } else {
            current->nextEffect_ = filter;
        }
        current = filter;
    }

    ROSEN_LOGE("RSNGRenderFilterBase: UnMarshalling filter count arrive limit(%{public}zu)",
        EFFECT_COUNT_LIMIT);
    return false;
}

void RSNGRenderFilterBase::UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect> src,
                                           std::shared_ptr<Drawing::GEVisualEffect> dest)
{
    RSUIFilterHelper::UpdateCacheData(src, dest);
}

void RSUIFilterHelper::GenerateGEVisualEffect(std::shared_ptr<RSNGRenderFilterBase>& filter)
{
    if (filter) {
        filter->GenerateGEVisualEffect();
    }
}

void RSUIFilterHelper::UpdateToGEContainer(std::shared_ptr<RSNGRenderFilterBase>& filter,
    std::shared_ptr<Drawing::GEVisualEffectContainer>& container)
{
    if (!container) {
        RS_LOGE("RSUIFilterHelper::UpdateToGEContainer: container nullptr");
        return;
    }

    auto current = filter;
    while (current) {
        container->AddToChainedFilter(current->geFilter_);
        current = current->nextEffect_;
    }
}

bool RSUIFilterHelper::CheckEnableEDR(std::shared_ptr<RSNGRenderFilterBase> filter)
{
    auto current = filter;
    while (current) {
        if (RSEffectLuminanceManager::GetEnableHdrEffect(current)) {
            return true;
        }
        current = current->nextEffect_;
    }
    return false;
}

void RSUIFilterHelper::UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect> src,
                                       std::shared_ptr<Drawing::GEVisualEffect> dest)
{
    if (src == nullptr) {
        RS_LOGE("RSUIFilterHelper::UpdateCacheData: src is nullptr");
        return;
    }
    if (dest == nullptr) {
        RS_LOGE("RSUIFilterHelper::UpdateCacheData: dest is nullptr");
        return;
    }
    auto srcImpl = src->GetImpl();
    auto destImpl = dest->GetImpl();
    if (srcImpl == nullptr || destImpl == nullptr || srcImpl->GetFilterType() != destImpl->GetFilterType()) {
        RS_LOGE("RSUIFilterHelper::UpdateCacheData: effect impl type mismatch");
        return;
    }

    auto cache = srcImpl->GetCache();
    if (cache != nullptr) {
        destImpl->SetCache(cache);
    }
}

} // namespace Rosen
} // namespace OHOS
