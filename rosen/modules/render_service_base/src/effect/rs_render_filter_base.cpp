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
#include "ge_visual_effect_container.h"

#include "effect/rs_render_mask_base.h"
#include "platform/common/rs_log.h"

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

void RSNGRenderFilterBase::Dump(std::string& out) const
{
    std::string descStr = ": ";
    std::string splitStr = ", ";

    out += RSNGRenderFilterHelper::GetFilterTypeString(GetType());
    out += descStr;
    DumpProperty(out);
    if (nextEffect_) {
        out += splitStr;
        nextEffect_->Dump(out);
    }
}

void RSNGRenderFilterHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, float value)
{
    geFilter->SetParam(desc, value);
}

void RSNGRenderFilterHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, const Vector4f& value)
{
    geFilter->SetParam(desc, value.x_);
}

void RSNGRenderFilterHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, const Vector2f& value)
{
    geFilter->SetParam(desc, std::make_pair(value.x_, value.y_));
}

void RSNGRenderFilterHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, std::shared_ptr<RSNGRenderMaskBase> value)
{
    geFilter->SetParam(desc, value->GenerateGEShaderMask());
}

std::shared_ptr<Drawing::GEVisualEffect> RSNGRenderFilterHelper::CreateGEFilter(RSNGEffectType type)
{
    return std::make_shared<Drawing::GEVisualEffect>(GetFilterTypeString(type), Drawing::DrawingPaintType::BRUSH);
}

void RSUIFilterHelper::UpdateToGEContainer(std::shared_ptr<RSNGRenderFilterBase> filter,
    std::shared_ptr<Drawing::GEVisualEffectContainer> container)
{
    if (!filter || !container) {
        RS_LOGE("RSUIFilterHelper::UpdateToGEContainer: filter or container nullptr");
        return;
    }

    std::for_each(filter->geFilters_.begin(), filter->geFilters_.end(), [&container](const auto& filter) {
        container->AddToChainedFilter(filter);
    });
}
} // namespace Rosen
} // namespace OHOS
