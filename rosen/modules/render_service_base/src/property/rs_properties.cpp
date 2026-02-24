/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "property/rs_properties.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <math.h>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <securec.h>

#include "src/core/SkOpts.h"

#include "animation/rs_particle_ripple_field.h"
#include "animation/rs_particle_velocity_field.h"
#include "animation/rs_render_particle_animation.h"
#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector4.h"
#include "draw/color.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_mask_base.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "effect/runtime_blender_builder.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_color_picker_def.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_def.h"
#include "render/rs_attraction_effect_filter.h"
#include "render/rs_color_adaptive_filter.h"
#include "render/rs_colorful_shadow_filter.h"
#include "render/rs_distortion_shader_filter.h"
#include "render/rs_filter.h"
#include "render/rs_fly_out_shader_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_hdr_ui_brightness_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_particles_drawable.h"
#include "render/rs_render_aibar_filter.h"
#include "render/rs_render_always_snapshot_filter.h"
#include "render/rs_render_filter_base.h"
#include "render/rs_render_grey_filter.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_light_blur_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_magnifier_filter.h"
#include "render/rs_render_maskcolor_filter.h"
#include "render/rs_render_mesa_blur_filter.h"
#include "render/rs_render_water_ripple_filter.h"
#include "render/rs_spherize_effect_filter.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

#ifdef ROSEN_OHOS
#include "app_mgr_client.h"
#include "hisysevent.h"
#endif

/**
 * Usage:
 * WITH_EFFECT(optionalMember.reset());
 * WITH_EFFECT(rawPointer = nullptr);
 * WITH_EFFECT(optionalMem = std::nullopt);
 * WITH_EFFECT(container.clear());
 */
#undef WITH_EFFECT
#define WITH_EFFECT(expr) \
    do { \
        if (effect_) { \
            (effect_->expr); \
        } \
    } while (0)

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INDEX_2 = 2;
constexpr int32_t INDEX_4 = 4;
constexpr int32_t INDEX_5 = 5;
constexpr int32_t INDEX_6 = 6;
constexpr int32_t INDEX_7 = 7;
constexpr int32_t INDEX_9 = 9;
constexpr int32_t INDEX_10 = 10;
constexpr int32_t INDEX_11 = 11;
constexpr int32_t INDEX_12 = 12;
constexpr int32_t INDEX_14 = 14;
constexpr int32_t INDEX_18 = 18;

const Vector4f Vector4fZero { 0.f, 0.f, 0.f, 0.f };
const auto EMPTY_RECT = RectF();
constexpr float SPHERIZE_VALID_EPSILON = 0.001f; // used to judge if spherize valid
constexpr float ATTRACTION_VALID_EPSILON = 0.001f; // used to judge if attraction valid
} // namespace

#ifdef SUBTREE_PARALLEL_ENABLE
static inline bool IsFilterNeedForceSubmit(const std::shared_ptr<RSFilter>& filter)
{
    return filter != nullptr && filter->NeedForceSubmit();
}
#endif

// Only enable filter cache when uni-render is enabled and filter cache is enabled
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef ROSEN_ARKUI_X
bool RSProperties::filterCacheEnabled_ =
    RSSystemProperties::GetFilterCacheEnabled() && RSUniRenderJudgement::IsUniRender();
void RSProperties::SetFilterCacheEnabledByCCM(bool isCCMFilterCacheEnable)
{
    filterCacheEnabled_ = (RSSystemProperties::GetFilterCacheEnabled() &&
        isCCMFilterCacheEnable) && RSUniRenderJudgement::IsUniRender();
}
#else
bool RSProperties::filterCacheEnabled_ = false;
#endif
#endif

bool RSProperties::blurAdaptiveAdjustEnabled_ = false;
void RSProperties::SetBlurAdaptiveAdjustEnabledByCCM(bool isCCMBlurAdaptiveAdjustEnabled)
{
    blurAdaptiveAdjustEnabled_ = isCCMBlurAdaptiveAdjustEnabled;
}

const bool RSProperties::IS_UNI_RENDER = RSUniRenderJudgement::IsUniRender();
const bool RSProperties::FOREGROUND_FILTER_ENABLED = RSSystemProperties::GetForegroundFilterEnabled();

RSProperties::RSProperties()
{
    boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
}

RSProperties::~RSProperties() = default;


void RSProperties::SetBounds(Vector4f bounds)
{
    if (bounds.z_ != boundsGeo_->GetWidth() || bounds.w_ != boundsGeo_->GetHeight()) {
        contentDirty_ = true;
    }
    boundsGeo_->SetRect(bounds.x_, bounds.y_, bounds.z_, bounds.w_);
    hasBounds_ = true;
    geoDirty_ = true;
    SetDirty();
    if (IsShadowMaskValid()) {
        filterNeedUpdate_ = true;
    }
}

