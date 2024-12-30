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

 #include "modifier_ng/rs_render_modifier_ng.h"

#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "rs_profiler.h"
#include "rs_trace.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/appearance/rs_blend_render_modifier.h"
#include "modifier_ng/appearance/rs_env_foreground_color_render_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_render_modifier.h"
#include "modifier_ng/background/rs_background_color_render_modifier.h"
#include "modifier_ng/background/rs_background_image_render_modifier.h"
#include "modifier_ng/background/rs_background_shader_render_modifier.h"
#include "modifier_ng/background/rs_dynamic_light_up_render_modifier.h"
#include "modifier_ng/background/rs_mask_render_modifier.h"
#include "modifier_ng/background/rs_outline_render_modifier.h"
#include "modifier_ng/background/rs_shadow_render_modifier.h"
#include "modifier_ng/background/rs_useeffect_render_modifier.h"
#include "modifier_ng/foreground/rs_foreground_color_render_modifier.h"
#include "modifier_ng/geometry/rs_bounds_clip_render_modifier.h"
#include "modifier_ng/geometry/rs_bounds_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_render_modifier.h"
#include "modifier_ng/overlay/rs_border_render_modifier.h"
#include "modifier_ng/overlay/rs_point_light_render_modifier.h"
#include "modifier_ng/rs_background_render_modifier.h"
#include "modifier_ng/rs_foreground_render_modifier.h"
#include "modifier_ng/rs_overlay_render_modifier.h"
#include "modifier_ng/rs_transition_render_modifier.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"

