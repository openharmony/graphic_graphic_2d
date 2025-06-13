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

#include "render/rs_render_filter_base.h"

#include <unordered_map>

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "platform/common/rs_log.h"
#include "render/rs_render_color_gradient_filter.h"
#include "render/rs_render_mask_base.h"

namespace OHOS {
namespace Rosen {
using FilterCreator = std::function<std::shared_ptr<RSNGRenderFilterBase>()>;

static std::unordered_map<RSUIFilterType, FilterCreator> creatorLUT = {
    {RSUIFilterType::BLUR, [] {
            return std::make_shared<RSNGRenderBlurFilter>();
        }
    },
    {RSUIFilterType::DISPLACEMENT_DISTORT, [] {
            return std::make_shared<RSNGRenderDispDistortFilter>();
        }
    },
    {RSUIFilterType::SOUND_WAVE, [] {
            return std::make_shared<RSNGRenderSoundWaveFilter>();
        }
    },
    {RSUIFilterType::DISPERSION, [] {
            return std::make_shared<RSNGRenderDispersionFilter>();
        }
    },
    {RSUIFilterType::EDGE_LIGHT, [] {
            return std::make_shared<RSNGRenderEdgeLightFilter>();
        }
    },
};

std::shared_ptr<RSNGRenderFilterBase> RSNGRenderFilterBase::Create(RSUIFilterType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

[[nodiscard]] bool RSNGRenderFilterBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderFilterBase>& val)
{
    std::shared_ptr<RSNGRenderFilterBase> head = nullptr;
    auto current = head;
    for (size_t filterCount = 0; filterCount <= EFFECT_COUNT_LIMIT; ++filterCount) {
        RSUIFilterTypeUnderlying type = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            ROSEN_LOGE("RSNGRenderFilterBase: Unmarshalling type failed");
            return false;
        }

        if (type == END_OF_CHAIN) {
            val = head;
            return true;
        }

        auto filter = Create(static_cast<RSUIFilterType>(type));
        if (filter && !filter->OnUnmarshalling(parcel)) {
            ROSEN_LOGE("RSNGRenderFilterBase: Unmarshalling filter failed with type %{public}d", type);
            return false;
        }
        if (!current) {
            head = filter; // init head
        } else {
            current->SetNextEffect(filter);
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

std::shared_ptr<Drawing::GEVisualEffect> RSNGRenderFilterHelper::CreateGEFilter(RSUIFilterType type)
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

    RSUIFilterType RSRenderFilterParaBase::GetType() const
    {
        return type_;
    }

    bool RSRenderFilterParaBase::IsValid() const
    {
        return type_ != RSUIFilterType::NONE;
    }

    void RSRenderFilterParaBase::Dump(std::string& out) const
    {
        GetDescription(out);
        out += ": [";
        std::string splitStr = "] ";
        char buffer[UINT8_MAX] = { 0 };
        for (const auto& [key, value] : properties_) {
            if (sprintf_s(buffer, UINT8_MAX, "[Type:%d Value:", static_cast<int>(key))) {
                out.append(buffer);
            }
            if (value) {
                value->Dump(out);
            } else {
                out += "nullptr";
            }
            out += splitStr;
        }
    }

    bool RSRenderFilterParaBase::WriteToParcel(Parcel& parcel)
    {
        return true;
    }

    bool RSRenderFilterParaBase::ReadFromParcel(Parcel& parcel)
    {
        return true;
    }

    std::shared_ptr<RSRenderPropertyBase> RSRenderFilterParaBase::GetRenderProperty(RSUIFilterType type) const
    {
        auto it = properties_.find(type);
        if (it != properties_.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderFilterParaBase::GetLeafRenderProperties()
    {
        return {};
    }

} // namespace Rosen
} // namespace OHOS