void RSProperties::SetBoundsSize(Vector2f size)
{
    boundsGeo_->SetSize(size.x_, size.y_);
    hasBounds_ = true;
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsWidth(float width)
{
    boundsGeo_->SetWidth(width);
    hasBounds_ = true;
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsHeight(float height)
{
    boundsGeo_->SetHeight(height);
    hasBounds_ = true;
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPosition(Vector2f position)
{
    boundsGeo_->SetPosition(position.x_, position.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPositionX(float positionX)
{
    boundsGeo_->SetX(positionX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPositionY(float positionY)
{
    boundsGeo_->SetY(positionY);
    geoDirty_ = true;
    SetDirty();
}

Vector2f RSProperties::GetBoundsSize() const
{
    return { boundsGeo_->GetWidth(), boundsGeo_->GetHeight() };
}

float RSProperties::GetBoundsPositionX() const
{
    return boundsGeo_->GetX();
}

float RSProperties::GetBoundsPositionY() const
{
    return boundsGeo_->GetY();
}

Vector2f RSProperties::GetBoundsPosition() const
{
    return { GetBoundsPositionX(), GetBoundsPositionY() };
}

void RSProperties::SetFrame(Vector4f frame)
{
    if (frame.z_ != frameGeo_.GetWidth() || frame.w_ != frameGeo_.GetHeight()) {
        contentDirty_ = true;
    }
    frameGeo_.SetRect(frame.x_, frame.y_, frame.z_, frame.w_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFrameSize(Vector2f size)
{
    frameGeo_.SetSize(size.x_, size.y_);
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetFrameWidth(float width)
{
    frameGeo_.SetWidth(width);
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetFrameHeight(float height)
{
    frameGeo_.SetHeight(height);
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePosition(Vector2f position)
{
    frameGeo_.SetPosition(position.x_, position.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePositionX(float positionX)
{
    frameGeo_.SetX(positionX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePositionY(float positionY)
{
    frameGeo_.SetY(positionY);
    geoDirty_ = true;
    SetDirty();
}

Vector4f RSProperties::GetFrame() const
{
    return { frameGeo_.GetX(), frameGeo_.GetY(), frameGeo_.GetWidth(), frameGeo_.GetHeight() };
}

Vector2f RSProperties::GetFrameSize() const
{
    return { frameGeo_.GetWidth(), frameGeo_.GetHeight() };
}

float RSProperties::GetFramePositionX() const
{
    return frameGeo_.GetX();
}

float RSProperties::GetFramePositionY() const
{
    return frameGeo_.GetY();
}

Vector2f RSProperties::GetFramePosition() const
{
    return { GetFramePositionX(), GetFramePositionY() };
}

const RSObjGeometry& RSProperties::GetFrameGeometry() const
{
    return frameGeo_;
}

bool RSProperties::UpdateGeometryByParent(const Drawing::Matrix* parentMatrix,
    const std::optional<Drawing::Point>& offset)
{
    static thread_local Drawing::Matrix prevAbsMatrix;
    prevAbsMatrix.Swap(prevAbsMatrix_);
    boundsGeo_->UpdateMatrix(parentMatrix, offset);
    prevAbsMatrix_ = boundsGeo_->GetAbsMatrix();
    if (!RSSystemProperties::GetSkipGeometryNotChangeEnabled()) {
        return true;
    }
    const auto& rect = boundsGeo_->GetAbsRect();
    if (!lastRect_.has_value()) {
        lastRect_ = rect;
        return true;
    }
    auto dirtyFlag = (rect != lastRect_.value()) || !(prevAbsMatrix == prevAbsMatrix_);
    lastRect_ = rect;
    bgShaderNeedUpdate_ = true;
    const auto& fgNGRenderFilter = GetForegroundNGFilter();
    if (fgNGRenderFilter && fgNGRenderFilter->ContainsType(RSNGEffectType::CONTENT_LIGHT)) {
        filterNeedUpdate_ = true;
    }
    return dirtyFlag;
}

bool RSProperties::UpdateGeometry(
    const RSProperties* parent, bool dirtyFlag, const std::optional<Drawing::Point>& offset)
{
    if (!dirtyFlag && !geoDirty_) {
        return false;
    }
    auto parentMatrix = parent == nullptr ? nullptr : &(parent->GetBoundsGeometry()->GetAbsMatrix());
    if (parentMatrix && sandbox_ && sandbox_->matrix_) {
        parentMatrix = &(sandbox_->matrix_.value());
    }
    boundsGeo_->UpdateMatrix(parentMatrix, offset);
    if (RSSystemProperties::GetSkipGeometryNotChangeEnabled()) {
        auto rect = boundsGeo_->GetAbsRect();
        if (!lastRect_.has_value()) {
            lastRect_ = rect;
            return true;
        }
        dirtyFlag = dirtyFlag || rect != lastRect_.value();
        lastRect_ = rect;
        return dirtyFlag;
    } else {
        return true;
    }
}

void RSProperties::SetSandBox(const std::optional<Vector2f>& parentPosition)
{
    if (!sandbox_) {
        sandbox_ = std::make_unique<Sandbox>();
    }
    sandbox_->position_ = parentPosition;
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::ResetSandBox()
{
    sandbox_ = nullptr;
}

void RSProperties::UpdateSandBoxMatrix(const std::optional<Drawing::Matrix>& rootMatrix)
{
    if (!sandbox_) {
        return;
    }
    if (!rootMatrix || !sandbox_->position_) {
        sandbox_->matrix_ = std::nullopt;
        return;
    }
    auto rootMat = rootMatrix.value();
    bool hasScale = false;
    // scaleFactors[0]-minimum scaling factor, scaleFactors[1]-maximum scaling factor
    Drawing::scalar scaleFactors[2];
    bool getMinMaxScales = rootMat.GetMinMaxScales(scaleFactors);
    if (getMinMaxScales) {
        hasScale = !ROSEN_EQ(scaleFactors[0], 1.f) || !ROSEN_EQ(scaleFactors[1], 1.f);
    }
    if (hasScale) {
        sandbox_->matrix_ = std::nullopt;
        return;
    }
    Drawing::Matrix matrix = rootMatrix.value();
    matrix.PreTranslate(sandbox_->position_->x_, sandbox_->position_->y_);
    sandbox_->matrix_ = matrix;
}

std::optional<Drawing::Matrix> RSProperties::GetSandBoxMatrix() const
{
    return sandbox_ ? sandbox_->matrix_ : std::nullopt;
}

void RSProperties::SetPositionZ(float positionZ)
{
    boundsGeo_->SetZ(positionZ);
    frameGeo_.SetZ(positionZ);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPositionZApplicableCamera3D(bool isApplicable)
{
    boundsGeo_->SetZApplicableCamera3D(isApplicable);
    geoDirty_ = true;
    SetDirty();
}

bool RSProperties::GetPositionZApplicableCamera3D() const
{
    return boundsGeo_->GetZApplicableCamera3D();
}

void RSProperties::SetPivot(Vector2f pivot)
{
    boundsGeo_->SetPivot(pivot.x_, pivot.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPivotX(float pivotX)
{
    boundsGeo_->SetPivotX(pivotX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPivotY(float pivotY)
{
    boundsGeo_->SetPivotY(pivotY);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPivotZ(float pivotZ)
{
    boundsGeo_->SetPivotZ(pivotZ);
    geoDirty_ = true;
    SetDirty();
}

float RSProperties::GetPivotX() const
{
    return boundsGeo_->GetPivotX();
}

float RSProperties::GetPivotY() const
{
    return boundsGeo_->GetPivotY();
}

float RSProperties::GetPivotZ() const
{
    return boundsGeo_->GetPivotZ();
}

void RSProperties::SetCornerRadius(const Vector4f& cornerRadius)
{
    cornerRadius_ = cornerRadius;
    SetDirty();
    if (IsShadowMaskValid()) {
        filterNeedUpdate_ = true;
    }
    contentDirty_ = true;
}

const Vector4f& RSProperties::GetCornerRadius() const
{
    return cornerRadius_ ? cornerRadius_.value() : Vector4fZero;
}

void RSProperties::SetCornerApplyType(int type)
{
    cornerApplyType_ = std::clamp<int>(type, 0, static_cast<int>(RSCornerApplyType::MAX));
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::NeedCornerOptimization() const
{
    return cornerApplyType_ != static_cast<int>(RSCornerApplyType::FAST);
}

void RSProperties::SetQuaternion(Quaternion quaternion)
{
    boundsGeo_->SetQuaternion(quaternion);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotation(float degree)
{
    boundsGeo_->SetRotation(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotationX(float degree)
{
    boundsGeo_->SetRotationX(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotationY(float degree)
{
    boundsGeo_->SetRotationY(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetCameraDistance(float cameraDistance)
{
    boundsGeo_->SetCameraDistance(cameraDistance);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScale(Vector2f scale)
{
    boundsGeo_->SetScale(scale.x_, scale.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScaleZ(float sz)
{
    boundsGeo_->SetScaleZ(sz);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScaleX(float sx)
{
    boundsGeo_->SetScaleX(sx);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScaleY(float sy)
{
    boundsGeo_->SetScaleY(sy);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetSkew(const Vector3f& skew)
{
    boundsGeo_->SetSkew(skew.x_, skew.y_, skew.z_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetSkewX(float skewX)
{
    boundsGeo_->SetSkewX(skewX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetSkewY(float skewY)
{
    boundsGeo_->SetSkewY(skewY);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetSkewZ(float skewZ)
{
    boundsGeo_->SetSkewZ(skewZ);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPersp(const Vector4f& persp)
{
    boundsGeo_->SetPersp(persp.x_, persp.y_, persp.z_, persp.w_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPerspX(float perspX)
{
    boundsGeo_->SetPerspX(perspX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPerspY(float perspY)
{
    boundsGeo_->SetPerspY(perspY);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPerspZ(float perspZ)
{
    boundsGeo_->SetPerspZ(perspZ);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPerspW(float perspW)
{
    boundsGeo_->SetPerspW(perspW);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslate(Vector2f translate)
{
    boundsGeo_->SetTranslateX(translate[0]);
    boundsGeo_->SetTranslateY(translate[1]);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateX(float translate)
{
    boundsGeo_->SetTranslateX(translate);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateY(float translate)
{
    boundsGeo_->SetTranslateY(translate);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateZ(float translate)
{
    boundsGeo_->SetTranslateZ(translate);
    geoDirty_ = true;
    SetDirty();
}

float RSProperties::GetCameraDistance() const
{
    return boundsGeo_->GetCameraDistance();
}

float RSProperties::GetSkewX() const
{
    return boundsGeo_->GetSkewX();
}

float RSProperties::GetSkewY() const
{
    return boundsGeo_->GetSkewY();
}

float RSProperties::GetSkewZ() const
{
    return boundsGeo_->GetSkewZ();
}

float RSProperties::GetPerspX() const
{
    return boundsGeo_->GetPerspX();
}

float RSProperties::GetPerspY() const
{
    return boundsGeo_->GetPerspY();
}

float RSProperties::GetPerspZ() const
{
    return boundsGeo_->GetPerspZ();
}

float RSProperties::GetPerspW() const
{
    return boundsGeo_->GetPerspW();
}

void RSProperties::SetParticles(const RSRenderParticleVector& particles)
{
    GetEffect().particles_ = particles;
    if (particles.GetParticleSize() > 0) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

const RSRenderParticleVector& RSProperties::GetParticles() const
{
    static const RSRenderParticleVector defaultValue = RSRenderParticleVector();
    if (effect_) {
        return effect_->particles_;
    }
    return defaultValue;
}

void RSProperties::SetAlpha(float alpha)
{
    alpha_ = alpha;
    if (alpha_ < 1.f) {
        alphaNeedApply_ = true;
    }
    SetDirty();
    // The alpha value of parent node affects all child node
    subTreeAllDirty_ = true;
}

void RSProperties::SetAlphaOffscreen(bool alphaOffscreen)
{
    alphaOffscreen_ = alphaOffscreen;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::GetAlphaOffscreen() const
{
    return alphaOffscreen_;
}

void RSProperties::SetLocalMagnificationCap(bool localMagnificationCap)
{
    localMagnificationCap_ = localMagnificationCap;
    if (localMagnificationCap_) {
        filterNeedUpdate_ = true;
    }
}

void RSProperties::SetSublayerTransform(const std::optional<Matrix3f>& sublayerTransform)
{
    sublayerTransform_ = sublayerTransform;
    SetDirty();
}

const std::optional<Matrix3f>& RSProperties::GetSublayerTransform() const
{
    return sublayerTransform_;
}

// foreground properties
void RSProperties::SetForegroundColor(Color color)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->foregroundColor_ = color;
    SetDirty();
    contentDirty_ = true;
}

Color RSProperties::GetForegroundColor() const
{
    return decoration_ ? decoration_->foregroundColor_ : RgbPalette::Transparent();
}

// background properties
void RSProperties::SetBackgroundColor(Color color)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    if (color.GetAlpha() > 0) {
        isDrawn_ = true;
    }
    decoration_->backgroundColor_ = color;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    if (shader) {
        isDrawn_ = true;
    }
    decoration_->bgShader_ = shader;
    bgShaderNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSShader> RSProperties::GetBackgroundShader() const
{
    return decoration_ ? decoration_->bgShader_ : nullptr;
}

void RSProperties::SetBackgroundShaderProgress(const float& progress)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    isDrawn_ = true;
    decoration_->bgShaderProgress_ = progress;
    bgShaderNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBackgroundShaderProgress() const
{
    return decoration_ ? decoration_->bgShaderProgress_ : -1.f;
}


void RSProperties::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    if (image) {
        isDrawn_ = true;
    }
    decoration_->bgImage_ = image;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSImage> RSProperties::GetBgImage() const
{
    return decoration_ ? decoration_->bgImage_ : nullptr;
}

void RSProperties::SetBgImageInnerRect(const Vector4f& rect)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageInnerRect_ = rect;
    SetDirty();
    contentDirty_ = true;
}

Vector4f RSProperties::GetBgImageInnerRect() const
{
    return decoration_ ? decoration_->bgImageInnerRect_ : Vector4f();
}

void RSProperties::SetBgImageDstRect(const Vector4f& rect)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_ = RectF(rect);
    SetDirty();
    contentDirty_ = true;
}

const Vector4f RSProperties::GetBgImageDstRect()
{
    RectF rect = decoration_ ? decoration_->bgImageRect_ : EMPTY_RECT;
    return Vector4f(rect.left_, rect.top_, rect.width_, rect.height_);
}

const RectF& RSProperties::GetBgImageRect() const
{
    return decoration_ ? decoration_->bgImageRect_ : EMPTY_RECT;
}

void RSProperties::SetBgImageWidth(float width)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.width_ = width;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBgImageHeight(float height)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.height_ = height;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBgImagePositionX(float positionX)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.left_ = positionX;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBgImagePositionY(float positionY)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.top_ = positionY;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBgImageWidth() const
{
    return decoration_ ? decoration_->bgImageRect_.width_ : 0.f;
}

float RSProperties::GetBgImageHeight() const
{
    return decoration_ ? decoration_->bgImageRect_.height_ : 0.f;
}

float RSProperties::GetBgImagePositionX() const
{
    return decoration_ ? decoration_->bgImageRect_.left_ : 0.f;
}

float RSProperties::GetBgImagePositionY() const
{
    return decoration_ ? decoration_->bgImageRect_.top_ : 0.f;
}

// border properties
void RSProperties::SetBorderColor(Vector4<Color> color)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetColorFour(color);
    if (border_->GetColor().GetAlpha() > 0) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBorderWidth(Vector4f width)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetWidthFour(width);
    if (!width.IsZero()) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBorderStyle(Vector4<uint32_t> style)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetStyleFour(style);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBorderDashWidth(const Vector4f& dashWidth)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetDashWidthFour(dashWidth);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBorderDashGap(const Vector4f& dashGap)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetDashGapFour(dashGap);
    SetDirty();
    contentDirty_ = true;
}

Vector4<Color> RSProperties::GetBorderColor() const
{
    return border_ ? border_->GetColorFour() : Vector4<Color>(RgbPalette::Transparent());
}

Vector4f RSProperties::GetBorderWidth() const
{
    return border_ ? border_->GetWidthFour() : Vector4f(0.f);
}

Vector4<uint32_t> RSProperties::GetBorderStyle() const
{
    return border_ ? border_->GetStyleFour() : Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::SOLID));
}

Vector4f RSProperties::GetBorderDashWidth() const
{
    return border_ ? border_->GetDashWidthFour() : Vector4f(0.f);
}

Vector4f RSProperties::GetBorderDashGap() const
{
    return border_ ? border_->GetDashGapFour() : Vector4f(0.f);
}

const std::shared_ptr<RSBorder>& RSProperties::GetBorder() const
{
    return border_;
}

void RSProperties::SetOutlineColor(Vector4<Color> color)
{
    if (!outline_) {
        outline_ = std::make_shared<RSBorder>(true);
    }
    outline_->SetColorFour(color);
    if (outline_->GetColor().GetAlpha() > 0) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOutlineWidth(Vector4f width)
{
    if (!outline_) {
        outline_ = std::make_shared<RSBorder>(true);
    }
    outline_->SetWidthFour(width);
    if (!width.IsZero()) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOutlineStyle(Vector4<uint32_t> style)
{
    if (!outline_) {
        outline_ = std::make_shared<RSBorder>(true);
    }
    outline_->SetStyleFour(style);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOutlineDashWidth(const Vector4f& dashWidth)
{
    if (!outline_) {
        outline_ = std::make_shared<RSBorder>();
    }
    outline_->SetDashWidthFour(dashWidth);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOutlineDashGap(const Vector4f& dashGap)
{
    if (!outline_) {
        outline_ = std::make_shared<RSBorder>();
    }
    outline_->SetDashGapFour(dashGap);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOutlineRadius(Vector4f radius)
{
    if (!outline_) {
        outline_ = std::make_shared<RSBorder>(true);
    }
    outline_->SetRadiusFour(radius);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4<Color> RSProperties::GetOutlineColor() const
{
    return outline_ ? outline_->GetColorFour() : Vector4<Color>(RgbPalette::Transparent());
}

Vector4f RSProperties::GetOutlineWidth() const
{
    return outline_ ? outline_->GetWidthFour() : Vector4f(0.f);
}

Vector4<uint32_t> RSProperties::GetOutlineStyle() const
{
    return outline_ ? outline_->GetStyleFour() : Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE));
}

Vector4f RSProperties::GetOutlineDashWidth() const
{
    return outline_ ? outline_->GetDashWidthFour() : Vector4f(0.f);
}

Vector4f RSProperties::GetOutlineDashGap() const
{
    return outline_ ? outline_->GetDashGapFour() : Vector4f(0.f);
}

Vector4f RSProperties::GetOutlineRadius() const
{
    return outline_ ? outline_->GetRadiusFour() : Vector4fZero;
}

void RSProperties::SetForegroundEffectRadius(const float foregroundEffectRadius)
{
    GetEffect().foregroundEffectRadius_ = foregroundEffectRadius;
    if (IsForegroundEffectRadiusValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
}

float RSProperties::GetForegroundEffectRadius() const
{
    if (effect_) {
        return effect_->foregroundEffectRadius_;
    }
    return 0.f;
}

bool RSProperties::IsForegroundEffectRadiusValid() const
{
    return ROSEN_GNE(GetForegroundEffectRadius(), 0.999f); // if blur radius < 1, no need to draw
}

void RSProperties::SetForegroundEffectDirty(bool dirty)
{
    foregroundEffectDirty_ = dirty;
}

bool RSProperties::GetForegroundEffectDirty() const
{
    return foregroundEffectDirty_;
}

void RSProperties::SetForegroundFilterCache(const std::shared_ptr<RSFilter>& foregroundFilterCache)
{
    foregroundFilterCache_ = foregroundFilterCache;
    if (foregroundFilterCache) {
        isDrawn_ = true;
    }
    SetDirty();
    filterNeedUpdate_ = true;
    contentDirty_ = true;
}

void RSProperties::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    GetEffect().linearGradientBlurPara_ = para;
    if (para && para->blurRadius_ > 0.f) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para)
{
    GetEffect().emitterUpdater_ = para;
    if (!para.empty()) {
        isDrawn_ = true;
        auto renderNode = backref_.lock();
        if (renderNode == nullptr) {
            return;
        }
        auto animation = renderNode->GetAnimationManager().GetParticleAnimation();
        if (animation == nullptr) {
            return;
        }
        auto particleAnimation = std::static_pointer_cast<RSRenderParticleAnimation>(animation);
        if (particleAnimation != nullptr) {
            particleAnimation->UpdateEmitter(para);
        }
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para)
{
    GetEffect().particleNoiseFields_ = para;
    if (para) {
        isDrawn_ = true;
        auto renderNode = backref_.lock();
        if (renderNode == nullptr) {
            return;
        }
        auto animation = renderNode->GetAnimationManager().GetParticleAnimation();
        if (animation == nullptr) {
            return;
        }
        auto particleAnimation = std::static_pointer_cast<RSRenderParticleAnimation>(animation);
        if (particleAnimation) {
            particleAnimation->UpdateNoiseField(para);
        }
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetParticleRippleFields(const std::shared_ptr<ParticleRippleFields>& para)
{
    particleRippleFields_ = para;
    if (particleRippleFields_) {
        isDrawn_ = true;
        auto renderNode = backref_.lock();
        if (renderNode == nullptr) {
            return;
        }
        auto animation = renderNode->GetAnimationManager().GetParticleAnimation();
        if (animation == nullptr) {
            return;
        }
        auto particleAnimation = std::static_pointer_cast<RSRenderParticleAnimation>(animation);
        if (particleAnimation) {
            particleAnimation->UpdateRippleField(particleRippleFields_);
        }
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetParticleVelocityFields(const std::shared_ptr<ParticleVelocityFields>& para)
{
    particleVelocityFields_ = para;
    if (particleVelocityFields_) {
        isDrawn_ = true;
        auto renderNode = backref_.lock();
        if (renderNode == nullptr) {
            return;
        }
        auto animation = renderNode->GetAnimationManager().GetParticleAnimation();
        if (animation == nullptr) {
            return;
        }
        auto particleAnimation = std::static_pointer_cast<RSRenderParticleAnimation>(animation);
        if (particleAnimation) {
            particleAnimation->UpdateVelocityField(particleVelocityFields_);
        }
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetColorPickerPlaceholder(int placeholder)
{
    if (!colorPicker_) {
        colorPicker_ = std::make_shared<ColorPickerParam>();
    }
    colorPicker_->placeholder =
        std::clamp(static_cast<ColorPlaceholder>(placeholder), ColorPlaceholder::NONE, ColorPlaceholder::MAX);
    SetDirty();
}

void RSProperties::SetColorPickerStrategy(int strategy)
{
    if (!colorPicker_) {
        colorPicker_ = std::make_shared<ColorPickerParam>();
    }
    colorPicker_->strategy = std::clamp(
        static_cast<ColorPickStrategyType>(strategy), ColorPickStrategyType::NONE, ColorPickStrategyType::MAX);
    SetDirty();
}

void RSProperties::SetColorPickerInterval(int interval)
{
    if (!colorPicker_) {
        colorPicker_ = std::make_shared<ColorPickerParam>();
    }
    static constexpr uint64_t MIN_INTERVAL = 180; // unit: ms
    colorPicker_->interval = std::max(static_cast<uint64_t>(interval), MIN_INTERVAL);
    SetDirty();
}

void RSProperties::SetColorPickerNotifyThreshold(int packedThresholds)
{
    if (!colorPicker_) {
        colorPicker_ = std::make_shared<ColorPickerParam>();
    }
    // Unpack: lower 16 bits = dark threshold, upper 16 bits = light threshold
    uint32_t darkThreshold = static_cast<uint32_t>(packedThresholds & 0xFFFF);
    uint32_t lightThreshold = static_cast<uint32_t>((packedThresholds >> 16) & 0xFFFF);
    colorPicker_->notifyThreshold = {
        std::clamp(darkThreshold, 0u, RGBA_MAX),
        std::clamp(lightThreshold, 0u, RGBA_MAX)
    };
    SetDirty();
}

void RSProperties::SetColorPickerRect(const Vector4f& rect)
{
    if (!colorPicker_) {
        colorPicker_ = std::make_shared<ColorPickerParam>();
    }
    // Convert Vector4f [left, top, right, bottom] to Drawing::Rect
    auto effectiveRect = Drawing::Rect(rect.x_, rect.y_, rect.z_, rect.w_);
    colorPicker_->rect = effectiveRect.IsValid() ? std::make_optional(effectiveRect) : std::nullopt;
    SetDirty();
}

std::shared_ptr<ColorPickerParam> RSProperties::GetColorPicker() const
{
    return colorPicker_;
}

void RSProperties::SetDynamicLightUpRate(const std::optional<float>& rate)
{
    GetEffect().dynamicLightUpRate_ = rate;
    if (rate.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree)
{
    GetEffect().dynamicLightUpDegree_ = lightUpDegree;
    if (lightUpDegree.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetWaterRippleProgress(const float& progress)
{
    GetEffect().waterRippleProgress_ = progress;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetWaterRippleProgress() const
{
    if (effect_) {
        return effect_->waterRippleProgress_;
    }
    return 0.0f;
}

void RSProperties::SetWaterRippleParams(const std::optional<RSWaterRipplePara>& params)
{
    GetEffect().waterRippleParams_ = params;
    if (params.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::optional<RSWaterRipplePara> RSProperties::GetWaterRippleParams() const
{
    if (effect_) {
        return effect_->waterRippleParams_;
    }
    return std::nullopt;
}

bool RSProperties::IsWaterRippleValid() const
{
    uint32_t WAVE_COUNT_MAX = 3;
    uint32_t WAVE_COUNT_MIN = 1;
    float waterRippleProgress = GetWaterRippleProgress();
    const auto& waterRippleParams = GetWaterRippleParams();
    return ROSEN_GE(waterRippleProgress, 0.0f) && ROSEN_LE(waterRippleProgress, 1.0f) &&
           waterRippleParams.has_value() && waterRippleParams->waveCount >= WAVE_COUNT_MIN &&
           waterRippleParams->waveCount <= WAVE_COUNT_MAX;
}

void RSProperties::SetFlyOutDegree(const float& degree)
{
    GetEffect().flyOutDegree_ = degree;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetFlyOutDegree() const
{
    if (effect_) {
        return effect_->flyOutDegree_;
    }
    return 0.0f;
}

void RSProperties::SetFlyOutParams(const std::optional<RSFlyOutPara>& params)
{
    GetEffect().flyOutParams_ = params;
    if (params.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::optional<RSFlyOutPara> RSProperties::GetFlyOutParams() const
{
    if (effect_) {
        return effect_->flyOutParams_;
    }
    return std::nullopt;
}

bool RSProperties::IsFlyOutValid() const
{
    const auto& flyOutDegree = GetFlyOutDegree();
    return ROSEN_GE(flyOutDegree, 0.0f) && ROSEN_LE(flyOutDegree, 1.0f) && GetFlyOutParams().has_value();
}

void RSProperties::SetDistortionK(const std::optional<float>& distortionK)
{
    GetEffect().distortionK_ = distortionK;
    if (distortionK.has_value()) {
        isDrawn_ = true;
        GetEffect().distortionEffectDirty_ = ROSEN_GNE(*distortionK, 0.0f) && ROSEN_LE(*distortionK, 1.0f);
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::IsDistortionKValid() const
{
    const auto& distortionK = GetDistortionK();
    return distortionK.has_value() && ROSEN_GE(*distortionK, -1.0f) && ROSEN_LE(*distortionK, 1.0f);
}

void RSProperties::SetDistortionDirty(bool distortionEffectDirty)
{
    GetEffect().distortionEffectDirty_ = distortionEffectDirty;
}

bool RSProperties::GetDistortionDirty() const
{
    if (effect_) {
        return effect_->distortionEffectDirty_;
    }
    return false;
}

void RSProperties::SetFgBrightnessRates(const Vector4f& rates)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetEffect().fgBrightnessParams_->rates_ = rates;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4f RSProperties::GetFgBrightnessRates() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams ? fgBrightnessParams->rates_ : Vector4f();
}

void RSProperties::SetFgBrightnessSaturation(const float& saturation)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetEffect().fgBrightnessParams_->saturation_ = saturation;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetFgBrightnessSaturation() const
{
    return GetFgBrightnessParams() ? GetFgBrightnessParams()->saturation_ : 0.0f;
}

void RSProperties::SetFgBrightnessPosCoeff(const Vector4f& coeff)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetEffect().fgBrightnessParams_->posCoeff_ = coeff;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4f RSProperties::GetFgBrightnessPosCoeff() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams ? fgBrightnessParams->posCoeff_ : Vector4f();
}

void RSProperties::SetFgBrightnessNegCoeff(const Vector4f& coeff)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetEffect().fgBrightnessParams_->negCoeff_ = coeff;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4f RSProperties::GetFgBrightnessNegCoeff() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams ? fgBrightnessParams->negCoeff_ : Vector4f();
}

void RSProperties::SetFgBrightnessFract(const float& fraction)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetEffect().fgBrightnessParams_->fraction_ = fraction;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetFgBrightnessFract() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams ? fgBrightnessParams->fraction_ : 1.0f;
}

void RSProperties::SetFgBrightnessHdr(const bool enableHdr)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetEffect().fgBrightnessParams_->enableHdr_ = enableHdr;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::GetFgBrightnessHdr() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams ? fgBrightnessParams->enableHdr_ : false;
}

void RSProperties::SetFgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params)
{
    GetEffect().fgBrightnessParams_ = params;
    if (params.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::optional<RSDynamicBrightnessPara> RSProperties::GetFgBrightnessParams() const
{
    if (effect_) {
        return effect_->fgBrightnessParams_;
    }
    return std::nullopt;
}

bool RSProperties::GetFgBrightnessEnableEDR() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams.has_value() && fgBrightnessParams->enableHdr_ && IsFgBrightnessValid();
}

void RSProperties::SetBgBrightnessRates(const Vector4f& rates)
{
    if (!GetBgBrightnessParams().has_value()) {
        bgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    bgBrightnessParams_->rates_ = rates;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4f RSProperties::GetBgBrightnessRates() const
{
    const auto& bgBrightnessParams = GetBgBrightnessParams();
    return bgBrightnessParams ? bgBrightnessParams->rates_ : Vector4f();
}

void RSProperties::SetBgBrightnessSaturation(const float& saturation)
{
    if (!GetBgBrightnessParams().has_value()) {
        bgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    bgBrightnessParams_->saturation_ = saturation;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBgBrightnessSaturation() const
{
    const auto& bgBrightnessParams = GetBgBrightnessParams();
    return bgBrightnessParams ? bgBrightnessParams->saturation_ : 0.0f;
}

void RSProperties::SetBgBrightnessPosCoeff(const Vector4f& coeff)
{
    if (!GetBgBrightnessParams().has_value()) {
        bgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    bgBrightnessParams_->posCoeff_ = coeff;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4f RSProperties::GetBgBrightnessPosCoeff() const
{
    const auto& bgBrightnessParams = GetBgBrightnessParams();
    return bgBrightnessParams ? bgBrightnessParams->posCoeff_ : Vector4f();
}

void RSProperties::SetBgBrightnessNegCoeff(const Vector4f& coeff)
{
    if (!GetBgBrightnessParams().has_value()) {
        bgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    bgBrightnessParams_->negCoeff_ = coeff;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4f RSProperties::GetBgBrightnessNegCoeff() const
{
    const auto& bgBrightnessParams = GetBgBrightnessParams();
    return bgBrightnessParams ? bgBrightnessParams->negCoeff_ : Vector4f();
}

void RSProperties::SetBgBrightnessFract(const float& fraction)
{
    if (!GetBgBrightnessParams().has_value()) {
        bgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    bgBrightnessParams_->fraction_ = fraction;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBgBrightnessFract() const
{
    const auto& bgBrightnessParams = GetBgBrightnessParams();
    return bgBrightnessParams ? bgBrightnessParams->fraction_ : 1.0f;
}

void RSProperties::SetBgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params)
{
    bgBrightnessParams_ = params;
    if (params.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::optional<RSDynamicBrightnessPara> RSProperties::GetBgBrightnessParams() const
{
    return bgBrightnessParams_;
}

std::string RSProperties::GetFgBrightnessDescription() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    if (!fgBrightnessParams.has_value()) {
        return "fgBrightnessParams_ is nullopt";
    }
    std::string description =
        "ForegroundBrightness, cubicCoeff: " + std::to_string(fgBrightnessParams->rates_.x_) +
        ", quadCoeff: " + std::to_string(fgBrightnessParams->rates_.y_) +
        ", rate: " + std::to_string(fgBrightnessParams->rates_.z_) +
        ", lightUpDegree: " + std::to_string(fgBrightnessParams->rates_.w_) +
        ", saturation: " + std::to_string(fgBrightnessParams->saturation_) +
        ", fgBrightnessFract: " + std::to_string(fgBrightnessParams->fraction_) +
        ", fgBrightnessHdr: " + std::to_string(fgBrightnessParams->enableHdr_);
    return description;
}

std::string RSProperties::GetBgBrightnessDescription() const
{
    const auto& bgBrightnessParams = GetBgBrightnessParams();
    if (!bgBrightnessParams.has_value()) {
        return "bgBrightnessParams_ is nullopt";
    }
    std::string description =
        "BackgroundBrightnessInternal, cubicCoeff: " + std::to_string(bgBrightnessParams->rates_.x_) +
        ", quadCoeff: " + std::to_string(bgBrightnessParams->rates_.y_) +
        ", rate: " + std::to_string(bgBrightnessParams->rates_.z_) +
        ", lightUpDegree: " + std::to_string(bgBrightnessParams->rates_.w_) +
        ", saturation: " + std::to_string(bgBrightnessParams->saturation_) +
        ", bgBrightnessFract: " + std::to_string(bgBrightnessParams->fraction_) +
        ", bgBrightnessHdr: " + std::to_string(bgBrightnessParams->enableHdr_);
    return description;
}

void RSProperties::SetShadowBlenderParams(const std::optional<RSShadowBlenderPara>& params)
{
    GetEffect().shadowBlenderParams_ = params;
    if (params.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::optional<RSShadowBlenderPara> RSProperties::GetShadowBlenderParams() const
{
    if (effect_) {
        return effect_->shadowBlenderParams_;
    }
    return std::nullopt;
}

bool RSProperties::IsShadowBlenderValid() const
{
    const auto& shadowBlenderParams = GetShadowBlenderParams();
    return shadowBlenderParams.has_value() && shadowBlenderParams->IsValid();
}

std::string RSProperties::GetShadowBlenderDescription() const
{
    const auto& shadowBlenderParams = GetShadowBlenderParams();
    if (!shadowBlenderParams.has_value()) {
        return "shadowBlenderParams_ is nullopt";
    }
    std::string description =
        "ShadowBlender, cubic: " + std::to_string(shadowBlenderParams->cubic_) +
        ", quadratic: " + std::to_string(shadowBlenderParams->quadratic_) +
        ", linear: " + std::to_string(shadowBlenderParams->linear_) +
        ", constant: " + std::to_string(shadowBlenderParams->constant_);
    return description;
}

void RSProperties::SetGreyCoef(const std::optional<Vector2f>& greyCoef)
{
    GetEffect().greyCoef_ = greyCoef;
    GetEffect().greyCoefNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetDynamicDimDegree(const std::optional<float>& DimDegree)
{
    GetEffect().dynamicDimDegree_ = DimDegree;
    if (DimDegree.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetMotionBlurPara(const std::shared_ptr<MotionBlurParam>& para)
{
    GetEffect().motionBlurPara_ = para;

    if (para && para->radius > 0.f) {
        isDrawn_ = true;
    }
    SetDirty();
    filterNeedUpdate_ = true;
    contentDirty_ = true;
}

void RSProperties::SetMagnifierParams(const std::shared_ptr<RSMagnifierParams>& para)
{
    GetEffect().magnifierPara_ = para;

    if (para) {
        isDrawn_ = true;
    }
    SetDirty();
    filterNeedUpdate_ = true;
    contentDirty_ = true;
}

bool RSProperties::GetMagnifierDirty() const
{
    const auto& magnifierPara = GetMagnifierPara();
    return magnifierPara && ROSEN_GNE(magnifierPara->factor_, 0.f);
}

const std::shared_ptr<RSMagnifierParams>& RSProperties::GetMagnifierPara() const
{
    static const std::shared_ptr<RSMagnifierParams> defaultValue = nullptr;
    if (effect_) {
        return effect_->magnifierPara_;
    }
    return defaultValue;
}

const std::shared_ptr<RSLinearGradientBlurPara>& RSProperties::GetLinearGradientBlurPara() const
{
    static const std::shared_ptr<RSLinearGradientBlurPara> defaultValue = nullptr;
    if (effect_) {
        return effect_->linearGradientBlurPara_;
    }
    return defaultValue;
}

const std::vector<std::shared_ptr<EmitterUpdater>>& RSProperties::GetEmitterUpdater() const
{
    static const std::vector<std::shared_ptr<EmitterUpdater>> defaultValue = {};
    if (effect_) {
        return effect_->emitterUpdater_;
    }
    return defaultValue;
}

const std::shared_ptr<ParticleNoiseFields>& RSProperties::GetParticleNoiseFields() const
{
    static const std::shared_ptr<ParticleNoiseFields> defaultValue = nullptr;
    if (effect_) {
        return effect_->particleNoiseFields_;
    }
    return defaultValue;
}

void RSProperties::IfLinearGradientBlurInvalid()
{
    if (GetLinearGradientBlurPara() != nullptr) {
        bool isValid = ROSEN_GE(GetLinearGradientBlurPara()->blurRadius_, 0.0);
        if (!isValid) {
            WITH_EFFECT(linearGradientBlurPara_.reset());
        }
    }
}

const std::optional<float>& RSProperties::GetDynamicLightUpRate() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->dynamicLightUpRate_;
    }
    return defaultValue;
}

const std::optional<float>& RSProperties::GetDynamicLightUpDegree() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->dynamicLightUpDegree_;
    }
    return defaultValue;
}

const std::optional<float>& RSProperties::GetDynamicDimDegree() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->dynamicDimDegree_;
    }
    return defaultValue;
}

const std::optional<Vector2f>& RSProperties::GetGreyCoef() const
{
    static const std::optional<Vector2f> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->greyCoef_;
    }
    return defaultValue;
}

bool RSProperties::IsDynamicDimValid() const
{
    const auto& dynamicDimDegree = GetDynamicDimDegree();
    return dynamicDimDegree.has_value() &&
           ROSEN_GE(*dynamicDimDegree, 0.0) && ROSEN_LNE(*dynamicDimDegree, 1.0);
}

const std::shared_ptr<MotionBlurParam>& RSProperties::GetMotionBlurPara() const
{
    static const std::shared_ptr<MotionBlurParam> defaultValue = nullptr;
    if (effect_) {
        return effect_->motionBlurPara_;
    }
    return defaultValue;
}

bool RSProperties::IsDynamicLightUpValid() const
{
    const auto& dynamicLightUpRate = GetDynamicLightUpRate();
    const auto& dynamicLightUpDegree = GetDynamicLightUpDegree();
    return dynamicLightUpRate.has_value() && dynamicLightUpDegree.has_value() &&
           ROSEN_GNE(*dynamicLightUpRate, 0.0) && ROSEN_GE(*dynamicLightUpDegree, -1.0) &&
           ROSEN_LE(*dynamicLightUpDegree, 1.0);
}

void RSProperties::SetForegroundFilter(const std::shared_ptr<RSFilter>& foregroundFilter)
{
    foregroundFilter_ = foregroundFilter;
    if (foregroundFilter) {
        isDrawn_ = true;
    }
    SetDirty();
    filterNeedUpdate_ = true;
    contentDirty_ = true;
}

// shadow properties
void RSProperties::SetShadowColor(Color color)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetColor(color);
    if (GetShadowMask() == SHADOW_MASK_STRATEGY::MASK_COLOR_BLUR) {
        filterNeedUpdate_ = true;
    }
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowOffsetX(float offsetX)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetOffsetX(offsetX);
    SetDirty();
    filterNeedUpdate_ = true;
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowOffsetY(float offsetY)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetOffsetY(offsetY);
    SetDirty();
    filterNeedUpdate_ = true;
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowElevation(float elevation)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetElevation(elevation);
    if (GetShadow()->IsValid()) {
        isDrawn_ = true;
    }
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowRadius(float radius)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetRadius(radius);
    if (GetShadow()->IsValid()) {
        isDrawn_ = true;
    }
    SetDirty();
    filterNeedUpdate_ = true;
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowPath(std::shared_ptr<RSPath> shadowPath)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetPath(shadowPath);
    SetDirty();
    if (IsShadowMaskValid()) {
        filterNeedUpdate_ = true;
    }
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowMask(int shadowMask)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetMask(shadowMask);
    SetDirty();
    filterNeedUpdate_ = true;
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowIsFilled(bool shadowIsFilled)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetIsFilled(shadowIsFilled);
    SetDirty();
    if (IsShadowMaskValid()) {
        filterNeedUpdate_ = true;
    }
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowColorStrategy(int shadowColorStrategy)
{
    if (!GetShadow().has_value()) {
        GetEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetEffect().shadow_->SetColorStrategy(shadowColorStrategy);
    SetDirty();
    filterNeedUpdate_ = true;
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}


const Color& RSProperties::GetShadowColor() const
{
    static const auto DEFAULT_SPOT_COLOR_VALUE = Color::FromArgbInt(DEFAULT_SPOT_COLOR);
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetColor() : DEFAULT_SPOT_COLOR_VALUE;
}

float RSProperties::GetShadowOffsetX() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetOffsetX() : DEFAULT_SHADOW_OFFSET_X;
}

float RSProperties::GetShadowOffsetY() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetOffsetY() : DEFAULT_SHADOW_OFFSET_Y;
}

float RSProperties::GetShadowElevation() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetElevation() : 0.f;
}

float RSProperties::GetShadowRadius() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetRadius() : DEFAULT_SHADOW_RADIUS;
}

std::shared_ptr<RSPath> RSProperties::GetShadowPath() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetPath() : nullptr;
}

int RSProperties::GetShadowMask() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetMask() : SHADOW_MASK_STRATEGY::MASK_NONE;
}

bool RSProperties::IsShadowMaskValid() const
{
    const auto& shadow = GetShadow();
    if (!shadow.has_value()) {
        return false;
    }
    return (shadow->GetMask() > SHADOW_MASK_STRATEGY::MASK_NONE) && (
        shadow->GetMask() <= SHADOW_MASK_STRATEGY::MASK_COLOR_BLUR);
}

bool RSProperties::GetShadowIsFilled() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetIsFilled() : false;
}

int RSProperties::GetShadowColorStrategy() const
{
    const auto& shadow = GetShadow();
    return shadow ? shadow->GetColorStrategy() : SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
}

const std::optional<RSShadow>& RSProperties::GetShadow() const
{
    static const std::optional<RSShadow> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->shadow_;
    }
    return defaultValue;
}

bool RSProperties::IsShadowValid() const
{
    return GetShadow() && GetShadow()->IsValid();
}

void RSProperties::SetFrameGravity(Gravity gravity)
{
    if (frameGravity_ != gravity) {
        frameGravity_ = gravity;
        SetDirty();
        contentDirty_ = true;
    }
}

void RSProperties::SetDrawRegion(const std::shared_ptr<RectF>& rect)
{
    drawRegion_ = rect;
    SetDirty();
    geoDirty_ = true;  // since drawRegion affect dirtyRegion, mark it as geoDirty
}

void RSProperties::SetClipRRect(RRect clipRRect)
{
    clipRRect_ = clipRRect;
    if (GetClipToRRect()) {
        isDrawn_ = true;
    }
    SetDirty();
    geoDirty_ = true;  // [planning] all clip ops should be checked
}

void RSProperties::SetClipBounds(const std::shared_ptr<RSPath>& path)
{
    if (path) {
        isDrawn_ = true;
    }
    if (clipPath_ != path) {
        clipPath_ = path;
        SetDirty();
        geoDirty_ = true;  // [planning] all clip ops should be checked
    }
}

void RSProperties::SetClipToBounds(bool clipToBounds)
{
    if (clipToBounds) {
        isDrawn_ = true;
    }
    if (clipToBounds_ != clipToBounds) {
        clipToBounds_ = clipToBounds;
        SetDirty();
        geoDirty_ = true;  // [planning] all clip ops should be checked
    }
}

void RSProperties::SetClipToFrame(bool clipToFrame)
{
    if (clipToFrame) {
        isDrawn_ = true;
    }
    if (clipToFrame_ != clipToFrame) {
        clipToFrame_ = clipToFrame;
        SetDirty();
        geoDirty_ = true;  // [planning] all clip ops should be checked
    }
}

RectF RSProperties::GetLocalBoundsAndFramesRect() const
{
    auto rect = GetBoundsRect();
    if (!clipToBounds_ && !std::isinf(GetFrameWidth()) && !std::isinf(GetFrameHeight())) {
        rect = rect.JoinRect(RectF(GetFrameOffsetX(), GetFrameOffsetY(), GetFrameWidth(), GetFrameHeight()));
    }
    return rect;
}

RectF RSProperties::GetBoundsRect() const
{
    auto rect = RectF();
    if (boundsGeo_->IsEmpty()) {
        if (!std::isinf(GetFrameWidth()) && !std::isinf(GetFrameHeight())) {
            rect = {0, 0, GetFrameWidth(), GetFrameHeight()};
        }
    } else {
        if (!std::isinf(GetBoundsWidth()) && !std::isinf(GetBoundsHeight())) {
            rect = {0, 0, GetBoundsWidth(), GetBoundsHeight()};
        }
    }
    return rect;
}

RectF RSProperties::GetFrameRect() const
{
    return {0, 0, GetFrameWidth(), GetFrameHeight()};
}

void RSProperties::SetVisible(bool visible)
{
    if (visible_ != visible) {
        visible_ = visible;
        SetDirty();
        contentDirty_ = true;
    }
    // The visible value of parent node affects all child node
    subTreeAllDirty_ = true;
}

const RRect& RSProperties::GetRRect() const
{
    return rrect_;
}

void RSProperties::GenerateRRect()
{
    RectF rect = GetBoundsRect();
    rrect_ = RRect(rect, GetCornerRadius());
}

RRect RSProperties::GetInnerRRect() const
{
    auto rect = GetBoundsRect();
    Vector4f cornerRadius = GetCornerRadius();
    if (border_) {
        rect.left_ += border_->GetWidth(RSBorder::LEFT);
        rect.top_ += border_->GetWidth(RSBorder::TOP);
        rect.width_ -= border_->GetWidth(RSBorder::LEFT) + border_->GetWidth(RSBorder::RIGHT);
        rect.height_ -= border_->GetWidth(RSBorder::TOP) + border_->GetWidth(RSBorder::BOTTOM);
    }
    RRect rrect = RRect(rect, cornerRadius);
    if (border_) {
        rrect.radius_[0] -= { border_->GetWidth(RSBorder::LEFT), border_->GetWidth(RSBorder::TOP) };
        rrect.radius_[1] -= { border_->GetWidth(RSBorder::RIGHT), border_->GetWidth(RSBorder::TOP) };
        rrect.radius_[2] -= { border_->GetWidth(RSBorder::RIGHT), border_->GetWidth(RSBorder::BOTTOM) };
        rrect.radius_[3] -= { border_->GetWidth(RSBorder::LEFT), border_->GetWidth(RSBorder::BOTTOM) };
    }
    return rrect;
}

bool RSProperties::NeedFilter() const
{
    return needFilter_;
}

bool RSProperties::NeedHwcFilter() const
{
    return needHwcFilter_;
}

bool RSProperties::NeedSkipSubtreeParallel() const
{
    return GetForegroundFilter() || GetMask() || IsColorBlendModeValid();
}

bool RSProperties::NeedClip() const
{
    return clipToBounds_ || clipToFrame_;
}

void RSProperties::SetDirty()
{
    isDirty_ = true;
}

void RSProperties::ResetDirty()
{
    isDirty_ = false;
    geoDirty_ = false;
    contentDirty_ = false;
    subTreeAllDirty_ = false;
}

void RSProperties::RecordCurDirtyStatus()
{
    curIsDirty_ = isDirty_;
    curGeoDirty_ = geoDirty_;
    curContentDirty_ = contentDirty_;
    curSubTreeAllDirty_ = subTreeAllDirty_;
}

void RSProperties::AccumulateDirtyStatus()
{
    isDirty_ = isDirty_ || curIsDirty_;
    geoDirty_ = geoDirty_ || curGeoDirty_;
    contentDirty_ = contentDirty_ || curContentDirty_;
    subTreeAllDirty_ = subTreeAllDirty_ || curSubTreeAllDirty_;
}

bool RSProperties::IsDirty() const
{
    return isDirty_;
}

bool RSProperties::IsGeoDirty() const
{
    return geoDirty_;
}

bool RSProperties::IsCurGeoDirty() const
{
    return curGeoDirty_;
}

bool RSProperties::IsContentDirty() const
{
    return contentDirty_;
}

bool RSProperties::IsSubTreeAllDirty() const
{
    return subTreeAllDirty_;
}

RectI RSProperties::GetDirtyRect() const
{
    RectI dirtyRect;
    auto boundsGeometry = (boundsGeo_);
    if (clipToBounds_ || std::isinf(GetFrameWidth()) || std::isinf(GetFrameHeight())) {
        dirtyRect = boundsGeometry->GetAbsRect();
    } else {
        auto frameRect =
            boundsGeometry->MapAbsRect(RectF(GetFrameOffsetX(), GetFrameOffsetY(), GetFrameWidth(), GetFrameHeight()));
        dirtyRect = boundsGeometry->GetAbsRect().JoinRect(frameRect);
    }
    if (drawRegion_ == nullptr || drawRegion_->IsEmpty()) {
        return dirtyRect;
    } else {
        auto drawRegion = boundsGeo_->MapAbsRect(*drawRegion_);
        // this is used to fix the scene with drawRegion problem, which is need to be optimized
        drawRegion.SetRight(drawRegion.GetRight() + 1);
        drawRegion.SetBottom(drawRegion.GetBottom() + 1);
        drawRegion.SetAll(drawRegion.left_ - 1, drawRegion.top_ - 1,
            drawRegion.width_ + 1, drawRegion.height_ + 1);
        return dirtyRect.JoinRect(drawRegion);
    }
}

RectI RSProperties::GetDirtyRect(RectI& drawRegion) const
{
    RectI dirtyRect;
    if (clipToBounds_ || std::isinf(GetFrameWidth()) || std::isinf(GetFrameHeight())) {
        dirtyRect = boundsGeo_->GetAbsRect();
    } else {
        auto frameRect =
            boundsGeo_->MapAbsRect(RectF(GetFrameOffsetX(), GetFrameOffsetY(), GetFrameWidth(), GetFrameHeight()));
        dirtyRect = boundsGeo_->GetAbsRect().JoinRect(frameRect);
    }
    if (drawRegion_ == nullptr || drawRegion_->IsEmpty()) {
        drawRegion = RectI();
        return dirtyRect;
    } else {
        drawRegion = boundsGeo_->MapAbsRect(*drawRegion_);
        // this is used to fix the scene with drawRegion problem, which is need to be optimized
        drawRegion.SetRight(drawRegion.GetRight() + 1);
        drawRegion.SetBottom(drawRegion.GetBottom() + 1);
        drawRegion.SetAll(drawRegion.left_ - 1, drawRegion.top_ - 1,
            drawRegion.width_ + 1, drawRegion.height_ + 1);
        return dirtyRect.JoinRect(drawRegion);
    }
}

void RSProperties::CheckEmptyBounds()
{
    // [planning] remove this func and fallback to framerect after surfacenode using frame
    if (!hasBounds_) {
        boundsGeo_->SetRect(frameGeo_.GetX(), frameGeo_.GetY(), frameGeo_.GetWidth(), frameGeo_.GetHeight());
    }
}

// mask properties
void RSProperties::SetMask(const std::shared_ptr<RSMask>& mask)
{
    GetEffect().mask_ = mask;
    if (mask) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSMask> RSProperties::GetMask() const
{
    if (effect_) {
        return effect_->mask_;
    }
    return nullptr;
}

void RSProperties::SetBackgroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filterProp)
{
    GetEffect().bgNGRenderFilter_ = filterProp;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSNGRenderFilterBase> RSProperties::GetBackgroundNGFilter() const
{
    if (effect_) {
        return effect_->bgNGRenderFilter_;
    }
    return nullptr;
}

void RSProperties::SetForegroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filterProp)
{
    GetEffect().fgNGRenderFilter_ = filterProp;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSNGRenderFilterBase> RSProperties::GetForegroundNGFilter() const
{
    if (effect_) {
        return effect_->fgNGRenderFilter_;
    }
    return nullptr;
}

void RSProperties::SetCompositingNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter)
{
    GetEffect().cgNGRenderFilter_ = renderFilter;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSNGRenderFilterBase> RSProperties::GetCompositingNGFilter() const
{
    if (effect_) {
        return effect_->cgNGRenderFilter_;
    }
    return nullptr;
}

void RSProperties::SetHDRUIBrightness(float hdrUIBrightness)
{
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(backref_.lock())) {
        bool oldHDRUIStatus = IsHDRUIBrightnessValid();
        bool newHDRUIStatus = ROSEN_GNE(hdrUIBrightness, 1.0f);
        if (oldHDRUIStatus != newHDRUIStatus) {
            node->UpdateHDRStatus(HdrStatus::HDR_UICOMPONENT, newHDRUIStatus);
            if (node->IsOnTheTree()) {
                node->SetHdrNum(newHDRUIStatus,
                    node->GetInstanceRootNodeId(), node->GetScreenNodeId(), HDRComponentType::UICOMPONENT);
                node->UpdateDisplayHDRNodeMap(newHDRUIStatus, node->GetLogicalDisplayNodeId());
            }
        }
    }
    hdrUIBrightness_ = hdrUIBrightness;
    if (IsHDRUIBrightnessValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
}

float RSProperties::GetHDRUIBrightness() const
{
    return hdrUIBrightness_;
}

bool RSProperties::IsHDRUIBrightnessValid() const
{
    return ROSEN_GNE(GetHDRUIBrightness(), 1.0f);
}

void RSProperties::CreateHDRUIBrightnessFilter()
{
    auto hdrUIBrightnessFilter = std::make_shared<RSHDRUIBrightnessFilter>(hdrUIBrightness_);
    if (IS_UNI_RENDER) {
        foregroundFilterCache_ = hdrUIBrightnessFilter;
    } else {
        foregroundFilter_ = hdrUIBrightnessFilter;
    }
}

void RSProperties::SetSpherize(float spherizeDegree)
{
    GetEffect().spherizeDegree_ = spherizeDegree;
    bool isSpherizeValid = spherizeDegree > SPHERIZE_VALID_EPSILON;
    GetEffect().isSpherizeValid_ = isSpherizeValid;
    if (isSpherizeValid) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
}

float RSProperties::GetSpherize() const
{
    if (effect_) {
        return effect_->spherizeDegree_;
    }
    return 0.f;
}

bool RSProperties::IsSpherizeValid() const
{
    if (effect_) {
        return effect_->isSpherizeValid_;
    }
    return false;
}

void RSProperties::CreateFlyOutShaderFilter()
{
    uint32_t flyMode = GetFlyOutParams() ? GetFlyOutParams()->flyMode : 0;
    auto flyOutShaderFilter = std::make_shared<RSFlyOutShaderFilter>(GetFlyOutDegree(), flyMode);
    foregroundFilter_ = flyOutShaderFilter;
}

void RSProperties::CreateSphereEffectFilter()
{
    auto spherizeEffectFilter = std::make_shared<RSSpherizeEffectFilter>(GetSpherize());
    if (IS_UNI_RENDER) {
        foregroundFilterCache_ = spherizeEffectFilter;
    } else {
        foregroundFilter_ = spherizeEffectFilter;
    }
}

void RSProperties::CreateAttractionEffectFilter()
{
    float canvasWidth = GetBoundsRect().GetWidth();
    float canvasHeight = GetBoundsRect().GetHeight();
    Vector2f destinationPoint = GetAttractionDstPoint();
    float windowLeftPoint = GetFramePositionX();
    float windowTopPoint = GetFramePositionY();
    auto attractionEffectFilter = std::make_shared<RSAttractionEffectFilter>(GetAttractionFraction());
    attractionEffectFilter->CalculateWindowStatus(canvasWidth, canvasHeight, destinationPoint);
    attractionEffectFilter->UpdateDirtyRegion(windowLeftPoint, windowTopPoint);
    GetEffect().attractionEffectCurrentDirtyRegion_ = attractionEffectFilter->GetAttractionDirtyRegion();
    foregroundFilter_ = attractionEffectFilter;
}

float RSProperties::GetAttractionFraction() const
{
    if (effect_) {
        return effect_->attractFraction_;
    }
    return 0.f;
}

void RSProperties::SetAttractionDstPoint(Vector2f dstPoint)
{
    GetEffect().attractDstPoint_ = dstPoint;
}

Vector2f RSProperties::GetAttractionDstPoint() const
{
    if (effect_) {
        return effect_->attractDstPoint_;
    }
    return {0.f, 0.f};
}

void RSProperties::SetAttractionFraction(float fraction)
{
    GetEffect().attractFraction_ = fraction;
    bool isAttractionValid = fraction > ATTRACTION_VALID_EPSILON;
    GetEffect().isAttractionValid_ = isAttractionValid;
    if (isAttractionValid) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetHDRBrightnessFactor(float factor)
{
    if (ROSEN_EQ(hdrBrightnessFactor_, factor)) {
        return;
    }
    hdrBrightnessFactor_ = factor;
    auto displayNode = RSBaseRenderNode::ReinterpretCast<RSLogicalDisplayRenderNode>(backref_.lock());
    if (displayNode == nullptr) {
        ROSEN_LOGE("RSProperties::SetHDRBrightnessFactor Invalid displayNode");
        return;
    }
    auto context = displayNode->GetContext().lock();
    if (!context) {
        ROSEN_LOGE("RSProperties::SetHDRBrightnessFactor Invalid context");
        return;
    }
    const auto& hdrNodeMap = displayNode->GetHDRNodeMap();
    for (const auto& [nodeId, _] : hdrNodeMap) {
        auto canvasNode = context->GetNodeMap().GetRenderNode(nodeId);
        if (!canvasNode) {
            RS_LOGD("RSHdrUtil::SetHDRBrightnessFactor canvasNode is not on the tree");
            continue;
        }
        canvasNode->SetContentDirty();
        canvasNode->GetMutableRenderProperties().SetCanvasNodeHDRBrightnessFactor(factor);
    }
}

void RSProperties::SetCanvasNodeHDRBrightnessFactor(float factor)
{
    canvasNodeHDRBrightnessFactor_ = factor;
}

float RSProperties::GetCanvasNodeHDRBrightnessFactor() const
{
    return canvasNodeHDRBrightnessFactor_;
}

void RSProperties::SetLightUpEffect(float lightUpEffectDegree)
{
    GetEffect().lightUpEffectDegree_ = lightUpEffectDegree;
    if (IsLightUpEffectValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetLightUpEffect() const
{
    if (effect_) {
        return effect_->lightUpEffectDegree_;
    }
    return 1.0f;
}

bool RSProperties::IsLightUpEffectValid() const
{
    return ROSEN_GE(GetLightUpEffect(), 0.0) && ROSEN_LNE(GetLightUpEffect(), 1.0);
}

// filter property
void RSProperties::SetBackgroundBlurRadius(float backgroundBlurRadius)
{
    GetEffect().backgroundBlurRadius_ = backgroundBlurRadius;
    if (IsBackgroundBlurRadiusValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBackgroundBlurRadius() const
{
    if (effect_) {
        return effect_->backgroundBlurRadius_;
    }
    return 0.f;
}

bool RSProperties::IsBackgroundBlurRadiusValid() const
{
    return ROSEN_GNE(GetBackgroundBlurRadius(), 0.9f); // Adjust the materialBlur radius to 0.9 for the spring curve
}

void RSProperties::SetBackgroundBlurSaturation(float backgroundBlurSaturation)
{
    GetEffect().backgroundBlurSaturation_ = backgroundBlurSaturation;
    if (IsBackgroundBlurSaturationValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBackgroundBlurSaturation() const
{
    if (effect_) {
        return effect_->backgroundBlurSaturation_;
    }
    return 1.f;
}

bool RSProperties::IsBackgroundBlurSaturationValid() const
{
    return (!ROSEN_EQ(GetBackgroundBlurSaturation(), 1.0f)) && ROSEN_GE(GetBackgroundBlurSaturation(), 0.0f);
}

void RSProperties::SetBackgroundBlurBrightness(float backgroundBlurBrightness)
{
    GetEffect().backgroundBlurBrightness_ = backgroundBlurBrightness;
    if (IsBackgroundBlurBrightnessValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBackgroundBlurBrightness() const
{
    if (effect_) {
        return effect_->backgroundBlurBrightness_;
    }
    return 1.f;
}

bool RSProperties::IsBackgroundBlurBrightnessValid() const
{
    return (!ROSEN_EQ(GetBackgroundBlurBrightness(), 1.0f)) && ROSEN_GE(GetBackgroundBlurBrightness(), 0.0f);
}

void RSProperties::SetBackgroundBlurMaskColor(Color backgroundMaskColor)
{
    GetEffect().backgroundMaskColor_ = backgroundMaskColor;
    if (IsBackgroundBlurMaskColorValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const Color& RSProperties::GetBackgroundBlurMaskColor() const
{
    static const Color defaultValue = RSColor();
    if (effect_) {
        return effect_->backgroundMaskColor_;
    }
    return defaultValue;
}

bool RSProperties::IsBackgroundBlurMaskColorValid() const
{
    return GetBackgroundBlurMaskColor() != RSColor();
}

void RSProperties::SetBackgroundBlurColorMode(int backgroundColorMode)
{
    GetEffect().backgroundColorMode_ = backgroundColorMode;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

int RSProperties::GetBackgroundBlurColorMode() const
{
    if (effect_) {
        return effect_->backgroundColorMode_;
    }
    return BLUR_COLOR_MODE::DEFAULT;
}

void RSProperties::SetBackgroundBlurRadiusX(float backgroundBlurRadiusX)
{
    GetEffect().backgroundBlurRadiusX_ = backgroundBlurRadiusX;
    if (IsBackgroundBlurRadiusXValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBackgroundBlurRadiusX() const
{
    if (effect_) {
        return effect_->backgroundBlurRadiusX_;
    }
    return 0.f;
}

bool RSProperties::IsBackgroundBlurRadiusXValid() const
{
    return ROSEN_GNE(GetBackgroundBlurRadiusX(), 0.999f);
}

void RSProperties::SetBackgroundBlurRadiusY(float backgroundBlurRadiusY)
{
    GetEffect().backgroundBlurRadiusY_ = backgroundBlurRadiusY;
    if (IsBackgroundBlurRadiusYValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBackgroundBlurRadiusY() const
{
    if (effect_) {
        return effect_->backgroundBlurRadiusY_;
    }
    return 0.f;
}

bool RSProperties::IsBackgroundBlurRadiusYValid() const
{
    return ROSEN_GNE(GetBackgroundBlurRadiusY(), 0.999f);
}

void RSProperties::SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    GetEffect().bgBlurDisableSystemAdaptation = disableSystemAdaptation;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::GetBgBlurDisableSystemAdaptation() const
{
    if (effect_) {
        return effect_->bgBlurDisableSystemAdaptation;
    }
    return true;
}

void RSProperties::SetForegroundBlurRadius(float foregroundBlurRadius)
{
    GetEffect().foregroundBlurRadius_ = foregroundBlurRadius;
    if (IsForegroundBlurRadiusValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetForegroundBlurRadius() const
{
    if (effect_) {
        return effect_->foregroundBlurRadius_;
    }
    return 0.f;
}

bool RSProperties::IsForegroundBlurRadiusValid() const
{
    return ROSEN_GNE(GetForegroundBlurRadius(), 0.9f); // Adjust the materialBlur radius to 0.9 for the spring curve
}

void RSProperties::SetForegroundBlurSaturation(float foregroundBlurSaturation)
{
    GetEffect().foregroundBlurSaturation_ = foregroundBlurSaturation;
    if (IsForegroundBlurSaturationValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetForegroundBlurSaturation() const
{
    if (effect_) {
        return effect_->foregroundBlurSaturation_;
    }
    return 1.f;
}

bool RSProperties::IsForegroundBlurSaturationValid() const
{
    return ROSEN_GE(GetForegroundBlurSaturation(), 1.0);
}

void RSProperties::SetForegroundBlurBrightness(float foregroundBlurBrightness)
{
    GetEffect().foregroundBlurBrightness_ = foregroundBlurBrightness;
    if (IsForegroundBlurBrightnessValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetForegroundBlurBrightness() const
{
    if (effect_) {
        return effect_->foregroundBlurBrightness_;
    }
    return 1.f;
}

bool RSProperties::IsForegroundBlurBrightnessValid() const
{
    return ROSEN_GE(GetForegroundBlurBrightness(), 1.0);
}

void RSProperties::SetForegroundBlurMaskColor(Color foregroundMaskColor)
{
    GetEffect().foregroundMaskColor_ = foregroundMaskColor;
    if (IsForegroundBlurMaskColorValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const Color& RSProperties::GetForegroundBlurMaskColor() const
{
    static const Color defaultValue = RSColor();
    if (effect_) {
        return effect_->foregroundMaskColor_;
    }
    return defaultValue;
}

bool RSProperties::IsForegroundBlurMaskColorValid() const
{
    return GetForegroundBlurMaskColor() != RSColor();
}

void RSProperties::SetForegroundBlurColorMode(int foregroundColorMode)
{
    GetEffect().foregroundColorMode_ = foregroundColorMode;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

int RSProperties::GetForegroundBlurColorMode() const
{
    if (effect_) {
        return effect_->foregroundColorMode_;
    }
    return BLUR_COLOR_MODE::DEFAULT;
}

void RSProperties::SetForegroundBlurRadiusX(float foregroundBlurRadiusX)
{
    GetEffect().foregroundBlurRadiusX_ = foregroundBlurRadiusX;
    if (IsForegroundBlurRadiusXValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetForegroundBlurRadiusX() const
{
    if (effect_) {
        return effect_->foregroundBlurRadiusX_;
    }
    return 0.f;
}

bool RSProperties::IsForegroundBlurRadiusXValid() const
{
    return ROSEN_GNE(GetForegroundBlurRadiusX(), 0.999f);
}

void RSProperties::SetForegroundBlurRadiusY(float foregroundBlurRadiusY)
{
    GetEffect().foregroundBlurRadiusY_ = foregroundBlurRadiusY;
    if (IsForegroundBlurRadiusYValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetForegroundBlurRadiusY() const
{
    if (effect_) {
        return effect_->foregroundBlurRadiusY_;
    }
    return 0.f;
}

bool RSProperties::IsForegroundBlurRadiusYValid() const
{
    return ROSEN_GNE(GetForegroundBlurRadiusY(), 0.999f);
}

void RSProperties::SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    GetEffect().fgBlurDisableSystemAdaptation = disableSystemAdaptation;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::GetFgBlurDisableSystemAdaptation() const
{
    if (effect_) {
        return effect_->fgBlurDisableSystemAdaptation;
    }
    return true;
}

void RSProperties::SetAlwaysSnapshot(bool enable)
{
    GetEffect().alwaysSnapshot_ = enable;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::GetAlwaysSnapshot() const
{
    if (effect_) {
        return effect_->alwaysSnapshot_;
    }
    return false;
}

void RSProperties::SetComplexShaderParam(const std::vector<float> &param)
{
    GetEffect().complexShaderParam_ = param;
    if (!param.empty()) {
        isDrawn_ = true;
    }
    bgShaderNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::optional<std::vector<float>> RSProperties::GetComplexShaderParam() const
{
    if (effect_) {
        return effect_->complexShaderParam_;
    }
    return std::nullopt;
}

bool RSProperties::IsBackgroundMaterialFilterValid() const
{
    return IsBackgroundBlurRadiusValid() || IsBackgroundBlurBrightnessValid() || IsBackgroundBlurSaturationValid();
}

bool RSProperties::IsForegroundMaterialFilterValid() const
{
    return IsForegroundBlurRadiusValid();
}

std::shared_ptr<Drawing::ColorFilter> RSProperties::GetMaterialColorFilter(float sat, float brightness)
{
    float normalizedDegree = brightness - 1.0;
    const float brightnessMat[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };
    Drawing::ColorMatrix cm;
    cm.SetSaturation(sat);
    float cmArray[Drawing::ColorMatrix::MATRIX_SIZE];
    cm.GetArray(cmArray);
    std::shared_ptr<Drawing::ColorFilter> filterCompose =
        Drawing::ColorFilter::CreateComposeColorFilter(cmArray, brightnessMat, Drawing::Clamp::NO_CLAMP);
    return filterCompose;
}

bool RSProperties::NeedLightBlur(bool disableSystemAdaptation)
{
    return blurAdaptiveAdjustEnabled_ && !disableSystemAdaptation &&
        (NeedBlurFuzed() || RSSystemProperties::GetKawaseEnabled());
}

std::shared_ptr<RSFilter> RSProperties::GenerateLightBlurFilter(float radius)
{
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;
    // Lightblur does not take effect when using grayscale
    if (GetGreyCoef().has_value()) {
        ROSEN_LOGD("RSProperties::GenerateLightBlurFilter: Lightblur does not take effect when using grayscale");
    }

    std::shared_ptr<RSLightBlurShaderFilter> lightBlurShaderFilter = std::make_shared<RSLightBlurShaderFilter>(radius);
    if (originalFilter == nullptr) {
        originalFilter = std::make_shared<RSDrawingFilter>(lightBlurShaderFilter);
    } else {
        originalFilter = originalFilter->Compose(lightBlurShaderFilter);
    }
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(radius));
    originalFilter->SetFilterType(RSFilter::BLUR);
    return originalFilter;
}

std::shared_ptr<RSFilter> RSProperties::GenerateMaterialLightBlurFilter(
    std::shared_ptr<Drawing::ColorFilter> colorFilter, uint32_t hash, float radius,
    int colorMode, const RSColor& color)
{
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;
    // Lightblur does not take effect when using grayscale
    if (GetGreyCoef().has_value()) {
        ROSEN_LOGD("RSProperties::GenerateMaterialLightBlurFilter: "
           "Lightblur does not take effect when using grayscale");
    }
    auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    if (originalFilter == nullptr) {
        originalFilter = std::make_shared<RSDrawingFilter>(colorImageFilter, hash);
    } else {
        originalFilter = originalFilter->Compose(colorImageFilter, hash);
    }

    std::shared_ptr<RSLightBlurShaderFilter> lightBlurFilter = std::make_shared<RSLightBlurShaderFilter>(radius);
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(lightBlurFilter));

    if (colorMode == BLUR_COLOR_MODE::PRE_DEFINED) {
        std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
            colorMode, color);
        originalFilter =
            originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(maskColorShaderFilter));
    }

    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(radius));
    originalFilter->SetFilterType(RSFilter::MATERIAL);
    return originalFilter;
}

void RSProperties::GenerateBackgroundBlurFilter()
{
    std::shared_ptr<Drawing::ImageFilter> blurFilter = Drawing::ImageFilter::CreateBlurImageFilter(
        GetBackgroundBlurRadiusX(), GetBackgroundBlurRadiusY(), Drawing::TileMode::CLAMP, nullptr);
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    const auto& backgroundBlurRadiusX_ = GetEffect().backgroundBlurRadiusX_;
    uint32_t hash = hashFunc(&backgroundBlurRadiusX_, sizeof(backgroundBlurRadiusX_), 0);
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    if (NeedLightBlur(GetBgBlurDisableSystemAdaptation())) {
        backgroundFilter_ = GenerateLightBlurFilter(GetBackgroundBlurRadiusX());
        return;
    }

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
        if (GetGreyCoef().has_value()) {
            mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetBackgroundBlurRadiusX(),
                GetGreyCoef()->x_, GetGreyCoef()->y_);
        } else {
            mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetBackgroundBlurRadiusX());
        }
        originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
        originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetBackgroundBlurRadiusX()));
        backgroundFilter_ = originalFilter;
        backgroundFilter_->SetFilterType(RSFilter::BLUR);
        return;
    }

    if (GetGreyCoef().has_value()) {
        const auto& greyCoef_ = GetGreyCoef();
        std::shared_ptr<RSGreyShaderFilter> greyShaderFilter =
            std::make_shared<RSGreyShaderFilter>(greyCoef_->x_, greyCoef_->y_);
        originalFilter = std::make_shared<RSDrawingFilter>(greyShaderFilter);
    }

    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(GetBackgroundBlurRadiusX());
        if (originalFilter == nullptr) {
            originalFilter = std::make_shared<RSDrawingFilter>(kawaseBlurFilter);
        } else {
            originalFilter =
                originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(kawaseBlurFilter));
        }
    } else {
        if (originalFilter == nullptr) {
            originalFilter = std::make_shared<RSDrawingFilter>(blurFilter, hash);
        } else {
            originalFilter = originalFilter->Compose(blurFilter, hash);
        }
    }
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetBackgroundBlurRadiusX()));
    backgroundFilter_ = originalFilter;
    backgroundFilter_->SetFilterType(RSFilter::BLUR);
}

void RSProperties::GenerateBackgroundMaterialBlurFilter()
{
    if (GetBackgroundBlurColorMode() == BLUR_COLOR_MODE::FASTAVERAGE) {
        GetEffect().backgroundColorMode_ = BLUR_COLOR_MODE::AVERAGE;
    }

    float radiusForHash = DecreasePrecision(GetBackgroundBlurRadius());
    float saturationForHash = DecreasePrecision(GetBackgroundBlurSaturation());
    float brightnessForHash = DecreasePrecision(GetBackgroundBlurBrightness());
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    uint32_t hash = hashFunc(&radiusForHash, sizeof(radiusForHash), 0);
    hash = hashFunc(&saturationForHash, sizeof(saturationForHash), hash);
    hash = hashFunc(&brightnessForHash, sizeof(brightnessForHash), hash);

    std::shared_ptr<Drawing::ColorFilter> colorFilter = GetMaterialColorFilter(
        GetBackgroundBlurSaturation(), GetBackgroundBlurBrightness());
    if (NeedLightBlur(GetBgBlurDisableSystemAdaptation())) {
        backgroundFilter_ = GenerateMaterialLightBlurFilter(colorFilter, hash, GetBackgroundBlurRadius(),
            GetBackgroundBlurColorMode(), GetBackgroundBlurMaskColor());
        return;
    }
    std::shared_ptr<Drawing::ImageFilter> blurColorFilter = Drawing::ImageFilter::CreateColorBlurImageFilter(
        *colorFilter, GetBackgroundBlurRadius(), GetBackgroundBlurRadius());

    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        GenerateBackgroundMaterialFuzedBlurFilter();
        return;
    }

    if (GetGreyCoef().has_value()) {
        const auto& greyCoef_ = GetGreyCoef();
        std::shared_ptr<RSGreyShaderFilter> greyShaderFilter =
            std::make_shared<RSGreyShaderFilter>(greyCoef_->x_, greyCoef_->y_);
        originalFilter = std::make_shared<RSDrawingFilter>(greyShaderFilter);
    }

    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(GetBackgroundBlurRadius());
        auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
        originalFilter = originalFilter?
            originalFilter->Compose(colorImageFilter, hash) : std::make_shared<RSDrawingFilter>(colorImageFilter, hash);
        originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(kawaseBlurFilter));
    } else {
        originalFilter = originalFilter?
            originalFilter->Compose(blurColorFilter, hash) : std::make_shared<RSDrawingFilter>(blurColorFilter, hash);
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter =
        std::make_shared<RSMaskColorShaderFilter>(GetBackgroundBlurColorMode(), GetBackgroundBlurMaskColor());
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetBackgroundBlurRadius()));
    originalFilter->SetSaturationForHPS(GetBackgroundBlurSaturation());
    originalFilter->SetBrightnessForHPS(GetBackgroundBlurBrightness());
    backgroundFilter_ = originalFilter;
    backgroundFilter_->SetFilterType(RSFilter::MATERIAL);
}

void RSProperties::GenerateForegroundBlurFilter()
{
    if (NeedLightBlur(GetFgBlurDisableSystemAdaptation())) {
        filter_ = GenerateLightBlurFilter(GetForegroundBlurRadiusX());
        return;
    }

    std::shared_ptr<Drawing::ImageFilter> blurFilter = Drawing::ImageFilter::CreateBlurImageFilter(
        GetForegroundBlurRadiusX(), GetForegroundBlurRadiusY(), Drawing::TileMode::CLAMP, nullptr);
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    const auto& foregroundBlurRadiusX_ = GetEffect().foregroundBlurRadiusX_;
    uint32_t hash = hashFunc(&foregroundBlurRadiusX_, sizeof(foregroundBlurRadiusX_), 0);
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
        if (GetGreyCoef().has_value()) {
            mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetForegroundBlurRadiusX(),
                GetGreyCoef()->x_, GetGreyCoef()->y_);
        } else {
            mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetForegroundBlurRadiusX());
        }
        originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
        originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetForegroundBlurRadiusX()));
        filter_ = originalFilter;
        GetFilter()->SetFilterType(RSFilter::BLUR);
        return;
    }

    if (GetGreyCoef().has_value()) {
        const auto& greyCoef_ = GetGreyCoef();
        std::shared_ptr<RSGreyShaderFilter> greyShaderFilter =
            std::make_shared<RSGreyShaderFilter>(greyCoef_->x_, greyCoef_->y_);
        originalFilter = std::make_shared<RSDrawingFilter>(greyShaderFilter);
    }
    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(GetForegroundBlurRadiusX());
        if (originalFilter == nullptr) {
            originalFilter = std::make_shared<RSDrawingFilter>(kawaseBlurFilter);
        } else {
            originalFilter =
                originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(kawaseBlurFilter));
        }
    } else {
        if (originalFilter == nullptr) {
            originalFilter = std::make_shared<RSDrawingFilter>(blurFilter, hash);
        } else {
            originalFilter = originalFilter->Compose(blurFilter, hash);
        }
    }
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetForegroundBlurRadiusX()));
    filter_ = originalFilter;
    GetFilter()->SetFilterType(RSFilter::BLUR);
}

void RSProperties::GenerateForegroundMaterialBlurFilter()
{
    if (GetForegroundBlurColorMode() == BLUR_COLOR_MODE::FASTAVERAGE) {
        GetEffect().foregroundColorMode_ = BLUR_COLOR_MODE::AVERAGE;
    }
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    const auto& foregroundBlurRadius_ = GetEffect().foregroundBlurRadius_;
    uint32_t hash = hashFunc(&foregroundBlurRadius_, sizeof(foregroundBlurRadius_), 0);
    std::shared_ptr<Drawing::ColorFilter> colorFilter = GetMaterialColorFilter(
        GetForegroundBlurSaturation(), GetForegroundBlurBrightness());
    if (NeedLightBlur(GetFgBlurDisableSystemAdaptation())) {
        filter_ = GenerateMaterialLightBlurFilter(colorFilter, hash, GetForegroundBlurRadius(),
            GetForegroundBlurColorMode(), GetForegroundBlurMaskColor());
        return;
    }
    std::shared_ptr<Drawing::ImageFilter> blurColorFilter =
        Drawing::ImageFilter::CreateColorBlurImageFilter(*colorFilter,
            GetForegroundBlurRadius(), GetForegroundBlurRadius());

    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        GenerateCompositingMaterialFuzedBlurFilter();
        return;
    }

    if (GetGreyCoef().has_value()) {
        const auto& greyCoef_ = GetGreyCoef();
        std::shared_ptr<RSGreyShaderFilter> greyShaderFilter =
            std::make_shared<RSGreyShaderFilter>(greyCoef_->x_, greyCoef_->y_);
        originalFilter = std::make_shared<RSDrawingFilter>(greyShaderFilter);
    }

    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(GetForegroundBlurRadius());
        auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
        originalFilter = originalFilter?
            originalFilter->Compose(colorImageFilter, hash) : std::make_shared<RSDrawingFilter>(colorImageFilter, hash);
        originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(kawaseBlurFilter));
    } else {
        hash = hashFunc(&(GetEffect().foregroundBlurSaturation_), sizeof(GetEffect().foregroundBlurSaturation_), hash);
        hash = hashFunc(&(GetEffect().foregroundBlurBrightness_), sizeof(GetEffect().foregroundBlurBrightness_), hash);
        originalFilter = originalFilter?
            originalFilter->Compose(blurColorFilter, hash) : std::make_shared<RSDrawingFilter>(blurColorFilter, hash);
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        GetForegroundBlurColorMode(), GetForegroundBlurMaskColor());
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetForegroundBlurRadius()));
    originalFilter->SetSaturationForHPS(GetForegroundBlurSaturation());
    originalFilter->SetBrightnessForHPS(GetForegroundBlurBrightness());
    filter_ = originalFilter;
    GetFilter()->SetFilterType(RSFilter::MATERIAL);
}

void RSProperties::GenerateBackgroundMaterialFuzedBlurFilter()
{
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;
    std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
    if (GetGreyCoef().has_value()) {
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetBackgroundBlurRadius(),
            GetGreyCoef()->x_, GetGreyCoef()->y_);
    } else {
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetBackgroundBlurRadius());
    }
    originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    const auto& backgroundBlurRadius_ = GetEffect().backgroundBlurRadius_;
    uint32_t hash = hashFunc(&backgroundBlurRadius_, sizeof(backgroundBlurRadius_), 0);
    std::shared_ptr<Drawing::ColorFilter> colorFilter = GetMaterialColorFilter(
        GetBackgroundBlurSaturation(), GetBackgroundBlurBrightness());
    auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    originalFilter = originalFilter->Compose(colorImageFilter, hash);
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        GetBackgroundBlurColorMode(), GetBackgroundBlurMaskColor());
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetBackgroundBlurRadius()));
    backgroundFilter_ = originalFilter;
    backgroundFilter_->SetFilterType(RSFilter::MATERIAL);
}

void RSProperties::GenerateCompositingMaterialFuzedBlurFilter()
{
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;
    std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
    if (GetGreyCoef().has_value()) {
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetForegroundBlurRadius(),
            GetGreyCoef()->x_, GetGreyCoef()->y_);
    } else {
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(GetForegroundBlurRadius());
    }
    originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    const auto& foregroundBlurRadius_ = GetEffect().foregroundBlurRadius_;
    uint32_t hash = hashFunc(&foregroundBlurRadius_, sizeof(foregroundBlurRadius_), 0);
    std::shared_ptr<Drawing::ColorFilter> colorFilter = GetMaterialColorFilter(
        GetForegroundBlurSaturation(), GetForegroundBlurBrightness());
    auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    originalFilter = originalFilter->Compose(colorImageFilter, hash);
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        GetForegroundBlurColorMode(), GetForegroundBlurMaskColor());
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(GetForegroundBlurRadius()));
    filter_ = originalFilter;
    GetFilter()->SetFilterType(RSFilter::MATERIAL);
}

void RSProperties::GenerateAIBarFilter()
{
    std::vector<float> aiInvertCoef = RSAIBarShaderFilter::GetAiInvertCoef();
    float aiBarRadius = aiInvertCoef[5]; // aiInvertCoef[5] is filter_radius
    std::shared_ptr<Drawing::ImageFilter> blurFilter =
        Drawing::ImageFilter::CreateBlurImageFilter(aiBarRadius, aiBarRadius, Drawing::TileMode::CLAMP, nullptr);
    std::shared_ptr<RSAIBarShaderFilter> aiBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(aiBarShaderFilter);

    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(aiBarRadius);
        originalFilter = originalFilter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(kawaseBlurFilter));
    } else {
#ifdef USE_M133_SKIA
        const auto hashFunc = SkChecksum::Hash32;
#else
        const auto hashFunc = SkOpts::hash;
#endif
        uint32_t hash = hashFunc(&aiBarRadius, sizeof(aiBarRadius), 0);
        originalFilter = originalFilter->Compose(blurFilter, hash);
    }
    backgroundFilter_ = originalFilter;
    backgroundFilter_->SetFilterType(RSFilter::AIBAR);
}

void RSProperties::GenerateLinearGradientBlurFilter()
{
    auto linearBlurFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(GetLinearGradientBlurPara(),
        frameGeo_.GetWidth(), frameGeo_.GetHeight());
    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(linearBlurFilter);

    filter_ = originalFilter;
    GetFilter()->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
}

void RSProperties::GenerateMagnifierFilter()
{
    auto magnifierFilter = std::make_shared<RSMagnifierShaderFilter>(GetMagnifierPara());

    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(magnifierFilter);
    backgroundFilter_ = originalFilter;
    backgroundFilter_->SetFilterType(RSFilter::MAGNIFIER);
}

void RSProperties::GenerateWaterRippleFilter()
{
    const auto& waterRippleParams = GetWaterRippleParams();
    if (!waterRippleParams) {
        return;
    }
    uint32_t waveCount = waterRippleParams->waveCount;
    float rippleCenterX = waterRippleParams->rippleCenterX;
    float rippleCenterY = waterRippleParams->rippleCenterY;
    uint32_t rippleMode = waterRippleParams->rippleMode;
    std::shared_ptr<RSWaterRippleShaderFilter> waterRippleFilter =
        std::make_shared<RSWaterRippleShaderFilter>(GetWaterRippleProgress(), waveCount, rippleCenterX, rippleCenterY,
            rippleMode);
    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(waterRippleFilter);
    if (!GetBackgroundFilter()) {
        backgroundFilter_ = originalFilter;
        backgroundFilter_->SetFilterType(RSFilter::WATER_RIPPLE);
    } else {
        auto backgroudDrawingFilter = std::static_pointer_cast<RSDrawingFilter>(GetBackgroundFilter());
        backgroudDrawingFilter->Compose(waterRippleFilter);
        backgroudDrawingFilter->SetFilterType(RSFilter::COMPOUND_EFFECT);
        backgroundFilter_ = backgroudDrawingFilter;
    }
}

void RSProperties::GenerateAlwaysSnapshotFilter()
{
    std::shared_ptr<RSAlwaysSnapshotShaderFilter> alwaysSnapshotFilter =
        std::make_shared<RSAlwaysSnapshotShaderFilter>();
    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(alwaysSnapshotFilter);
    backgroundFilter_ = originalFilter;
    backgroundFilter_->SetFilterType(RSFilter::ALWAYS_SNAPSHOT);
}

void RSProperties::ComposeNGRenderFilter(
    std::shared_ptr<RSFilter>& originFilter, std::shared_ptr<RSNGRenderFilterBase> filter)
{
    std::shared_ptr<RSDrawingFilter> originDrawingFilter = nullptr;
    if (!originFilter) {
        originDrawingFilter = std::make_shared<RSDrawingFilter>();
    } else {
        originDrawingFilter = std::static_pointer_cast<RSDrawingFilter>(originFilter);
    }
    originDrawingFilter->SetNGRenderFilter(filter);
    if (filter) {
        originDrawingFilter->SetFilterType(RSFilter::COMPOUND_EFFECT);
        if (filter->ContainsType(RSNGEffectType::CONTENT_LIGHT)) {
            Vector3f rotationAngle(boundsGeo_->GetRotationX(), boundsGeo_->GetRotationY(), boundsGeo_->GetRotation());
            RSNGRenderFilterHelper::SetRotationAngle(filter, rotationAngle);
        }
    }
    originFilter = originDrawingFilter;
}

struct FilterCascadeBundleInfo {
    std::string bundleName = "";
    std::string versionName = "";
    int32_t versionCode = 0;
};

enum class ServerXXFilterCascadeType : size_t {
    AIBAR = 0,
    MAGNIFIER,
    BG_MATERIALBLUR,
    BG_BLUR,
    WATERRIPPLE,
    BG_NGFILTER,
    ALWAYSSNAPSHOT,
    LINEARGRADIENTBLUR,
    CP_MATERIALBLUR,
    CP_BLUR,
    MOTIONBLUR,
    FG_BLUR,
    SPHERIZE,
    FLY,
    ATTRACTION,
    SHADOW,
    DISTORTIONK,
    HDRUIBRIGHTNESS,
    FG_NGFILTER,
    COLORADAPTIVE,
    MAX_TYPE
};

enum class ServerFilterFunctionType : uint16_t {
    BACKGROUND_FILTER = 0,
    COMPOSITING_FILTER,
    FOREGROUND_FILTER,
    MAX_TYPE
};

struct ServerXXFilterCascadeParams {
    struct FilterCascadeBundleInfo bundleInfo;
    ServerFilterFunctionType functionType = ServerFilterFunctionType::BACKGROUND_FILTER;
    std::array<uint16_t, static_cast<size_t>(ServerXXFilterCascadeType::MAX_TYPE)> paramCounts = { 0 };
};

void ReportServerXXFilterCascade(ServerXXFilterCascadeParams params)
{
    switch (params.functionType) {
        // background filter
        case ServerFilterFunctionType::BACKGROUND_FILTER: {
            RS_TRACE_NAME("ReportServerXXFilterCascade BackgroundFilter HiSysEventWrite");
#ifdef ROSEN_OHOS
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_SERVER_XXFILTER_CASCADE",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAME", params.bundleInfo.bundleName,
                "VERSION_NAME", params.bundleInfo.versionName, "VERSION_CODE", params.bundleInfo.versionCode,
                "FUNCTION_TYPE", static_cast<uint16_t>(params.functionType),
                "AIBAR_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::AIBAR)],
                "MAGNIFIER_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::MAGNIFIER)],
                "BG_MATERIALBLUR_COUNT",
                params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::BG_MATERIALBLUR)],
                "BG_BLUR_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::BG_BLUR)],
                "WATERRIPPLE_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::WATERRIPPLE)],
                "BG_NGFILTER_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::BG_NGFILTER)],
                "ALWAYSSNAPSHOT_COUNT",
                params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::ALWAYSSNAPSHOT)]);
#endif
            break;
        }
        // compositing filter
        case ServerFilterFunctionType::COMPOSITING_FILTER: {
            RS_TRACE_NAME("ReportServerXXFilterCascade CompositingFilter HiSysEventWrite");
#ifdef ROSEN_OHOS
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_SERVER_XXFILTER_CASCADE",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAME", params.bundleInfo.bundleName,
                "VERSION_NAME", params.bundleInfo.versionName, "VERSION_CODE", params.bundleInfo.versionCode,
                "FUNCTION_TYPE", static_cast<uint16_t>(params.functionType),
                "LINEARGRADIENTBLUR_COUNT",
                params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::LINEARGRADIENTBLUR)],
                "CP_MATERIALBLUR_COUNT",
                params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::CP_MATERIALBLUR)],
                "CP_BLUR_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::CP_BLUR)]);
#endif
            break;
        }
        // foreground filter
        case ServerFilterFunctionType::FOREGROUND_FILTER: {
            RS_TRACE_NAME("ReportServerXXFilterCascade ForegroundFilter HiSysEventWrite");
#ifdef ROSEN_OHOS
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_SERVER_XXFILTER_CASCADE",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAME", params.bundleInfo.bundleName,
                "VERSION_NAME", params.bundleInfo.versionName, "VERSION_CODE", params.bundleInfo.versionCode,
                "FUNCTION_TYPE", static_cast<uint16_t>(params.functionType),
                "MOTIONBLUR_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::MOTIONBLUR)],
                "FG_BLUR_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::FG_BLUR)],
                "SPHERIZE_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::SPHERIZE)],
                "FLY_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::FLY)],
                "ATTRACTION_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::ATTRACTION)],
                "SHADOW_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::SHADOW)],
                "DISTORTIONK_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::DISTORTIONK)],
                "HDRUIBRIGHTNESS_COUNT",
                params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::HDRUIBRIGHTNESS)],
                "FG_NGFILTER_COUNT", params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::FG_NGFILTER)],
                "COLORADAPTIVE_COUNT",
                params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::COLORADAPTIVE)]);
#endif
            break;
        }
        default:
            break;
    }
}

void ReportServerXXFilterCascadeCheck(
    ServerXXFilterCascadeParams params, const std::shared_ptr<RSRenderNode>& renderNode = nullptr)
{
    const int kMaxEventsPerHour = 5;
    const int64_t kHourMs = 60LL * 60LL * 1000LL; // 1 hour
    const int kMaxEventsPerDay = 20;
    const int64_t kDayMs = 24LL * 60LL * 60LL * 1000LL; // 24 hours

    // If caller provides a render node, perform bundle name lookup on background thread
    if (renderNode != nullptr) {
        // Rate limit: at most 5 reports per hour and 20 reports per 24 hours
        static std::mutex sRateMutex;
        static int sEventCountHour = 0;
        static int64_t sWindowStartMsHour = 0;
        static int sEventCountDay = 0;
        static int64_t sWindowStartMsDay = 0;

        std::lock_guard<std::mutex> lock(sRateMutex);
        int64_t nowMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        if (sWindowStartMsHour == 0 || nowMs - sWindowStartMsHour >= kHourMs) {
            sWindowStartMsHour = nowMs;
            sEventCountHour = 0;
        }
        if (sWindowStartMsDay == 0 || nowMs - sWindowStartMsDay >= kDayMs) {
            sWindowStartMsDay = nowMs;
            sEventCountDay = 0;
        }
        if (sEventCountHour >= kMaxEventsPerHour || sEventCountDay >= kMaxEventsPerDay) {
            return;
        }
        ++sEventCountHour;
        ++sEventCountDay;
#ifdef ROSEN_OHOS
        RSBackgroundThread::Instance().PostTask([params, renderNode]() mutable {
            auto nodeId = renderNode->GetId();
            pid_t pid = ExtractPid(nodeId);
            static const auto appMgrClient = std::make_shared<AppExecFwk::AppMgrClient>();
            if (appMgrClient != nullptr) {
                std::string bundleName;
                int32_t uid = 0;
                int32_t ret = appMgrClient->GetBundleNameByPid(pid, bundleName, uid);
                if ((ret == ERR_OK) && !bundleName.empty()) {
                    params.bundleInfo.bundleName = bundleName;
                }
            }
            ReportServerXXFilterCascade(params);
        });
#endif
    }
    return;
}

void RSProperties::StatBackgroundFilter()
{
    ServerXXFilterCascadeParams params;
    params.functionType = ServerFilterFunctionType::BACKGROUND_FILTER;
    if (GetAiInvert().has_value() || GetSystemBarEffect()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::AIBAR)]++;
    }
    if (GetMagnifierPara() && ROSEN_GNE(GetMagnifierPara()->factor_, 0.f)) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::MAGNIFIER)]++;
    }
    if (IsBackgroundMaterialFilterValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::BG_MATERIALBLUR)]++;
    }
    if (IsBackgroundBlurRadiusXValid() && IsBackgroundBlurRadiusYValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::BG_BLUR)]++;
    }
    if (IsWaterRippleValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::WATERRIPPLE)]++;
    }
    if (GetBackgroundNGFilter()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::BG_NGFILTER)]++;
    }
    if (GetAlwaysSnapshot()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::ALWAYSSNAPSHOT)]++;
    }
    if (std::accumulate(params.paramCounts.begin(), params.paramCounts.end(), 0) > 1) {
        auto renderNode = backref_.lock();
        if (renderNode != nullptr) {
            hasReportedServerXXFilterCascade_[static_cast<size_t>(ServerFilterFunctionType::BACKGROUND_FILTER)] = true;
            ReportServerXXFilterCascadeCheck(params, renderNode);
        }
    }
}

void RSProperties::StatCompositingFilter()
{
    ServerXXFilterCascadeParams params;
    params.functionType = ServerFilterFunctionType::COMPOSITING_FILTER;
    if (GetLinearGradientBlurPara()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::LINEARGRADIENTBLUR)]++;
    }
    if (IsForegroundMaterialFilterValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::CP_MATERIALBLUR)]++;
    }
    if (IsForegroundBlurRadiusXValid() && IsForegroundBlurRadiusYValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::CP_BLUR)]++;
    }
    if (std::accumulate(params.paramCounts.begin(), params.paramCounts.end(), 0) > 1) {
        auto renderNode = backref_.lock();
        if (renderNode != nullptr) {
            hasReportedServerXXFilterCascade_[static_cast<size_t>(ServerFilterFunctionType::COMPOSITING_FILTER)] = true;
            ReportServerXXFilterCascadeCheck(params, renderNode);
        }
    }
}

void RSProperties::StatForegroundFilter()
{
    ServerXXFilterCascadeParams params;
    params.functionType = ServerFilterFunctionType::FOREGROUND_FILTER;
    auto motionBlurPara = RSProperties::GetMotionBlurPara();
    if (motionBlurPara && ROSEN_GNE(motionBlurPara->radius, 0.0)) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::MOTIONBLUR)]++;
    }
    if (IsForegroundEffectRadiusValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::FG_BLUR)]++;
    }
    if (IsSpherizeValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::SPHERIZE)]++;
    }
    if (IsFlyOutValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::FLY)]++;
    }
    if (IsAttractionValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::ATTRACTION)]++;
    }
    if (IsShadowMaskValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::SHADOW)]++;
    }
    if (IsDistortionKValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::DISTORTIONK)]++;
    }
    if (IsHDRUIBrightnessValid()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::HDRUIBRIGHTNESS)]++;
    }
    if (GetForegroundNGFilter()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::FG_NGFILTER)]++;
    }
    if (GetColorAdaptive()) {
        params.paramCounts[static_cast<size_t>(ServerXXFilterCascadeType::COLORADAPTIVE)]++;
    }
    if (std::accumulate(params.paramCounts.begin(), params.paramCounts.end(), 0) > 1) {
        auto renderNode = backref_.lock();
        if (renderNode != nullptr) {
            hasReportedServerXXFilterCascade_[static_cast<size_t>(ServerFilterFunctionType::FOREGROUND_FILTER)] = true;
            ReportServerXXFilterCascadeCheck(params, renderNode);
        }
    }
}

void RSProperties::GenerateBackgroundFilter()
{
    if (!hasReportedServerXXFilterCascade_[static_cast<size_t>(ServerFilterFunctionType::BACKGROUND_FILTER)]) {
        StatBackgroundFilter();
    }
    if (GetAiInvert().has_value() || GetSystemBarEffect()) {
        GenerateAIBarFilter();
    } else if (GetMagnifierPara() && ROSEN_GNE(GetMagnifierPara()->factor_, 0.f)) {
        GenerateMagnifierFilter();
    } else if (IsBackgroundMaterialFilterValid()) {
        GenerateBackgroundMaterialBlurFilter();
    } else if (IsBackgroundBlurRadiusXValid() && IsBackgroundBlurRadiusYValid()) {
        GenerateBackgroundBlurFilter();
    } else {
        backgroundFilter_ = nullptr;
    }

    if (IsWaterRippleValid()) {
        GenerateWaterRippleFilter();
    }

    if (GetBackgroundNGFilter()) {
        ComposeNGRenderFilter(backgroundFilter_, GetBackgroundNGFilter());
    }

    if (GetAlwaysSnapshot() && GetBackgroundFilter() == nullptr) {
        GenerateAlwaysSnapshotFilter();
    }
    if (GetBackgroundFilter() == nullptr) {
        ROSEN_LOGD("RSProperties::GenerateBackgroundFilter failed");
    }
}

void RSProperties::GenerateForegroundFilter()
{
    IfLinearGradientBlurInvalid();
    if (!hasReportedServerXXFilterCascade_[static_cast<size_t>(ServerFilterFunctionType::COMPOSITING_FILTER)]) {
        StatCompositingFilter();
    }
    if (GetLinearGradientBlurPara()) {
        GenerateLinearGradientBlurFilter();
    } else if (IsForegroundMaterialFilterValid()) {
        GenerateForegroundMaterialBlurFilter();
    } else if (IsForegroundBlurRadiusXValid() && IsForegroundBlurRadiusYValid()) {
        GenerateForegroundBlurFilter();
    } else {
        filter_ = nullptr;
    }

    if (GetFilter() == nullptr) {
        ROSEN_LOGD("RSProperties::GenerateForegroundFilter failed");
    }
}

void RSProperties::GenerateMaterialFilter()
{
    // not support compose yet, so do not use ComposeNGRenderFilter
    if (!GetMaterialNGFilter()) {
        WITH_EFFECT(materialFilter_ = nullptr);
        return;
    }
    auto filter = std::make_shared<RSDrawingFilter>();
    filter->SetNGRenderFilter(GetMaterialNGFilter());
    filter->SetFilterType(RSFilter::COMPOUND_EFFECT);
    GetEffect().materialFilter_ = filter;
}

void RSProperties::SetUseEffect(bool useEffect)
{
    GetEffect().useEffect_ = useEffect;
    if (GetUseEffect()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
}

bool RSProperties::GetUseEffect() const
{
    if (effect_) {
        return effect_->useEffect_;
    }
    return false;
}

void RSProperties::SetUseEffectType(int useEffectType)
{
    GetEffect().useEffectType_ = std::clamp<int>(useEffectType, 0, static_cast<int>(UseEffectType::MAX));
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

int RSProperties::GetUseEffectType() const
{
    if (effect_) {
        return effect_->useEffectType_;
    }
    return 0;
}

bool RSProperties::HasHarmonium() const
{
    return hasHarmonium_;
}

void RSProperties::SetNeedDrawBehindWindow(bool needDrawBehindWindow)
{
    GetEffect().needDrawBehindWindow_ = needDrawBehindWindow;
    UpdateFilter();
}

void RSProperties::SetUseUnion(bool useUnion)
{
    if (ROSEN_EQ(useUnion_, useUnion)) {
        return;
    }
    useUnion_ = useUnion;
    if (GetUseUnion()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
}

bool RSProperties::GetUseUnion() const
{
    return useUnion_;
}

void RSProperties::SetUnionSpacing(float spacing)
{
    if (ROSEN_EQ(unionSpacing_, spacing)) {
        return;
    }
    unionSpacing_ = spacing;
    geoDirty_ = true;
    contentDirty_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
}

float RSProperties::GetUnionSpacing() const
{
    return unionSpacing_;
}

void RSProperties::SetUseShadowBatching(bool useShadowBatching)
{
    if (useShadowBatching) {
        isDrawn_ = true;
    }

    GetEffect().useShadowBatching_ = useShadowBatching;
    SetDirty();
}

void RSProperties::SetPixelStretch(const std::optional<Vector4f>& stretchSize)
{
    GetEffect().pixelStretch_ = stretchSize;
    SetDirty();
    pixelStretchNeedUpdate_ = true;
    contentDirty_ = true;
    if (GetPixelStretch().has_value() && GetPixelStretch()->IsZero()) {
        WITH_EFFECT(pixelStretch_ = std::nullopt);
    }
}

RectI RSProperties::GetPixelStretchDirtyRect() const
{
    auto dirtyRect = GetDirtyRect();

    // effect_ exist
    const auto& pixelStretch = GetPixelStretch();
    auto scaledBounds = RectF(dirtyRect.left_ - pixelStretch->x_, dirtyRect.top_ - pixelStretch->y_,
        dirtyRect.width_ + pixelStretch->x_ + pixelStretch->z_,
        dirtyRect.height_ + pixelStretch->y_ + pixelStretch->w_);

    auto scaledIBounds = RectI(std::floor(scaledBounds.left_), std::floor(scaledBounds.top_),
        std::ceil(scaledBounds.width_) + 1, std::ceil(scaledBounds.height_) + 1);
    return dirtyRect.JoinRect(scaledIBounds);
}

void RSProperties::SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent)
{
    GetEffect().pixelStretchPercent_ = stretchPercent;
    SetDirty();
    pixelStretchNeedUpdate_ = true;
    contentDirty_ = true;
    const auto& pixelStretchPercent = GetPixelStretchPercent();
    if (pixelStretchPercent.has_value() && pixelStretchPercent->IsZero()) {
        WITH_EFFECT(pixelStretchPercent_ = std::nullopt);
    }
}

void RSProperties::SetPixelStretchTileMode(int stretchTileMode)
{
    GetEffect().pixelStretchTileMode_ = std::clamp<int>(stretchTileMode, static_cast<int>(Drawing::TileMode::CLAMP),
        static_cast<int>(Drawing::TileMode::DECAL));
    SetDirty();
    pixelStretchNeedUpdate_ = true;
    contentDirty_ = true;
}

int RSProperties::GetPixelStretchTileMode() const
{
    if (effect_) {
        return effect_->pixelStretchTileMode_;
    }
    return 0;
}

// Image effect properties
void RSProperties::SetGrayScale(const std::optional<float>& grayScale)
{
    GetEffect().grayScale_ = grayScale;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetLightIntensity(float lightIntensity)
{
    if (!GetLightSource()) {
        GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    if (ROSEN_EQ(lightIntensity, INVALID_INTENSITY)) { // skip when resetFunc call
        return;
    }
    GetLightSource()->SetLightIntensity(lightIntensity);
    SetDirty();
    contentDirty_ = true;
    auto preIntensity = GetLightSource()->GetPreLightIntensity();
    auto renderNode = backref_.lock();
    if (renderNode == nullptr) {
        return;
    }
    bool preIntensityIsZero = ROSEN_EQ(preIntensity, 0.f);
    bool curIntensityIsZero = ROSEN_EQ(lightIntensity, 0.f);
    if (preIntensityIsZero && !curIntensityIsZero) { // 0 --> non-zero
        RSPointLightManager::Instance()->RegisterLightSource(renderNode);
    } else if (!preIntensityIsZero && curIntensityIsZero) { // non-zero --> 0
        RSPointLightManager::Instance()->UnRegisterLightSource(renderNode);
    }
}

void RSProperties::SetLightColor(Color lightColor)
{
    if (!GetLightSource()) {
        GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    GetLightSource()->SetLightColor(lightColor);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetLightPosition(const Vector4f& lightPosition)
{
    if (!GetLightSource()) {
        GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    GetLightSource()->SetLightPosition(lightPosition);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    if (!GetIlluminated()) {
        GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    GetIlluminated()->SetIlluminatedBorderWidth(illuminatedBorderWidth);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetIlluminatedType(int illuminatedType)
{
    if (!GetIlluminated()) {
        GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    auto curIlluminateType = IlluminatedType(illuminatedType);
    if (curIlluminateType == IlluminatedType::INVALID) { // skip when resetFunc call
        return;
    }
    GetIlluminated()->SetIlluminatedType(curIlluminateType);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
    auto renderNode = backref_.lock();
    if (renderNode == nullptr) {
        return;
    }
    auto preIlluminatedType = GetIlluminated()->GetPreIlluminatedType();
    bool preTypeIsNone = preIlluminatedType == IlluminatedType::NONE;
    bool curTypeIsNone = curIlluminateType == IlluminatedType::NONE;
    if (preTypeIsNone && !curTypeIsNone) {
        RSPointLightManager::Instance()->RegisterIlluminated(renderNode);
    } else if (!preTypeIsNone && curTypeIsNone) {
        RSPointLightManager::Instance()->UnRegisterIlluminated(renderNode);
    }
}

void RSProperties::SetBloom(float bloomIntensity)
{
    if (!GetIlluminated()) {
        GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    GetIlluminated()->SetBloomIntensity(bloomIntensity);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetLightIntensity() const
{
    const auto& lightSourcePtr = GetLightSource();
    return lightSourcePtr ? lightSourcePtr->GetLightIntensity() : 0.f;
}

Color RSProperties::GetLightColor() const
{
    const auto& lightSourcePtr = GetLightSource();
    return lightSourcePtr ? lightSourcePtr->GetLightColor() : RgbPalette::White();
}

Vector4f RSProperties::GetLightPosition() const
{
    const auto& lightSourcePtr = GetLightSource();
    return lightSourcePtr ? lightSourcePtr->GetLightPosition() : Vector4f(0.f);
}

int RSProperties::GetIlluminatedType() const
{
    return GetIlluminated() ? static_cast<int>(GetIlluminated()->GetIlluminatedType()) : 0;
}

void RSProperties::SetBrightness(const std::optional<float>& brightness)
{
    GetEffect().brightness_ = brightness;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetBrightness() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->brightness_;
    }
    return defaultValue;
}

void RSProperties::SetContrast(const std::optional<float>& contrast)
{
    GetEffect().contrast_ = contrast;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetContrast() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->contrast_;
    }
    return defaultValue;
}

void RSProperties::SetSaturate(const std::optional<float>& saturate)
{
    GetEffect().saturate_ = saturate;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetSaturate() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->saturate_;
    }
    return defaultValue;
}

void RSProperties::SetSepia(const std::optional<float>& sepia)
{
    GetEffect().sepia_ = sepia;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetSepia() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->sepia_;
    }
    return defaultValue;
}

void RSProperties::SetInvert(const std::optional<float>& invert)
{
    GetEffect().invert_ = invert;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetInvert() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->invert_;
    }
    return defaultValue;
}


void RSProperties::SetAiInvert(const std::optional<Vector4f>& aiInvert)
{
    GetEffect().aiInvert_ = aiInvert;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
    isDrawn_ = true;
}

const std::optional<Vector4f>& RSProperties::GetAiInvert() const
{
    static const std::optional<Vector4f> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->aiInvert_;
    }
    return defaultValue;
}

void RSProperties::SetSystemBarEffect(bool systemBarEffect)
{
    GetEffect().systemBarEffect_ = systemBarEffect;
    colorFilterNeedUpdate_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
    isDrawn_ = true;
}

bool RSProperties::GetSystemBarEffect() const
{
    if (effect_) {
        return effect_->systemBarEffect_;
    }
    return false;
}

void RSProperties::SetHueRotate(const std::optional<float>& hueRotate)
{
    GetEffect().hueRotate_ = hueRotate;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetHueRotate() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->hueRotate_;
    }
    return defaultValue;
}

void RSProperties::SetColorBlend(const std::optional<Color>& colorBlend)
{
    GetEffect().colorBlend_ = colorBlend;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<Color>& RSProperties::GetColorBlend() const
{
    static const std::optional<Color> defaultValue = std::nullopt;
    if (effect_) {
        return effect_->colorBlend_;
    }
    return defaultValue;
}

static bool GreatNotEqual(double left, double right)
{
    constexpr double epsilon = 0.001f;
    return (left - right) > epsilon;
}

static bool NearEqual(const double left, const double right)
{
    constexpr double epsilon = 0.001f;
    return (std::abs(left - right) <= epsilon);
}

static bool GreatOrEqual(double left, double right)
{
    constexpr double epsilon = -0.001f;
    return (left - right) > epsilon;
}

void RSProperties::GenerateColorFilter()
{
    // No update needed if color filter is valid
    if (!colorFilterNeedUpdate_) {
        return;
    }

    colorFilterNeedUpdate_ = false;
    WITH_EFFECT(colorFilter_ = nullptr);
    const auto& grayScale_ = GetGrayScale();
    if (!grayScale_ && !GetBrightness() && !GetContrast() && !GetSaturate() && !GetSepia() &&
        !GetInvert() && !GetHueRotate() && !GetColorBlend()) {
        return;
    }

    std::shared_ptr<Drawing::ColorFilter> filter = nullptr;

    if (grayScale_.has_value() && GreatNotEqual(*grayScale_, 0.f)) {
        auto grayScale = grayScale_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = matrix[INDEX_5] = matrix[INDEX_10] = 0.2126f * grayScale; // 0.2126 : gray scale coefficient
        matrix[1] = matrix[INDEX_6] = matrix[INDEX_11] = 0.7152f * grayScale; // 0.7152 : gray scale coefficient
        matrix[INDEX_2] = matrix[INDEX_7] = matrix[INDEX_12] = 0.0722f * grayScale; // 0.0722 : gray scale coefficient
        matrix[INDEX_18] = 1.0 * grayScale;
        filter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    const auto& brightness_ = GetBrightness();
    if (brightness_.has_value() && !NearEqual(*brightness_, 1.0)) {
        auto brightness = brightness_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        // shift brightness to (-1, 1)
        brightness = brightness - 1;
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = matrix[INDEX_18] = 1.0f;
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = brightness;
        filter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    const auto& contrast_ = GetContrast();
    if (contrast_.has_value() && !NearEqual(*contrast_, 1.0)) {
        auto contrast = contrast_.value();
        uint32_t contrastValue128 = 128;
        uint32_t contrastValue255 = 255;
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = contrast;
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = contrastValue128 * (1 - contrast) / contrastValue255;
        matrix[INDEX_18] = 1.0f;
        filter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    const auto& saturate_ = GetSaturate();
    if (saturate_.has_value() && !NearEqual(*saturate_, 1.0) && GreatOrEqual(*saturate_, 0.0)) {
        auto saturate = saturate_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = 0.3086f * (1 - saturate) + saturate; // 0.3086 : saturate coefficient
        matrix[1] = matrix[INDEX_11] = 0.6094f * (1 - saturate); // 0.6094 : saturate coefficient
        matrix[INDEX_2] = matrix[INDEX_7] = 0.0820f * (1 - saturate); // 0.0820 : saturate coefficient
        matrix[INDEX_5] = matrix[INDEX_10] = 0.3086f * (1 - saturate); // 0.3086 : saturate coefficient
        matrix[INDEX_6] = 0.6094f * (1 - saturate) + saturate; // 0.6094 : saturate coefficient
        matrix[INDEX_12] = 0.0820f * (1 - saturate) + saturate; // 0.0820 : saturate coefficient
        matrix[INDEX_18] = 1.0f;
        filter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    const auto& sepia_ = GetSepia();
    if (sepia_.has_value() && GreatNotEqual(*sepia_, 0.0)) {
        auto sepia = sepia_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = 0.393f * sepia;
        matrix[1] = 0.769f * sepia;
        matrix[INDEX_2] = 0.189f * sepia;

        matrix[INDEX_5] = 0.349f * sepia;
        matrix[INDEX_6] = 0.686f * sepia;
        matrix[INDEX_7] = 0.168f * sepia;

        matrix[INDEX_10] = 0.272f * sepia;
        matrix[INDEX_11] = 0.534f * sepia;
        matrix[INDEX_12] = 0.131f * sepia;
        matrix[INDEX_18] = 1.0f * sepia;
        filter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    const auto& invert_ = GetInvert();
    if (invert_.has_value() && GreatNotEqual(*invert_, 0.0)) {
        auto invert = invert_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        if (invert > 1.0) {
            invert = 1.0;
        }
        // complete color invert when dstRGB = 1 - srcRGB
        // map (0, 1) to (1, -1)
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = 1.0 - 2.0 * invert; // 2.0: invert
        matrix[INDEX_18] = 1.0f;
        // invert = 0.5 -> RGB = (0.5, 0.5, 0.5) -> image completely gray
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = invert;
        filter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    const auto& hueRotate_ = GetHueRotate();
    if (hueRotate_.has_value() && GreatNotEqual(*hueRotate_, 0.0)) {
        auto hueRotate = hueRotate_.value();
        while (GreatOrEqual(hueRotate, 360)) { // 360 : degree
            hueRotate -= 360; // 360 : degree
        }
        float matrix[20] = { 0.0f }; // 20 : matrix size
        int32_t type = hueRotate / 120; // 120 : degree
        float N = (hueRotate - 120 * type) / 120; // 120 : degree
        switch (type) {
            case 0:
                // color change = R->G, G->B, B->R
                matrix[INDEX_2] = matrix[INDEX_5] = matrix[INDEX_11] = N;
                matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            case 1:
                // compare to original: R->B, G->R, B->G
                matrix[1] = matrix[INDEX_7] = matrix[INDEX_10] = N;
                matrix[INDEX_2] = matrix[INDEX_5] = matrix[INDEX_11] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            case 2: // 2: back to normal color
                matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = N;
                matrix[1] = matrix[INDEX_7] = matrix[INDEX_10] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            default:
                break;
        }
        filter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    const auto& colorBlend_ = GetColorBlend();
    if (colorBlend_.has_value() && *colorBlend_ != RgbPalette::Transparent()) {
        auto colorBlend = colorBlend_.value();
        filter = Drawing::ColorFilter::CreateBlendModeColorFilter(colorBlend.AsArgbInt(),
            Drawing::BlendMode::PLUS);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetEffect().colorFilter_ = filter;
    }
    isDrawn_ = true;
}

std::string RSProperties::Dump() const
{
    std::string dumpInfo;
    char buffer[UINT8_MAX] = { 0 };
    if (sprintf_s(buffer, UINT8_MAX, "Bounds[%.1f %.1f %.1f %.1f] Frame[%.1f %.1f %.1f %.1f]",
        GetBoundsPositionX(), GetBoundsPositionY(), GetBoundsWidth(), GetBoundsHeight(),
        GetFramePositionX(), GetFramePositionY(), GetFrameWidth(), GetFrameHeight()) != -1) {
        dumpInfo.append(buffer);
    }

    errno_t ret;
    if (clipToBounds_) {
        // clipToBounds
        ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
        if (ret != EOK) {
            return "Failed to memset_s for clipToBounds, ret=" + std::to_string(ret);
        }
        if (sprintf_s(buffer, UINT8_MAX, ", ClipToBounds[True]") != -1) {
            dumpInfo.append(buffer);
        }
    }
    if (clipToFrame_) {
        // clipToFrame
        ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
        if (ret != EOK) {
            return "Failed to memset_s for clipToFrame, ret=" + std::to_string(ret);
        }
        if (sprintf_s(buffer, UINT8_MAX, ", ClipToFrame[True]") != -1) {
            dumpInfo.append(buffer);
        }
    }

    // PositionZ
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for PositionZ, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetPositionZ(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", PositionZ[%.1f]", GetPositionZ()) != -1) {
        dumpInfo.append(buffer);
    }

    // Pivot
    std::unique_ptr<RSTransform> defaultTrans = std::make_unique<RSTransform>();
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Pivot, ret=" + std::to_string(ret);
    }
    Vector2f pivot = GetPivot();
    if ((!ROSEN_EQ(pivot[0], defaultTrans->pivotX_) || !ROSEN_EQ(pivot[1], defaultTrans->pivotY_)) &&
        sprintf_s(buffer, UINT8_MAX, ", Pivot[%.1f,%.1f]", pivot[0], pivot[1]) != -1) {
        dumpInfo.append(buffer);
    }

    // CornerRadius
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for CornerRadius, ret=" + std::to_string(ret);
    }
    if (!GetCornerRadius().IsZero() &&
        sprintf_s(buffer, UINT8_MAX, ", CornerRadius[%.1f %.1f %.1f %.1f]",
            GetCornerRadius().x_, GetCornerRadius().y_, GetCornerRadius().z_, GetCornerRadius().w_) != -1) {
        dumpInfo.append(buffer);
    }

    // PixelStretch PixelStretchPercent
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for PixelStretch, ret=" + std::to_string(ret);
    }
    const auto& pixelStretch = GetPixelStretch();
    if (pixelStretch.has_value() &&
        sprintf_s(buffer, UINT8_MAX, ", PixelStretch[left:%.1f top:%.1f right:%.1f bottom:%.1f]",
            pixelStretch->x_, pixelStretch->y_, pixelStretch->z_, pixelStretch->w_) != -1) {
        dumpInfo.append(buffer);
    }

    // Rotation
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Rotation, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetRotation(), defaultTrans->rotation_) &&
        sprintf_s(buffer, UINT8_MAX, ", Rotation[%.1f]", GetRotation()) != -1) {
        dumpInfo.append(buffer);
    }
    // RotationX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for RotationX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetRotationX(), defaultTrans->rotationX_) &&
        sprintf_s(buffer, UINT8_MAX, ", RotationX[%.1f]", GetRotationX()) != -1) {
        dumpInfo.append(buffer);
    }
    // RotationY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for RotationY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetRotationY(), defaultTrans->rotationY_) &&
        sprintf_s(buffer, UINT8_MAX, ", RotationY[%.1f]", GetRotationY()) != -1) {
        dumpInfo.append(buffer);
    }

    // TranslateX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for TranslateX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetTranslateX(), defaultTrans->translateX_) &&
        sprintf_s(buffer, UINT8_MAX, ", TranslateX[%.1f]", GetTranslateX()) != -1) {
        dumpInfo.append(buffer);
    }

    // TranslateY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for TranslateY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetTranslateY(), defaultTrans->translateY_) &&
        sprintf_s(buffer, UINT8_MAX, ", TranslateY[%.1f]", GetTranslateY()) != -1) {
        dumpInfo.append(buffer);
    }

    // TranslateZ
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for TranslateZ, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetTranslateZ(), defaultTrans->translateZ_) &&
        sprintf_s(buffer, UINT8_MAX, ", TranslateZ[%.1f]", GetTranslateZ()) != -1) {
        dumpInfo.append(buffer);
    }

    // ScaleX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ScaleX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetScaleX(), defaultTrans->scaleX_) &&
        sprintf_s(buffer, UINT8_MAX, ", ScaleX[%.1f]", GetScaleX()) != -1) {
        dumpInfo.append(buffer);
    }

    // ScaleY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ScaleY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetScaleY(), defaultTrans->scaleY_) &&
        sprintf_s(buffer, UINT8_MAX, ", ScaleY[%.1f]", GetScaleY()) != -1) {
        dumpInfo.append(buffer);
    }

    // Alpha
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Alpha, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetAlpha(), 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Alpha[%.3f]", GetAlpha()) != -1) {
        dumpInfo.append(buffer);
    }

    //NeedFilter
    if (NeedFilter()) {
        dumpInfo.append(", NeedFilter[true]");
    }

    // Spherize
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Spherize, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetSpherize(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Spherize[%.1f]", GetSpherize()) != -1) {
        dumpInfo.append(buffer);
    }

    // AttractFraction
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for AttractFraction, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetAttractionFraction(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", MiniFraction[%.1f]",  GetAttractionFraction()) != -1) {
        dumpInfo.append(buffer);
    }

    // Attraction Destination Position
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for MiniDstpoint, ret=" + std::to_string(ret);
    }
    Vector2f attractionDstpoint = GetAttractionDstPoint();
    if ((!ROSEN_EQ(attractionDstpoint[0], 0.f) || !ROSEN_EQ(attractionDstpoint[1], 0.f)) &&
        sprintf_s(buffer, UINT8_MAX, ", AttractionFraction DstPointY[%.1f,%.1f]",
        attractionDstpoint[0], attractionDstpoint[1]) != -1) {
        dumpInfo.append(buffer);
    }

    // blendmode
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for blendmode, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetColorBlendMode(), 0) &&
        sprintf_s(buffer, UINT8_MAX, ", skblendmode[%d], blendType[%d]",
        GetColorBlendMode() - 1, GetColorBlendApplyType()) != -1) {
        dumpInfo.append(buffer);
    }

    // LightUpEffect
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for LightUpEffect, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetLightUpEffect(), 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", LightUpEffect[%.1f]", GetLightUpEffect()) != -1) {
        dumpInfo.append(buffer);
    }

    // ForegroundColor
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ForegroundColor, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetForegroundColor(), RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", ForegroundColor[#%08X]", GetForegroundColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    // BackgroundColor
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BackgroundColor, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetBackgroundColor(), RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", BackgroundColor[#%08X]", GetBackgroundColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    if (sprintf_s(buffer, UINT8_MAX, " placeholder:%d",
        static_cast<int>(GetBackgroundColor().GetPlaceholder())) != -1) {
        dumpInfo.append(buffer);
    }

    // BgImage
    std::unique_ptr<Decoration> defaultDecoration = std::make_unique<Decoration>();
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BgImage, ret=" + std::to_string(ret);
    }
    if ((!ROSEN_EQ(GetBgImagePositionX(), defaultDecoration->bgImageRect_.left_) ||
        !ROSEN_EQ(GetBgImagePositionY(), defaultDecoration->bgImageRect_.top_) ||
        !ROSEN_EQ(GetBgImageWidth(), defaultDecoration->bgImageRect_.width_) ||
        !ROSEN_EQ(GetBgImageHeight(), defaultDecoration->bgImageRect_.height_)) &&
        sprintf_s(buffer, UINT8_MAX, ", BgImage[%.1f %.1f %.1f %.1f]", GetBgImagePositionX(),
            GetBgImagePositionY(), GetBgImageWidth(), GetBgImageHeight()) != -1) {
        dumpInfo.append(buffer);
    }

    // Border
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Border, ret=" + std::to_string(ret);
    }
    if (border_ && border_->HasBorder() &&
        sprintf_s(buffer, UINT8_MAX, ", Border[%s]", border_->ToString().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // Filter
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Filter, ret=" + std::to_string(ret);
    }
    auto filter_ = GetFilter();
    if (filter_ && filter_->IsValid() &&
        sprintf_s(buffer, UINT8_MAX, ", Filter[%s]", filter_->GetDescription().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // BackgroundFilter
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BackgroundFilter, ret=" + std::to_string(ret);
    }
    auto backgroundFilter = GetBackgroundFilter();
    if (backgroundFilter && backgroundFilter->IsValid() &&
        sprintf_s(buffer, UINT8_MAX, ", BackgroundFilter[%s]", backgroundFilter->GetDescription().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // ForegroundFilter
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ForegroundFilter, ret=" + std::to_string(ret);
    }
    auto foregroundFilterCache_ = GetForegroundFilterCache();
    if (foregroundFilterCache_ && foregroundFilterCache_->IsValid() &&
        sprintf_s(buffer, UINT8_MAX, ", ForegroundFilter[%s]", foregroundFilterCache_->GetDescription().c_str()) !=
        -1) {
        dumpInfo.append(buffer);
    }

    // Outline
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Outline, ret=" + std::to_string(ret);
    }
    if (outline_ && outline_->HasBorder() &&
        sprintf_s(buffer, UINT8_MAX, ", Outline[%s]", outline_->ToString().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowColor
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowColor, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowColor(), Color(DEFAULT_SPOT_COLOR)) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowColor[#%08X]", GetShadowColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowOffsetX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowOffsetX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowOffsetX(), DEFAULT_SHADOW_OFFSET_X) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowOffsetX[%.1f]", GetShadowOffsetX()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowOffsetY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowOffsetY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowOffsetY(), DEFAULT_SHADOW_OFFSET_Y) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowOffsetY[%.1f]", GetShadowOffsetY()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowElevation
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowElevation, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowElevation(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowElevation[%.1f]", GetShadowElevation()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowRadius
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowRadius, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowRadius(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowRadius[%.1f]", GetShadowRadius()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowIsFilled
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowIsFilled, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowIsFilled(), false) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowIsFilled[%d]", GetShadowIsFilled()) != -1) {
        dumpInfo.append(buffer);
    }

    // FrameGravity
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for FrameGravity, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetFrameGravity(), Gravity::DEFAULT) &&
        sprintf_s(buffer, UINT8_MAX, ", FrameGravity[%d]", GetFrameGravity()) != -1) {
        dumpInfo.append(buffer);
    }

    // IsVisible
    if (!GetVisible()) {
        dumpInfo.append(", IsVisible[false]");
    }

    // UseEffect
    if (GetUseEffect()) {
        dumpInfo.append(", GetUseEffect[true]");
    }
    if (GetAlwaysSnapshot()) {
        dumpInfo.append(", AlwaysSnapshot[true]");
    }

    // HasHarmonium
    if (HasHarmonium()) {
        dumpInfo.append(", HasHarmonium[true]");
    }

    // Gray Scale
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for GrayScale, ret=" + std::to_string(ret);
    }
    auto grayScale = GetGrayScale();
    if (grayScale.has_value() && !ROSEN_EQ(*grayScale, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", GrayScale[%.1f]", *grayScale) != -1) {
        dumpInfo.append(buffer);
    }

    // DynamicLightUpRate
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for DynamicLightUpRate, ret=" + std::to_string(ret);
    }
    auto dynamicLightUpRate = GetDynamicLightUpRate();
    if (dynamicLightUpRate.has_value() && !ROSEN_EQ(*dynamicLightUpRate, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", DynamicLightUpRate[%.1f]", *dynamicLightUpRate) != -1) {
        dumpInfo.append(buffer);
    }

    // DynamicLightUpDegree
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for DynamicLightUpDegree, ret=" + std::to_string(ret);
    }
    auto dynamicLightUpDegree = GetDynamicLightUpDegree();
    if (dynamicLightUpDegree.has_value() && !ROSEN_EQ(*dynamicLightUpDegree, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", DynamicLightUpDegree[%.1f]", *dynamicLightUpDegree) != -1) {
        dumpInfo.append(buffer);
    }

    // Brightness
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Brightness, ret=" + std::to_string(ret);
    }
    auto brightness = GetBrightness();
    if (brightness.has_value() && !ROSEN_EQ(*brightness, 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Brightness[%.1f]", *brightness) != -1) {
        dumpInfo.append(buffer);
    }

    // Contrast
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Contrast, ret=" + std::to_string(ret);
    }
    auto contrast = GetContrast();
    if (contrast.has_value() && !ROSEN_EQ(*contrast, 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Contrast[%.1f]", *contrast) != -1) {
        dumpInfo.append(buffer);
    }

    // Saturate
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Saturate, ret=" + std::to_string(ret);
    }
    auto saturate = GetSaturate();
    if (saturate.has_value() && !ROSEN_EQ(*saturate, 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Saturate[%.1f]", *saturate) != -1) {
        dumpInfo.append(buffer);
    }

    // Sepia
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Sepia, ret=" + std::to_string(ret);
    }
    auto sepia = GetSepia();
    if (sepia.has_value() && !ROSEN_EQ(*sepia, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Sepia[%.1f]", *sepia) != -1) {
        dumpInfo.append(buffer);
    }

    // Invert
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Invert, ret=" + std::to_string(ret);
    }
    auto invert = GetInvert();
    if (invert.has_value() && !ROSEN_EQ(*invert, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Invert[%.1f]", *invert) != -1) {
        dumpInfo.append(buffer);
    }

    // Hue Rotate
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for HueRotate, ret=" + std::to_string(ret);
    }
    auto hueRotate = GetHueRotate();
    if (hueRotate.has_value() && !ROSEN_EQ(*hueRotate, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", HueRotate[%.1f]", *hueRotate) != -1) {
        dumpInfo.append(buffer);
    }

    // Color Blend
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ColorBlend, ret=" + std::to_string(ret);
    }
    auto colorBlend = GetColorBlend();
    if (colorBlend.has_value() && !ROSEN_EQ(*colorBlend, RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", ColorBlend[#%08X]", colorBlend->AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    // UseUnion
    if (GetUseUnion()) {
        dumpInfo.append(", UseUnion[true]");
    }
    // UnionSpacing
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for UnionSpacing, ret=" + std::to_string(ret);
    }
    float spacing = GetUnionSpacing();
    if (!ROSEN_EQ(spacing, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", UnionSpacing[%.2f]", spacing) != -1) {
        dumpInfo.append(buffer);
    }
    // SDFShape
    auto sdfShape = GetSDFShape();
    if (sdfShape) {
        dumpInfo.append(", SDFShape[" + sdfShape->Dump() + "]");
    }
    return dumpInfo;
}