namespace OHOS::Rosen::ModifierNG {
const RSRenderModifier::LegacyPropertyApplierMap RSRenderModifier::emptyLegacyPropertyApplierMap_;

void RSDrawable::Sync()
{
    if (!needSync_) {
        return;
    }
    OnSync();
    needSync_ = false;
}

void RSDrawable::Purge()
{
    needSync_ = true;
}

// ==========================================================================
std::array<RSRenderModifier::Constructor, ModifierNG::MODIFIER_TYPE_COUNT>
    RSRenderModifier::ConstructorLUT_ = {
        nullptr,                                                                        // INVALID
        nullptr,                                                                        // SAVE_ALL

        [] { return new RSTransformRenderModifier(); },                                 // TRANSFORM
        [] { return new RSAlphaRenderModifier(); },                                     // ALPHA
        nullptr,                                                                        // VISIBILITY

        // Bounds Geometry
        [] { return new RSMaskRenderModifier(); },                                      // MASK
        [] { return new RSCustomRenderModifier<RSModifierType::TRANSITION>(); },        // TRANSITION
        [] { return new RSEnvForegroundColorRenderModifier(); },                        // ENV_FOREGROUND_COLOR
        [] { return new RSShadowRenderModifier(); },                                    // SHADOW
        nullptr,                                                                        // BEGIN_FOREGROUND_FILTER
        [] { return new RSOutlineRenderModifier(); },                                   // OUTLINE

        // BG properties in Bounds Clip
        nullptr,                                                                        // BG_SAVE_BOUNDS
        [] { return new RSBoundsRenderModifier(); },                                    // BOUNDS
        [] { return new RSBoundsClipRenderModifier(); },                                // CLIP_TO_BOUNDS
        [] { return new RSBlendRenderModifier(); },                                     // BLENDER
        [] { return new RSBackgroundColorRenderModifier(); },                           // BACKGROUND_COLOR
        [] { return new RSBackgroundShaderRenderModifier(); },                          // BACKGROUND_SHADER
        [] { return new RSBackgroundImageRenderModifier(); },                           // BACKGROUND_IMAGE
        nullptr,                                                                        // BEHIND_WINDOW_FILTER
        [] { return new RSBackgroundFilterRenderModifier(); },                          // BACKGROUND_FILTER
        [] { return new RSUseEffectRenderModifier(); },                                 // USE_EFFECT
        [] { return new RSCustomRenderModifier<RSModifierType::BACKGROUND_STYLE>(); },  // BACKGROUND_STYLE
        [] { return new RSDynamicLightUpRenderModifier(); },                            // DYNAMIC_LIGHT_UP
        nullptr,                                                                        // BG_RESTORE_BOUNDS

        // Frame Geometry
        nullptr,                                                                        // SAVE_FRAME
        [] { return new RSFrameRenderModifier(); },                                     // FRAME
        [] { return new RSFrameClipRenderModifier(); },                                 // CLIP_TO_FRAME
        nullptr,                                                                        // CUSTOM_CLIP_TO_FRAME
        [] { return new RSCustomRenderModifier<RSModifierType::CONTENT_STYLE>(); },     // CONTENT_STYLE
        nullptr,                                                                        // CHILDREN
        nullptr,                                                                        // NODE_MODIFIER
        [] { return new RSCustomRenderModifier<RSModifierType::FOREGROUND_STYLE>(); },  // FOREGROUND_STYLE
        nullptr,                                                                        // RESTORE_FRAME

        // FG properties in Bounds clip
        nullptr,                                                                        // FG_SAVE_BOUNDS
        nullptr,                                                                        // FG_CLIP_TO_BOUNDS
        nullptr,                                                                        // BINARIZATION
        nullptr,                                                                        // COLOR_FILTER
        nullptr,                                                                        // LIGHT_UP_EFFECT
        nullptr,                                                                        // DYNAMIC_DIM
        [] { return new RSCompositingFilterRenderModifier(); },                         // COMPOSITING_FILTER
        [] { return new RSForegroundColorRenderModifier(); },                           // FOREGROUND_COLOR
        nullptr,                                                                        // FG_RESTORE_BOUNDS

        // No clip (unless ClipToBounds is set)
        [] { return new RSPointLightRenderModifier(); },                                // POINT_LIGHT
        [] { return new RSBorderRenderModifier(); },                                    // BORDER
        [] { return new RSCustomRenderModifier<RSModifierType::OVERLAY_STYLE>(); },     // OVERLAY_STYLE
        [] { return new RSParticleRenderModifier(); },                                  // PARTICLE_EFFECT
        nullptr,                                                                        // PIXEL_STRETCH

        // Restore state
        nullptr,                                                                        // RESTORE_BLENDER
        [] { return new RSForegroundFilterRenderModifier(); },                          // FOREGROUND_FILTER
        nullptr,                                                                        // RESTORE_ALL
    };

void RSRenderModifier::AttachProperty(RSPropertyType type, const std::shared_ptr<RSRenderPropertyBase>& property)
{
    auto [it, isInserted] = properties_.try_emplace(type, property);
    if (!isInserted) {
        return;
    }
    if (auto node = target_.lock()) {
        node->properties_.emplace(property->GetId(), property);
        property->Attach(std::static_pointer_cast<RSRenderModifier>(shared_from_this()));
        node->AddDirtyType(GetType());
    }
    dirty_ = true;
}

void RSRenderModifier::DetachProperty(RSPropertyType type)
{
    auto it = properties_.find(type);
    if (it == properties_.end()) {
        return;
    }
    if (auto node = target_.lock()) {
        node->properties_.erase(it->second->GetId());
        node->AddDirtyType(GetType());
    }
    properties_.erase(it);
    dirty_ = true;
}

bool RSRenderModifier::Apply(RSModifierContext& context)
{
    if (!dirty_) {
        return false;
    }
    if (OnApply(context)) {
        dirty_ = false;
        needSync_ = true;
        return true;
    }
    return false;
}

void RSRenderModifier::ApplyLegacyProperty(RSProperties& properties)
{
    if (!dirty_) {
        return;
    }
    dirty_ = false;
    const auto& map = GetLegacyPropertyApplierMap();
    for (auto& [type, property] : properties_) {
        auto it = map.find(type);
        if (it == map.end()) {
            RS_LOGE("apply legacy property failed, type: %{public}d", static_cast<int>(type));
            continue;
        }
        it->second(properties, *property);
    }
}

void RSRenderModifier::Purge()
{
    dirty_ = true;
    RSDrawable::Purge();
}

bool RSRenderModifier::IsAttached() const
{
    return !target_.expired();
}

void RSRenderModifier::OnAttachModifier(RSRenderNode& node)
{
    node.AddDirtyType(GetType());
    target_ = node.weak_from_this();
    for (auto& [type, property] : properties_) {
        node.properties_.emplace(property->GetId(), property);
        property->Attach(std::static_pointer_cast<RSRenderModifier>(shared_from_this()));
    }
    dirty_ = true;
}

void RSRenderModifier::OnDetachModifier()
{
    auto node = target_.lock();
    target_.reset();
    if (node == nullptr) {
        return;
    }
    for (auto& [type, property] : properties_) {
        node->properties_.erase(property->GetId());
    }
    node->AddDirtyType(GetType());
}

void RSRenderModifier::SetDirty()
{
    if (dirty_) {
        return;
    }
    dirty_ = true;
    if (auto node = target_.lock()) {
        node->AddDirtyType(GetType());
    }
}

// ===========================================================================
void RSDisplayListRenderModifier::OnSync()
{
    std::swap(stagingDrawCmd_, renderDrawCmd_);
    stagingDrawCmd_.reset();
    renderPropertyDescription_ = stagingPropertyDescription_;
    stagingPropertyDescription_.clear();
}

void RSDisplayListRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (renderDrawCmd_ != nullptr) {
        renderDrawCmd_->Playback(canvas, &rect);
    }
}

