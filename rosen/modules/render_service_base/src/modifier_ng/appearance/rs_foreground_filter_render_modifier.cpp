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

#include "modifier_ng/appearance/rs_foreground_filter_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "property/rs_properties.h"
#include "render/rs_colorful_shadow_filter.h"
#include "render/rs_distortion_shader_filter.h"
#include "render/rs_fly_out_shader_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_spherize_effect_filter.h"

namespace OHOS::Rosen::ModifierNG {
constexpr float SPHERIZE_VALID_EPSILON = 0.001f;   // used to judge if spherize valid
constexpr float ATTRACTION_VALID_EPSILON = 0.001f; // used to judge if attraction valid
const bool RSForegroundFilterRenderModifier::IS_UNI_RENDER = RSUniRenderJudgement::IsUniRender();

const RSForegroundFilterRenderModifier::LegacyPropertyApplierMap
    RSForegroundFilterRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::SPHERIZE, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetSpherize> },
        { RSPropertyType::FOREGROUND_EFFECT_RADIUS,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetForegroundEffectRadius> },
        { RSPropertyType::MOTION_BLUR_PARA,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<MotionBlurParam>, &RSProperties::SetMotionBlurPara> },
        { RSPropertyType::FLY_OUT_PARAMS,
            RSRenderModifier::PropertyApplyHelper<std::optional<RSFlyOutPara>, &RSProperties::SetFlyOutParams> },
        { RSPropertyType::FLY_OUT_DEGREE, RSRenderModifier::PropertyApplyHelperAdd<float,
                                              &RSProperties::SetFlyOutDegree, &RSProperties::GetFlyOutDegree> },
        { RSPropertyType::DISTORTION_K, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>,
                                            &RSProperties::SetDistortionK, &RSProperties::GetDistortionK> },
        { RSPropertyType::ATTRACTION_FRACTION,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetAttractionFraction> },
        { RSPropertyType::ATTRACTION_DSTPOINT,
            RSRenderModifier::PropertyApplyHelper<Vector2f, &RSProperties::SetAttractionDstPoint> },
    };

void RSForegroundFilterRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetSpherize(0.f);
    properties.SetForegroundEffectRadius(0.f);
    properties.SetMotionBlurPara({});
    properties.SetFlyOutParams({});
    properties.SetFlyOutDegree(0.0f);
    properties.SetDistortionK(0.0f);
    properties.SetAttractionFraction(0.f);
    properties.SetAttractionDstPoint({});
}

float RSForegroundFilterRenderModifier::GetShadowElevation() const
{
    return shadow_ ? shadow_->GetElevation() : 0.f;
}

float RSForegroundFilterRenderModifier::GetShadowRadius() const
{
    return shadow_ ? shadow_->GetRadius() : DEFAULT_SHADOW_RADIUS;
}

std::shared_ptr<RSPath> RSForegroundFilterRenderModifier::GetShadowPath() const
{
    return shadow_ ? shadow_->GetPath() : nullptr;
}

float RSForegroundFilterRenderModifier::GetShadowOffsetX() const
{
    return shadow_ ? shadow_->GetOffsetX() : DEFAULT_SHADOW_OFFSET_X;
}

float RSForegroundFilterRenderModifier::GetShadowOffsetY() const
{
    return shadow_ ? shadow_->GetOffsetY() : DEFAULT_SHADOW_OFFSET_Y;
}

bool RSForegroundFilterRenderModifier::GetShadowIsFilled() const
{
    return shadow_ ? shadow_->GetIsFilled() : false;
}

std::shared_ptr<RSMotionBlurFilter> RSForegroundFilterRenderModifier::GenerateMotionBlurFilterIfValid()
{
    if (HasProperty(RSPropertyType::MOTION_BLUR_PARA)) {
        auto motionBlurPara = Getter<std::shared_ptr<MotionBlurParam>>(RSPropertyType::MOTION_BLUR_PARA);
        if (ROSEN_GNE(motionBlurPara->radius, 0.0)) {
            return std::make_shared<RSMotionBlurFilter>(motionBlurPara);
        }
    }
    return nullptr;
}

std::shared_ptr<RSForegroundEffectFilter> RSForegroundFilterRenderModifier::GenerateForegroundEffectFilterIfValid()
{
    auto foregroundEffectRadius = Getter<float>(RSPropertyType::FOREGROUND_EFFECT_RADIUS, 0.f);
    return ROSEN_GNE(foregroundEffectRadius, 0.999f)
               ? std::make_shared<RSForegroundEffectFilter>(foregroundEffectRadius)
               : nullptr; // if blur radius < 1, no need to draw
}

std::shared_ptr<RSSpherizeEffectFilter> RSForegroundFilterRenderModifier::GenerateSphereEffectFilterIfValid()
{
    auto spherize = Getter<float>(RSPropertyType::SPHERIZE, 0.f);
    return ROSEN_GNE(spherize, SPHERIZE_VALID_EPSILON) ? std::make_shared<RSSpherizeEffectFilter>(spherize) : nullptr;
}