// planning: need to delete, cachemanager moved to filter drawable
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
void RSProperties::CreateFilterCacheManagerIfNeed()
{
    if (!filterCacheEnabled_) {
        return;
    }
    if (auto& filter = GetBackgroundFilter()) {
        auto& cacheManager = backgroundFilterCacheManager_;
        if (cacheManager == nullptr) {
            cacheManager = std::make_unique<RSFilterCacheManager>();
        }
        cacheManager->UpdateCacheStateWithFilterHash(filter);
    } else {
        backgroundFilterCacheManager_.reset();
    }
    if (auto& filter = GetFilter()) {
        auto& cacheManager = foregroundFilterCacheManager_;
        if (cacheManager == nullptr) {
            cacheManager = std::make_unique<RSFilterCacheManager>();
        }
        cacheManager->UpdateCacheStateWithFilterHash(filter);
    } else {
        foregroundFilterCacheManager_.reset();
    }
}

const std::unique_ptr<RSFilterCacheManager>& RSProperties::GetFilterCacheManager(bool isForeground) const
{
    return isForeground ? foregroundFilterCacheManager_ : backgroundFilterCacheManager_;
}

void RSProperties::ClearFilterCache()
{
    if (foregroundFilterCacheManager_ != nullptr) {
        foregroundFilterCacheManager_->ReleaseCacheOffTree();
    }
    if (backgroundFilterCacheManager_ != nullptr) {
        backgroundFilterCacheManager_->ReleaseCacheOffTree();
    }
    const auto& backgroundFilter_ = GetBackgroundFilter();
    if (backgroundFilter_ != nullptr) {
        auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(backgroundFilter_);
        auto rsShaderFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::MASK_COLOR);
        if (rsShaderFilter != nullptr) {
            auto maskColorShaderFilter = std::static_pointer_cast<RSMaskColorShaderFilter>(rsShaderFilter);
        }
    }
    const auto& filter_ = GetFilter();
    if (filter_ != nullptr) {
        auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(filter_);
        auto rsShaderFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::MASK_COLOR);
        if (rsShaderFilter != nullptr) {
            auto maskColorShaderFilter = std::static_pointer_cast<RSMaskColorShaderFilter>(rsShaderFilter);
        }
    }
}
#endif