void RSDisplayListRenderModifier::Purge()
{
    // stagingDrawCmd_.reset();
    dirty_ = true;
    needSync_ = true;
}

bool RSRenderModifier::Marshalling(Parcel& parcel) const
{
    auto ret = RSMarshallingHelper::Marshalling(parcel, GetType());
    if (ret) {
        ret = RSMarshallingHelper::Marshalling(parcel, id_);
    }
    if (ret) {
        ret = RSMarshallingHelper::Marshalling(parcel, properties_);
    }
    return ret;
}

RSRenderModifier* RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    RSModifierType type = RSModifierType::MAX;
    if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
        return nullptr;
    }
    auto constructor = ConstructorLUT_[static_cast<uint8_t>(type)];
    if (constructor == nullptr) {
        return nullptr;
    }
    RSRenderModifier* ret = constructor();
    if (!RSMarshallingHelper::Unmarshalling(parcel, ret->id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, ret->properties_)) {
        delete ret;
        return nullptr;
    }
    for (auto& [type, property] : ret->properties_) {
        // Attach property to RSRenderModifier
        ret->AttachProperty(type, property);
    }
    return ret;
}

// ===========================================================================
template<RSModifierType T>
bool RSCustomRenderModifier<T>::OnApply(RSModifierContext& context)
{
    auto node = target_.lock();
    if (node->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        needClearOp_ = true;
    }
    stagingIsCanvasNode_ = node->template IsInstanceOf<RSCanvasRenderNode>();
    stagingGravity_ = context.frameGravity_;
    ModifierNG::RSPropertyType propType = StyleTypeConvertor::getPropertyType(Type);
    stagingDrawCmd_ = Getter<Drawing::DrawCmdListPtr>(propType, nullptr);
    return stagingDrawCmd_ != nullptr;
}

template<RSModifierType T>
void RSCustomRenderModifier<T>::OnSync()
{
    gravity_ = stagingGravity_;
    isCanvasNode_ = stagingIsCanvasNode_;
    std::swap(stagingDrawCmd_, renderDrawCmd_);
    stagingGravity_ = Gravity::DEFAULT;
    stagingIsCanvasNode_ = false;
    stagingDrawCmd_.reset();
}

