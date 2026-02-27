/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "common/rs_optional_trace.h"
#include "effect/rs_render_mask_base.h"
#include "effect/rs_render_shape_base.h"
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
    {RSNGEffectType::MATERIAL_BLUR, [] {
            return std::make_shared<RSNGRenderMaterialBlurFilter>();
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
    {RSNGEffectType::COLOR_GRADIENT, [] {
            return std::make_shared<RSNGRenderColorGradientFilter>();
        }
    },
    {RSNGEffectType::DIRECTION_LIGHT, [] {
            return std::make_shared<RSNGRenderDirectionLightFilter>();
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
    },
    {RSNGEffectType::CONTENT_LIGHT, [] {
            return std::make_shared<RSNGRenderContentLightFilter>();
        }
    },
    {RSNGEffectType::GASIFY_SCALE_TWIST, [] {
            return std::make_shared<RSNGRenderGasifyScaleTwistFilter>();
        }
    },
    {RSNGEffectType::GASIFY_BLUR, [] {
            return std::make_shared<RSNGRenderGasifyBlurFilter>();
        }
    },
    {RSNGEffectType::GASIFY, [] {
            return std::make_shared<RSNGRenderGasifyFilter>();
        }
    },
    {RSNGEffectType::FROSTED_GLASS, [] {
            return std::make_shared<RSNGRenderFrostedGlassFilter>();
        }
    },
    {RSNGEffectType::FROSTED_GLASS_BLUR, [] {
            return std::make_shared<RSNGRenderFrostedGlassBlurFilter>();
        }
    },
    {RSNGEffectType::GRID_WARP, [] {
            return std::make_shared<RSNGRenderGridWarpFilter>();
        }
    },
    {RSNGEffectType::SDF_EDGE_LIGHT, [] {
            return std::make_shared<RSNGRenderSDFEdgeLightFilter>();
        }
    },
    {RSNGEffectType::MAGNIFIER, [] {
            return std::make_shared<RSNGRenderMagnifierFilter>();
        }
    },
};

using FilterGetSnapshotRect = std::function<RectF(std::shared_ptr<RSNGRenderFilterBase>, RectF)>;
static std::unordered_map<RSNGEffectType, FilterGetSnapshotRect> getSnapshotRectLUT = {
    {
        RSNGEffectType::FROSTED_GLASS, [](std::shared_ptr<RSNGRenderFilterBase> filter, RectF rect) {
            auto frostedGlass = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(filter);
            auto blurRadius = frostedGlass->Getter<OHOS::Rosen::FrostedGlassBlurParamsRenderTag>()->Get();
            auto tmp = frostedGlass->Getter<OHOS::Rosen::FrostedGlassEnvLightParamsRenderTag>()->Get();
            auto samplingScale = frostedGlass->Getter<OHOS::Rosen::FrostedGlassSamplingScaleRenderTag>()->Get();
            auto weightsEmboss = frostedGlass->Getter<OHOS::Rosen::FrostedGlassWeightsEmbossRenderTag>()->Get();
            if (ROSEN_LE(weightsEmboss[1], 0.0f)) {
                return rect;
            }
            auto refractOutPx = tmp[0];
            const float maxRefractOutPx = 500.0f;
            float outStep = std::max(blurRadius[0] +
                std::max(std::min(refractOutPx, maxRefractOutPx), 0.0f), 0.0f) * samplingScale;
            auto snapshotRect = rect;
            snapshotRect.SetAll(rect.GetLeft() - outStep, rect.GetTop() - outStep, rect.GetWidth() + outStep * 2,
                rect.GetHeight() + outStep * 2);
            return snapshotRect;
        }
    },
    {
        RSNGEffectType::FROSTED_GLASS_BLUR, [](std::shared_ptr<RSNGRenderFilterBase> filter, RectF rect) {
            auto frostedGlassBlur = std::static_pointer_cast<RSNGRenderFrostedGlassBlurFilter>(filter);
            auto blurRadius = frostedGlassBlur->Getter<OHOS::Rosen::FrostedGlassBlurRadiusRenderTag>()->Get();
            auto refractOutPx = frostedGlassBlur->Getter<OHOS::Rosen::FrostedGlassBlurRefractOutPxRenderTag>()->Get();
            if (ROSEN_LE(refractOutPx, 0.0f)) {
                return rect;
            }
            const float maxRefractOutPx = 500.0f;
            float outStep = std::max(blurRadius + std::max(std::min(refractOutPx, maxRefractOutPx), 0.0f), 0.0f);
            auto snapshotRect = rect;
            snapshotRect.SetAll(rect.GetLeft() - outStep, rect.GetTop() - outStep,
                rect.GetWidth() + outStep * 2, rect.GetHeight() + outStep * 2);
            return snapshotRect;
        }
    },
    {
        RSNGEffectType::MAGNIFIER, [](std::shared_ptr<RSNGRenderFilterBase> filter, RectF rect) {
            auto magnifier = std::static_pointer_cast<RSNGRenderMagnifierFilter>(filter);
            auto offsetX = magnifier->Getter<OHOS::Rosen::MagnifierOffsetXRenderTag>()->Get();
            auto offsetY = magnifier->Getter<OHOS::Rosen::MagnifierOffsetYRenderTag>()->Get();
            auto snapshotRect = rect;
            snapshotRect.SetAll(rect.GetLeft() + offsetX, rect.GetTop() + offsetY, rect.GetWidth(), rect.GetHeight());
            return snapshotRect;
        }
    },
};

using FilterGetDrawRect = std::function<RectF(std::shared_ptr<RSNGRenderFilterBase>, RectF)>;
static std::unordered_map<RSNGEffectType, FilterGetDrawRect> getDrawRectLUT = {};

std::shared_ptr<RSNGRenderFilterBase> RSNGRenderFilterBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

[[nodiscard]] bool RSNGRenderFilterBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderFilterBase>& val)
{
    std::shared_ptr<RSNGRenderFilterBase> head = nullptr;
    auto current = head;
    for (size_t filterCount = 0; filterCount < EFFECT_COUNT_LIMIT; ++filterCount) {
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

void RSNGRenderFilterBase::UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect>& src,
                                           std::shared_ptr<Drawing::GEVisualEffect>& dest)
{
    RSNGRenderFilterHelper::UpdateCacheData(src, dest);
}

void RSNGRenderFilterHelper::GenerateGEVisualEffect(std::shared_ptr<RSNGRenderFilterBase>& filter)
{
    if (filter) {
        filter->GenerateGEVisualEffect();
    }
}

void RSNGRenderFilterHelper::UpdateToGEContainer(std::shared_ptr<RSNGRenderFilterBase>& filter,
    std::shared_ptr<Drawing::GEVisualEffectContainer>& container)
{
    if (!filter || !container) {
        RS_LOGE("RSNGRenderFilterHelper::UpdateToGEContainer: filter or container nullptr");
        return;
    }

    auto current = filter;
    while (current) {
        container->AddToChainedFilter(current->geFilter_);
        current = current->nextEffect_;
    }
}

bool RSNGRenderFilterHelper::CheckEnableEDR(std::shared_ptr<RSNGRenderFilterBase>& filter)
{
    auto current = filter;
    while (current) {
        if (RSEffectLuminanceManager::GetEnableHdrEffect(current)) {
            RS_OPTIONAL_TRACE_NAME_FMT("CheckEnableEDR:find edr filter, type=%d", static_cast<int>(current->GetType()));
            return true;
        }
        current = current->nextEffect_;
    }
    return false;
}

void RSNGRenderFilterHelper::UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect>& src,
    std::shared_ptr<Drawing::GEVisualEffect>& dest)
{
    if (src == nullptr) {
        RS_LOGE("RSNGRenderFilterHelper::UpdateCacheData: src is nullptr");
        return;
    }
    if (dest == nullptr) {
        RS_LOGE("RSNGRenderFilterHelper::UpdateCacheData: dest is nullptr");
        return;
    }

    auto srcImpl = src->GetImpl();
    auto destImpl = dest->GetImpl();
    bool typeMismatch = srcImpl == nullptr || destImpl == nullptr ||
                        srcImpl->GetFilterType() != destImpl->GetFilterType();
    if (typeMismatch) {
        RS_LOGE("RSNGRenderFilterHelper::UpdateCacheData: effect impl type mismatch");
        return;
    }

    auto cache = srcImpl->GetCache();
    if (cache != nullptr) {
        destImpl->SetCache(cache);
    }
}

void RSNGRenderFilterHelper::SetRotationAngle(std::shared_ptr<RSNGRenderFilterBase>& filter,
    const Vector3f& rotationAngle)
{
    auto current = filter;
    while (current) {
        if (current->GetType() == RSNGEffectType::CONTENT_LIGHT) {
            auto contentLightFilter = std::static_pointer_cast<RSNGRenderContentLightFilter>(current);
            contentLightFilter->Setter<ContentLightRotationAngleRenderTag>(rotationAngle);
        }
        current =  current->nextEffect_;
    }
}

RectF RSNGRenderFilterHelper::CalcRect(const std::shared_ptr<RSNGRenderFilterBase>& filter, const RectF& bound,
    EffectRectType rectType)
{
    if (filter == nullptr) {
        return RectF();
    }

    switch (rectType) {
        case EffectRectType::SNAPSHOT: {
            auto iter = getSnapshotRectLUT.find(filter->GetType());
            return iter == getSnapshotRectLUT.end() ? bound : iter->second(filter, bound);
        }
        case EffectRectType::DRAW: {
            auto current = filter;
            while (current->nextEffect_) {
                current = current->nextEffect_;
            }
            auto iter = getDrawRectLUT.find(current->GetType());
            return iter == getDrawRectLUT.end() ? bound : iter->second(filter, bound);
        }
        default:
            return RectF();
    }
}
} // namespace Rosen
} // namespace OHOS