std::shared_ptr<RSFlyOutShaderFilter> RSForegroundFilterRenderModifier::GenerateFlyOutShaderFilterIfValid()
{
    auto flyOutDegree = Getter<float>(RSPropertyType::FLY_OUT_DEGREE, 0.f);
    if (ROSEN_GE(flyOutDegree, 0.0f) && ROSEN_LE(flyOutDegree, 1.0f) && HasProperty(RSPropertyType::FLY_OUT_PARAMS)) {
        return std::make_shared<RSFlyOutShaderFilter>(
            flyOutDegree, Getter<RSFlyOutPara>(RSPropertyType::FLY_OUT_PARAMS).flyMode);
    }
    return nullptr;
}

std::shared_ptr<RSAttractionEffectFilter> RSForegroundFilterRenderModifier::GenerateAttractionEffectFilterIfValid(
    const RSModifierContext& context)
{
    auto attractionFraction = Getter<float>(RSPropertyType::ATTRACTION_FRACTION);
    if (ROSEN_GNE(attractionFraction, ATTRACTION_VALID_EPSILON)) {
        float canvasWidth = context.GetBoundsRect().GetWidth();
        float canvasHeight = context.GetBoundsRect().GetHeight();
        Vector2f destinationPoint = Getter<Vector2f>(RSPropertyType::ATTRACTION_DSTPOINT);
        float windowLeftPoint = context.frame_.GetLeft();
        float windowTopPoint = context.frame_.GetTop();
        auto attractionEffectFilter = std::make_shared<RSAttractionEffectFilter>(attractionFraction);
        attractionEffectFilter->CalculateWindowStatus(canvasWidth, canvasHeight, destinationPoint);
        attractionEffectFilter->UpdateDirtyRegion(windowLeftPoint, windowTopPoint);
        // TODO: remove it
        attractionEffectCurrentDirtyRegion_ = attractionEffectFilter->GetAttractionDirtyRegion();
        return attractionEffectFilter;
    }
    return nullptr;
}

std::shared_ptr<RSColorfulShadowFilter> RSForegroundFilterRenderModifier::GenerateColorfulShadowFilterIfValid(
    const RSModifierContext& context)
{
    if (shadow_->GetMask()) {
        float elevation = GetShadowElevation();
        Drawing::scalar n1 = 0.25f * elevation * (1 + elevation / 128.0f); // 0.25f 128.0f
        Drawing::scalar blurRadius = elevation > 0.0f ? n1 : GetShadowRadius();
        Drawing::Path path = RSPropertyDrawableUtils::CreateShadowPath(
            GetShadowPath(), context.clipBoundsParams_.clipBounds_, context.GetRRect());
        return std::make_shared<RSColorfulShadowFilter>(
            blurRadius, GetShadowOffsetX(), GetShadowOffsetY(), path, GetShadowIsFilled());
    }
    return nullptr;
}

std::shared_ptr<RSDistortionFilter> RSForegroundFilterRenderModifier::GenerateDistortionFilterIfValid()
{
    if (HasProperty(RSPropertyType::DISTORTION_K)) {
        auto distortionK = Getter<float>(RSPropertyType::DISTORTION_K);
        if (ROSEN_GE(distortionK, -1.0f) && ROSEN_LE(distortionK, 1.0f)) {
            return std::make_shared<RSDistortionFilter>(distortionK);
        }
    }
    return nullptr;
}

void RSForegroundFilterRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    RSPropertyDrawableUtils::DrawForegroundFilter(canvas, filter_);
}

std::shared_ptr<RSBeginForegroundFilterDrawable> RSForegroundFilterRenderModifier::GetBeginForegroundFilterDrawable()
{
    return beginForegroundFilterDrawable_;
}

bool RSForegroundFilterRenderModifier::OnApply(RSModifierContext& context)
{
    if (auto filter = GenerateMotionBlurFilterIfValid()) {
        stagingFilter_ = filter;
    } else if (auto filter = GenerateForegroundEffectFilterIfValid()) {
        stagingFilter_ = filter;
    } else if (auto filter = GenerateSphereEffectFilterIfValid()) {
        stagingFilter_ = filter;
    } else if (auto filter = GenerateFlyOutShaderFilterIfValid()) {
        stagingFilter_ = filter;
    } else if (auto filter = GenerateAttractionEffectFilterIfValid(context)) {
        stagingFilter_ = filter;
    } else if (auto filter = GenerateColorfulShadowFilterIfValid(context)) {
        stagingFilter_ = filter;
    } else if (auto filter = GenerateDistortionFilterIfValid()) {
        stagingFilter_ = filter;
    } else {
        stagingFilter_.reset();
    }
    if (stagingFilter_) {
        // beginForegroundFilterDrawable_ = std::make_shared<RSBeginForegroundFilterDrawable>();
        // beginForegroundFilterDrawable_->SetBoundsRect(context.GetBoundsRect());
        return true;
    } else {
        return false;
    }
}

void RSForegroundFilterRenderModifier::OnSync()
{
    std::swap(filter_, stagingFilter_);
    beginForegroundFilterDrawable_->Sync();
}

void RSBeginForegroundFilterDrawable::SetBoundsRect(const RectF& boundsRect)
{
    stagingBoundsRect_ = boundsRect;
}

void RSBeginForegroundFilterDrawable::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    RSPropertyDrawableUtils::BeginForegroundFilter(canvas, boundsRect_);
}

void RSBeginForegroundFilterDrawable::OnSync()
{
    std::swap(boundsRect_, stagingBoundsRect_);
}
} // namespace OHOS::Rosen::ModifierNG