template<RSModifierType T>
void RSCustomRenderModifier<T>::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (!renderDrawCmd_) {
        return;
    }
    Drawing::Matrix mat;
    if (isCanvasNode_ && RSPropertyDrawableUtils::GetGravityMatrix(
        gravity_, rect, renderDrawCmd_->GetWidth(), renderDrawCmd_->GetHeight(), mat)) {
        canvas.ConcatMatrix(mat);
    }
    renderDrawCmd_->Playback(canvas, &rect);
    if (needClearOp_ && Type == RSModifierType::CONTENT_STYLE) {
        RS_PROFILER_DRAWING_NODE_ADD_CLEAROP(renderDrawCmd_);
    }
}

template<RSModifierType T>
void RSCustomRenderModifier<T>::Purge()
{
    if (stagingDrawCmd_) {
        stagingDrawCmd_->Purge();
    }
    stagingDrawCmd_.reset();
    stagingGravity_ = Gravity::DEFAULT;
    stagingIsCanvasNode_ = false;
    needSync_ = true;
    dirty_ = true;
}

template class RSCustomRenderModifier<RSModifierType::TRANSITION>;
template class RSCustomRenderModifier<RSModifierType::BACKGROUND_STYLE>;
template class RSCustomRenderModifier<RSModifierType::CONTENT_STYLE>;
template class RSCustomRenderModifier<RSModifierType::FOREGROUND_STYLE>;
template class RSCustomRenderModifier<RSModifierType::OVERLAY_STYLE>;

bool RSFilterRenderModifier::NeedBlurFuzed()
{
    return RSSystemProperties::GetMESABlurFuzedEnabled() && HasProperty(RSPropertyType::GREY_COEF);
}

void RSFilterRenderModifier::ClearFilterCache() {}

std::shared_ptr<Drawing::ColorFilter> RSFilterRenderModifier::GetMaterialColorFilter(float sat, float brightness)
{
    float normalizedDegree = brightness - 1.0;
    const float brightnessMat[] = {
        1.000000f,
        0.000000f,
        0.000000f,
        0.000000f,
        normalizedDegree,
        0.000000f,
        1.000000f,
        0.000000f,
        0.000000f,
        normalizedDegree,
        0.000000f,
        0.000000f,
        1.000000f,
        0.000000f,
        normalizedDegree,
        0.000000f,
        0.000000f,
        0.000000f,
        1.000000f,
        0.000000f,
    };
    Drawing::ColorMatrix cm;
    cm.SetSaturation(sat);
    float cmArray[Drawing::ColorMatrix::MATRIX_SIZE];
    cm.GetArray(cmArray);
    std::shared_ptr<Drawing::ColorFilter> filterCompose =
        Drawing::ColorFilter::CreateComposeColorFilter(cmArray, brightnessMat, Drawing::Clamp::NO_CLAMP);
    return filterCompose;
}

void RSFilterRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (needDrawBehindWindow_) {
        if (!canvas.GetSurface()) {
            RS_LOGE(
                "RSFilterRenderModifier::Draw DrawBehindWindow surface:[%{public}d]", canvas.GetSurface() != nullptr);
            return;
        }
        RS_TRACE_NAME_FMT("RSFilterRenderModifier::Draw DrawBehindWindow node[%llu], windowFreezeCapture[%d]",
            renderNodeId_, canvas.GetIsWindowFreezeCapture());
        if (canvas.GetIsWindowFreezeCapture()) {
            RS_LOGD("RSFilterRenderModifier::Draw DrawBehindWindow capture freeze surface, "
                    "no need to drawBehindWindow");
            return;
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect(rect);
        Drawing::Rect absRect(0.0, 0.0, 0.0, 0.0);
        Drawing::Rect relativeBounds(drawBehindWindowRegion_.GetLeft(), drawBehindWindowRegion_.GetTop(),
            drawBehindWindowRegion_.GetRight(), drawBehindWindowRegion_.GetBottom());
        canvas.GetTotalMatrix().MapRect(absRect, relativeBounds);
        Drawing::RectI bounds(std::ceil(absRect.GetLeft()), std::ceil(absRect.GetTop()), std::ceil(absRect.GetRight()),
            std::ceil(absRect.GetBottom()));
        auto deviceRect = Drawing::RectI(0, 0, canvas.GetSurface()->Width(), canvas.GetSurface()->Height());
        bounds.Intersect(deviceRect);
        RSPropertyDrawableUtils::DrawBackgroundEffect(&canvas, filter_, cacheManager_, bounds);
        return;
    }
    if (renderIntersectWithDRM_) {
        RS_TRACE_NAME_FMT("RSFilterRenderModifier::Draw IntersectWithDRM node[%lld] isDarkColorMode[%d]", renderNodeId_,
            renderIsDarkColorMode_);
        RSPropertyDrawableUtils::DrawFilterWithDRM(&canvas, renderIsDarkColorMode_);
        return;
    }
    if (filter_) {
        RS_TRACE_NAME_FMT("RSFilterRenderModifier::Draw node[%llu] ", renderNodeId_);
        if (filter_->GetFilterType() == RSFilter::LINEAR_GRADIENT_BLUR) {
            auto filter = std::static_pointer_cast<RSDrawingFilter>(filter_);
            std::shared_ptr<RSShaderFilter> rsShaderFilter =
                filter->GetShaderFilterWithType(RSShaderFilter::LINEAR_GRADIENT_BLUR);
            if (rsShaderFilter != nullptr) {
                auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
                tmpFilter->SetGeometry(canvas, rect.GetWidth(), rect.GetHeight());
            }
        }
        int64_t startBlurTime = Drawing::PerfmonitorReporter::GetCurrentTime();
        RSPropertyDrawableUtils::DrawFilter(&canvas, filter_, cacheManager_, IsForeground());
        int64_t blurDuration = Drawing::PerfmonitorReporter::GetCurrentTime() - startBlurTime;
        auto filterType = filter_->GetFilterType();
        RSPerfMonitorReporter::GetInstance().RecordBlurNode(
            renderNodeName_, blurDuration, RSPropertyDrawableUtils::IsBlurFilterType(filterType));
        RSPerfMonitorReporter::GetInstance().RecordBlurPerfEvent(renderNodeId_, renderNodeName_,
            static_cast<uint16_t>(filterType), RSPropertyDrawableUtils::GetBlurFilterRadius(filter_), rect.GetWidth(),
            rect.GetHeight(), blurDuration, RSPropertyDrawableUtils::IsBlurFilterType(filterType));
    }
}

void RSFilterRenderModifier::Purge()
{
    dirty_ = true;
    needSync_ = true;
}

void RSFilterRenderModifier::OnSync()
{
    filter_ = std::move(stagingFilter_);

    renderFilterHashChanged_ = stagingFilterHashChanged_;
    renderForceClearCacheForLastFrame_ = stagingForceClearCacheForLastFrame_;
    renderIsEffectNode_ = stagingIsEffectNode_;
    renderIsSkipFrame_ = stagingIsSkipFrame_;
    renderNodeId_ = stagingNodeId_;
    renderNodeName_ = stagingNodeName_;
    renderClearType_ = stagingClearType_;
    renderIntersectWithDRM_ = stagingIntersectWithDRM_;
    renderIsDarkColorMode_ = stagingIsDarkColorMode_;

    ClearFilterCache();

    stagingFilterHashChanged_ = false;
    stagingFilterRegionChanged_ = false;
    stagingFilterInteractWithDirty_ = false;
    stagingRotationChanged_ = false;
    stagingForceClearCache_ = false;
    stagingForceUseCache_ = false;
    stagingIsOccluded_ = false;
    stagingForceClearCacheForLastFrame_ = false;
    stagingIntersectWithDRM_ = false;
    stagingIsDarkColorMode_ = false;

    stagingClearType_ = FilterCacheType::BOTH;
    stagingIsLargeArea_ = false;
    isFilterCacheValid_ = false;
    stagingIsEffectNode_ = false;
    stagingIsSkipFrame_ = false;
    stagingUpdateInterval_ = cacheUpdateInterval_;
    stagingLastCacheType_ = lastCacheType_;
}
} // namespace OHOS::Rosen::ModifierNG
