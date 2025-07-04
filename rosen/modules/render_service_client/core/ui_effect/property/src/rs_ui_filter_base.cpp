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

#include "ui_effect/property/include/rs_ui_filter_base.h"

#include "platform/common/rs_log.h"
#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/filter/include/filter_color_gradient_para.h"
#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "ui_effect/filter/include/filter_edge_light_para.h"

#undef LOG_TAG
#define LOG_TAG "RSNGFilterBase"

namespace OHOS {
namespace Rosen {
using FilterCreator = std::function<std::shared_ptr<RSNGFilterBase>()>;
using FilterConvertor = std::function<std::shared_ptr<RSNGFilterBase>(std::shared_ptr<FilterPara>)>;

static std::unordered_map<RSNGEffectType, FilterCreator> creatorLUT = {
    {RSNGEffectType::BLUR, [] {
            return std::make_shared<RSNGBlurFilter>();
        }
    },
    {RSNGEffectType::DISPLACEMENT_DISTORT, [] {
            return std::make_shared<RSNGDispDistortFilter>();
        }
    },
    {RSNGEffectType::SOUND_WAVE, [] {
            return std::make_shared<RSNGSoundWaveFilter>();
        }
    },
    {RSNGEffectType::DISPERSION, [] {
            return std::make_shared<RSNGDispersionFilter>();
        }
    },
    {RSNGEffectType::EDGE_LIGHT, [] {
            return std::make_shared<RSNGEdgeLightFilter>();
        }
    },
};

namespace {
std::shared_ptr<RSNGFilterBase> ConvertDisplacementDistortFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::DISPLACEMENT_DISTORT);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    auto dispDistortFilterPara = std::static_pointer_cast<DisplacementDistortPara>(filterPara);
    dispDistortFilter->Setter<DispDistortFactorTag>(dispDistortFilterPara->GetFactor());
    return dispDistortFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertEdgeLightFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::EDGE_LIGHT);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(filter);
    auto edgeLightFilterPara = std::static_pointer_cast<EdgeLightPara>(filterPara);
    edgeLightFilter->Setter<EdgeLightColorTag>(edgeLightFilterPara->GetColor());
    edgeLightFilter->Setter<EdgeLightAlphaTag>(edgeLightFilterPara->GetAlpha());
    edgeLightFilter->Setter<EdgeLightMaskTag>(RSNGMaskBase::Create(edgeLightFilterPara->GetMask()));
    edgeLightFilter->Setter<EdgeLightBloomTag>(edgeLightFilterPara->GetBloom());
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(edgeLightFilterPara->GetUseRawColor());
    return edgeLightFilter;
}
}

static std::unordered_map<FilterPara::ParaType, FilterConvertor> convertorLUT = {
    { FilterPara::ParaType::DISPLACEMENT_DISTORT, ConvertDisplacementDistortFilterPara },
    { FilterPara::ParaType::EDGE_LIGHT, ConvertEdgeLightFilterPara },
};

std::shared_ptr<RSNGFilterBase> RSNGFilterBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGFilterBase> RSNGFilterBase::Create(std::shared_ptr<FilterPara> filterPara)
{
    if (!filterPara) {
        return nullptr;
    }

    auto it = convertorLUT.find(filterPara->GetParaType());
    return it != convertorLUT.end() ? it->second(filterPara) : nullptr;
}

} // namespace Rosen
} // namespace OHOS