void RSProperties::OnApplyModifiers()
{
    if (geoDirty_) {
        if (!hasBounds_) {
            CheckEmptyBounds();
        } else {
            CalculateFrameOffset();
        }
        // frame and bounds are the same, no need to clip twice
        if (clipToFrame_ && clipToBounds_ && frameOffsetX_ == 0 && frameOffsetY_ == 0) {
            clipToFrame_ = false;
        }
        // planning: temporary fix to calculate relative matrix in OnApplyModifiers, later RSRenderNode::Update will
        // overwrite it.
        boundsGeo_->UpdateByMatrixFromSelf();

        if (GetPixelStretch().has_value() || GetPixelStretchPercent().has_value()) {
            pixelStretchNeedUpdate_ = true;
        }
    }
    if (colorFilterNeedUpdate_) {
        GenerateColorFilter();
        if (GetColorFilter() != nullptr) {
            needFilter_ = true;
            needHwcFilter_ = true;
        } else {
            // colorFilter generation failed, need to update needFilter
            filterNeedUpdate_ = true;
        }
    }
    if (pixelStretchNeedUpdate_) {
        CalculatePixelStretch();
        filterNeedUpdate_ = true;
    }

    if (bgShaderNeedUpdate_) {
        UpdateBackgroundShader();
    }

    if (IsGreyCoefNeedUpdate()) {
        CheckGreyCoef();
        GetEffect().greyCoefNeedUpdate_ = false;
        filterNeedUpdate_ = true;
    }
    GenerateRRect();
    if (filterNeedUpdate_) {
        UpdateFilter();
    }
}

void RSProperties::UpdateFilter()
{
    filterNeedUpdate_ = false;
    GenerateMaterialFilter();
    GenerateBackgroundFilter();
    GenerateForegroundFilter();
    if (GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        filterNeedUpdate_ = true;
    }
    if (GetBackgroundFilter() != nullptr && !GetBackgroundFilter()->IsValid()) {
        backgroundFilter_.reset();
    }
    if (GetFilter() != nullptr && !GetFilter()->IsValid()) {
        filter_.reset();
    }

    if (FOREGROUND_FILTER_ENABLED) {
        UpdateForegroundFilter();
    }

    needFilter_ = GetBackgroundFilter() != nullptr || GetFilter() != nullptr || GetUseEffect() || HasHarmonium() ||
                  IsLightUpEffectValid() || IsDynamicLightUpValid() || GetGreyCoef().has_value() ||
                  GetLinearGradientBlurPara() != nullptr || IsDynamicDimValid() ||
                  GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE ||
                  GetForegroundFilter() != nullptr || IsFgBrightnessValid() || IsBgBrightnessValid() ||
                  GetForegroundFilterCache() != nullptr || IsWaterRippleValid() || GetNeedDrawBehindWindow() ||
                  GetMask() || GetColorFilter() != nullptr || localMagnificationCap_ || GetPixelStretch().has_value() ||
                  GetMaterialFilter() != nullptr;

    needHwcFilter_ = GetBackgroundFilter() != nullptr || GetFilter() != nullptr || IsLightUpEffectValid() ||
                     IsDynamicLightUpValid() || GetLinearGradientBlurPara() != nullptr ||
                     IsDynamicDimValid() || IsFgBrightnessValid() || IsBgBrightnessValid() || IsWaterRippleValid() ||
                     GetNeedDrawBehindWindow() || GetColorFilter() != nullptr || localMagnificationCap_ ||
                     GetPixelStretch().has_value() || GetMaterialFilter() != nullptr;
#ifdef SUBTREE_PARALLEL_ENABLE
    // needForceSubmit_ is used to determine whether the subtree needs to read/scale pixels
    needForceSubmit_ = IsFilterNeedForceSubmit(GetFilter()) ||
                       IsFilterNeedForceSubmit(GetBackgroundFilter()) ||
                       IsFilterNeedForceSubmit(GetForegroundFilter()) ||
                       IsFilterNeedForceSubmit(GetForegroundFilterCache()) ||
                       IsFilterNeedForceSubmit(GetMaterialFilter()) ||
                       GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
#endif
}

bool RSProperties::DisableHWCForFilter() const
{
    // The difference compared to needFilter_ is no need to disable hwc when foregroundFilter is HDR_UI_BRIGHTNESS
    return GetBackgroundFilter() != nullptr || GetFilter() != nullptr || GetUseEffect() || IsLightUpEffectValid() ||
        IsDynamicLightUpValid() || GetGreyCoef().has_value() || GetLinearGradientBlurPara() != nullptr ||
        IsDynamicDimValid() || GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE ||
        (GetForegroundFilter() != nullptr && GetForegroundFilter()->GetFilterType() != RSFilter::HDR_UI_BRIGHTNESS) ||
        IsFgBrightnessValid() || IsBgBrightnessValid() ||
        (GetForegroundFilterCache() != nullptr &&
        GetForegroundFilterCache()->GetFilterType() != RSFilter::HDR_UI_BRIGHTNESS) ||
        IsWaterRippleValid() || GetNeedDrawBehindWindow() || GetMask() || GetColorFilter() != nullptr ||
        localMagnificationCap_ || GetPixelStretch().has_value() || HasHarmonium() || GetMaterialFilter() != nullptr;
}

void RSProperties::UpdateForegroundFilter()
{
    foregroundFilter_.reset();
    foregroundFilterCache_.reset();
    if (!hasReportedServerXXFilterCascade_[static_cast<size_t>(ServerFilterFunctionType::FOREGROUND_FILTER)]) {
        StatForegroundFilter();
    }
    auto motionBlurPara = GetMotionBlurPara();
    if (motionBlurPara && ROSEN_GNE(motionBlurPara->radius, 0.0)) {
        auto motionBlurFilter = std::make_shared<RSMotionBlurFilter>(motionBlurPara);
        if (IS_UNI_RENDER) {
            foregroundFilterCache_ = motionBlurFilter;
        } else {
            foregroundFilter_ = motionBlurFilter;
        }
    } else if (IsForegroundEffectRadiusValid()) {
        auto foregroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(GetForegroundEffectRadius());
        foregroundEffectFilter->SetColorPreprocess(GetColorAdaptive());
        if (IS_UNI_RENDER) {
            foregroundFilterCache_ = foregroundEffectFilter;
        } else {
            foregroundFilter_ = foregroundEffectFilter;
        }
    } else if (IsSpherizeValid()) {
        CreateSphereEffectFilter();
    } else if (IsFlyOutValid()) {
        CreateFlyOutShaderFilter();
    } else if (IsAttractionValid()) {
        CreateAttractionEffectFilter();
    } else if (IsShadowMaskValid()) {
        float elevation = GetShadowElevation();
        Drawing::scalar n1 = 0.25f * elevation * (1 + elevation / 128.0f); // 0.25f 128.0f
        Drawing::scalar blurRadius = elevation > 0.0f ? n1 : GetShadowRadius();
        auto colorfulShadowFilter =
            std::make_shared<RSColorfulShadowFilter>(blurRadius, GetShadowOffsetX(), GetShadowOffsetY());
        if (GetShadowMask() == SHADOW_MASK_STRATEGY::MASK_COLOR_BLUR) {
            colorfulShadowFilter->SetShadowColorMask(GetShadowColor());
        }
        if (IS_UNI_RENDER) {
            foregroundFilterCache_ = colorfulShadowFilter;
        } else {
            foregroundFilter_ = colorfulShadowFilter;
        }
    } else if (IsDistortionKValid()) {
        foregroundFilter_ = std::make_shared<RSDistortionFilter>(*GetDistortionK());
    } else if (IsHDRUIBrightnessValid()) {
        CreateHDRUIBrightnessFilter();
    } else if (GetForegroundNGFilter()) {
        ComposeNGRenderFilter(foregroundFilter_, GetForegroundNGFilter());
    } else if (GetColorAdaptive()) {
        foregroundFilterCache_ = std::make_shared<RSColorAdaptiveFilter>();
    }
}

void RSProperties::UpdateBackgroundShader()
{
    bgShaderNeedUpdate_ = false;
    const auto& bgShader = GetBackgroundShader();
    if (bgShader) {
        const auto &param = GetComplexShaderParam();
        if (param.has_value()) {
            const auto &paramValue = param.value();
            bgShader->MakeDrawingShader(GetBoundsRect(), paramValue);
        }
        bgShader->MakeDrawingShader(GetBoundsRect(), GetBackgroundShaderProgress());
    }
    const auto& bgNGRenderShader_ = GetBackgroundNGShader();
    if (bgNGRenderShader_ && bgNGRenderShader_->ContainsType(RSNGEffectType::BORDER_LIGHT)) {
        Vector3f rotationAngle(boundsGeo_->GetRotationX(), boundsGeo_->GetRotationY(), boundsGeo_->GetRotation());
        float cornerRadius = GetCornerRadius().x_;
        RSNGRenderShaderHelper::SetRotationAngle(bgNGRenderShader_, rotationAngle);
        RSNGRenderShaderHelper::SetCornerRadius(bgNGRenderShader_, cornerRadius);
    }
    if (bgNGRenderShader_ && bgNGRenderShader_->ContainsType(RSNGEffectType::HARMONIUM_EFFECT)) {
        float cornerRadius = GetCornerRadius().x_;
        RSNGRenderShaderHelper::SetCornerRadius(bgNGRenderShader_, cornerRadius);
    }
}

void RSProperties::CalculatePixelStretch()
{
    pixelStretchNeedUpdate_ = false;
    // no pixel stretch
    if (!GetPixelStretch().has_value() && !GetPixelStretchPercent().has_value()) {
        return;
    }
    // convert pixel stretch percent to pixel stretch
    if (GetPixelStretchPercent()) {
        auto width = GetBoundsWidth();
        auto height = GetBoundsHeight();
        if (isinf(width) || isinf(height)) {
            return;
        }
        GetEffect().pixelStretch_ = *GetPixelStretchPercent() * Vector4f(width, height, width, height);
    }
    constexpr static float EPS = 1e-5f;
    const auto& pixelStretch_ = GetPixelStretch();
    // parameter check: near zero
    if (abs(pixelStretch_->x_) < EPS && abs(pixelStretch_->y_) < EPS && abs(pixelStretch_->z_) < EPS &&
        abs(pixelStretch_->w_) < EPS) {
        WITH_EFFECT(pixelStretch_ = std::nullopt);
        return;
    }
    // parameter check: all >= 0 or all <= 0
    if ((pixelStretch_->x_ < EPS && pixelStretch_->y_ < EPS && pixelStretch_->z_ < EPS &&
        pixelStretch_->w_ < EPS) ||
        (pixelStretch_->x_ > -EPS && pixelStretch_->y_ > -EPS && pixelStretch_->z_ > -EPS &&
            pixelStretch_->w_ > -EPS)) {
        isDrawn_ = true;
        return;
    }
    WITH_EFFECT(pixelStretch_ = std::nullopt);
}

bool RSProperties::NeedBlurFuzed()
{
    if (RSSystemProperties::GetMESABlurFuzedEnabled() && GetGreyCoef().has_value()) {
        return true;
    }
    return false;
}

void RSProperties::CalculateFrameOffset()
{
    frameOffsetX_ = frameGeo_.GetX() - boundsGeo_->GetX();
    frameOffsetY_ = frameGeo_.GetY() - boundsGeo_->GetY();
    if (isinf(frameOffsetX_)) {
        frameOffsetX_ = 0.f;
    }
    if (isinf(frameOffsetY_)) {
        frameOffsetY_ = 0.f;
    }
    if (frameOffsetX_ != 0.f || frameOffsetY_ != 0.f) {
        isDrawn_ = true;
    }
}

void RSProperties::CheckGreyCoef()
{
    const auto& greyCoef_ = GetGreyCoef();
    if (!greyCoef_.has_value()) {
        return;
    }
    // 127.0 half of 255.0
    if (ROSEN_LNE(greyCoef_->x_, 0.f) || ROSEN_GNE(greyCoef_->x_, 127.f) ||
        ROSEN_LNE(greyCoef_->y_, 0.f) || ROSEN_GNE(greyCoef_->y_, 127.f) ||
        (ROSEN_EQ(greyCoef_->x_, 0.f) && ROSEN_EQ(greyCoef_->y_, 0.f))) {
        WITH_EFFECT(greyCoef_ = std::nullopt);
    }
}

// blend with background
void RSProperties::SetColorBlendMode(int colorBlendMode)
{
    GetEffect().colorBlendMode_ = std::clamp<int>(colorBlendMode, 0, static_cast<int>(RSColorBlendMode::MAX));
    if (GetColorBlendMode() != static_cast<int>(RSColorBlendMode::NONE)) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetColorBlendApplyType(int colorBlendApplyType)
{
    GetEffect().colorBlendApplyType_ =
        std::clamp<int>(colorBlendApplyType, 0, static_cast<int>(RSColorBlendApplyType::MAX));
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::GetHaveEffectRegion() const
{
    if (effect_) {
        return effect_->haveEffectRegion_;
    }
    return false;
}

void RSProperties::SetHaveEffectRegion(bool haveEffectRegion)
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    // clear cache if new region is null or outside current region
    if (auto& manager = GetFilterCacheManager(false);
        manager && manager->IsCacheValid() && haveEffectRegion == false) {
        manager->UpdateCacheStateWithFilterRegion();
    }
#endif
    GetEffect().haveEffectRegion_ = haveEffectRegion;
}

void RSProperties::ResetBorder(bool isOutline)
{
    if (isOutline) {
        outline_ = nullptr;
    } else {
        border_ = nullptr;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBackgroundNGShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader)
{
    GetEffect().bgNGRenderShader_ = renderShader;
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
    hasHarmonium_ = false;
    const auto& bgNGRenderShader_ = GetBackgroundNGShader();
    if (bgNGRenderShader_ && bgNGRenderShader_->ContainsType(RSNGEffectType::HARMONIUM_EFFECT)) {
        hasHarmonium_ = true;
    }
}

std::shared_ptr<RSNGRenderShaderBase> RSProperties::GetBackgroundNGShader() const
{
    if (effect_) {
        return effect_->bgNGRenderShader_;
    }
    return nullptr;
}

void RSProperties::SetForegroundShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader)
{
    GetEffect().fgRenderShader_ = renderShader;
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSNGRenderShaderBase> RSProperties::GetForegroundShader() const
{
    if (effect_) {
        return effect_->fgRenderShader_;
    }
    return nullptr;
}

void RSProperties::InternalSetSDFShape(const std::shared_ptr<RSNGRenderShapeBase>& shape)
{
    if (ROSEN_EQ(renderSDFShape_, shape)) {
        return;
    }
    renderSDFShape_ = shape;
}

void RSProperties::SetSDFShape(const std::shared_ptr<RSNGRenderShapeBase>& shape)
{
    if (ROSEN_EQ(renderSDFShape_, shape)) {
        return;
    }
    renderSDFShape_ = shape;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSNGRenderShapeBase> RSProperties::GetSDFShape() const
{
    return renderSDFShape_;
}

void RSProperties::SetMaterialNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter)
{
    GetEffect().mtNGRenderFilter_ = renderFilter;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSNGRenderFilterBase> RSProperties::GetMaterialNGFilter() const
{
    if (effect_) {
        return effect_->mtNGRenderFilter_;
    }
    return nullptr;
}

RRect RSProperties::GetRRectForSDF() const
{
    RRect sdfRRect;
    if (GetClipToRRect()) {
        auto rrect = GetClipRRect();
        sdfRRect = RRect(rrect.rect_, rrect.radius_[0].x_, rrect.radius_[0].y_);
    } else if (!GetCornerRadius().IsZero()) {
        auto rrect = GetRRect();
        sdfRRect = RRect(rrect.rect_, rrect.radius_[0].x_, rrect.radius_[0].y_);
    } else {
        sdfRRect.rect_ = GetBoundsRect();
    }
    return sdfRRect;
}

bool RSProperties::GetColorAdaptive() const
{
    if (effect_) {
        return effect_->colorAdaptive_;
    }
    return false;
}

void RSProperties::SetAdaptive(bool value)
{
    GetEffect().colorAdaptive_ = value;
    isDrawn_ = true;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}
} // namespace Rosen
} // namespace OHOS
