/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_property_drawable_foreground.h"

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_property_drawable_utils.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
namespace {
constexpr int PARAM_TWO = 2;
} // namespace

const bool FOREGROUND_FILTER_ENABLED = RSSystemProperties::GetForegroundFilterEnabled();

// ====================================
// Binarization
RSDrawable::Ptr RSBinarizationDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBinarizationDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSBinarizationDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& aiInvert = node.GetRenderProperties().GetAiInvert();
    if (!aiInvert.has_value()) {
        return false;
    }
    needSync_ = true;
    stagingAiInvert_ = aiInvert;
    return true;
}

void RSBinarizationDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    aiInvert_ = std::move(stagingAiInvert_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSBinarizationDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSBinarizationDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSBINARIZATIONDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawBinarization(canvas, ptr->aiInvert_);
    };
}

RSDrawable::Ptr RSColorFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSColorFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSColorFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& colorFilter = node.GetRenderProperties().GetColorFilter();
    if (colorFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingFilter_ = colorFilter;
    return true;
}

void RSColorFilterDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    filter_ = std::move(stagingFilter_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSColorFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSColorFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSCOLORFILTERDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawColorFilter(canvas, ptr->filter_);
    };
}
RSDrawable::Ptr RSLightUpEffectDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSLightUpEffectDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSLightUpEffectDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().IsLightUpEffectValid()) {
        return false;
    }
    needSync_ = true;
    stagingLightUpEffectDegree_ = node.GetRenderProperties().GetLightUpEffect();
    return true;
}

void RSLightUpEffectDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    lightUpEffectDegree_ = stagingLightUpEffectDegree_;
    stagingLightUpEffectDegree_ = 1.0f;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSLightUpEffectDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSLightUpEffectDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSLIGHTUPEFFECTDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawLightUpEffect(canvas, ptr->lightUpEffectDegree_);
    };
}

RSDrawable::Ptr RSDynamicDimDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSDynamicDimDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSDynamicDimDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().IsDynamicDimValid()) {
        return false;
    }
    needSync_ = true;
    stagingDynamicDimDegree_ = node.GetRenderProperties().GetDynamicDimDegree().value();
    return true;
}

void RSDynamicDimDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    dynamicDimDegree_ = stagingDynamicDimDegree_;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSDynamicDimDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSDynamicDimDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSDYNAMICDIMDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawDynamicDim(canvas, ptr->dynamicDimDegree_);
    };
}

RSDrawable::Ptr RSForegroundColorDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundColorDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSForegroundColorDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto fgColor = properties.GetForegroundColor();
    if (fgColor == RgbPalette::Transparent()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(fgColor.AsArgbInt()));
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSCompositingFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSCompositingFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSCompositingFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    stagingNodeName_ = node.GetNodeName();
    auto& rsFilter = node.GetRenderProperties().GetFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    RecordFilterInfos(rsFilter);
    needSync_ = true;
    stagingFilter_ = rsFilter;
    return true;
}

// foregroundFilter
RSDrawable::Ptr RSForegroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (!FOREGROUND_FILTER_ENABLED) {
        ROSEN_LOGD("RSForegroundFilterDrawable::OnGenerate close blur.");
        return nullptr;
    }
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return nullptr;
    }

    if (auto ret = std::make_shared<RSForegroundFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSForegroundFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingBoundsRect_ = node.GetRenderProperties().GetBoundsRect();
    return true;
}

Drawing::RecordingCanvas::DrawFunc RSForegroundFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSForegroundFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSFOREGROUNDFILTERDRAWABLE);
#endif
        RSPropertyDrawableUtils::BeginForegroundFilter(*paintFilterCanvas, ptr->boundsRect_);
    };
}

void RSForegroundFilterDrawable::OnSync()
{
    if (needSync_ == false) {
        return;
    }
    boundsRect_ = stagingBoundsRect_;
    needSync_ = false;
}

// Restore RSForegroundFilter
RSDrawable::Ptr RSForegroundFilterRestoreDrawable::OnGenerate(const RSRenderNode& node)
{
    if (!FOREGROUND_FILTER_ENABLED) {
        ROSEN_LOGD("RSForegroundFilterRestoreDrawable::OnGenerate close blur.");
        return nullptr;
    }
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return nullptr;
    }

    if (auto ret = std::make_shared<RSForegroundFilterRestoreDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSForegroundFilterRestoreDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingForegroundFilter_ = rsFilter;
    return true;
}

Drawing::RecordingCanvas::DrawFunc RSForegroundFilterRestoreDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSForegroundFilterRestoreDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSFOREGROUNDFILTERRESTOREDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawForegroundFilter(*paintFilterCanvas, ptr->foregroundFilter_);
    };
}

void RSForegroundFilterRestoreDrawable::OnSync()
{
    if (needSync_ == false) {
        return;
    }
    foregroundFilter_ = std::move(stagingForegroundFilter_);
    needSync_ = false;
}

RSDrawable::Ptr RSPixelStretchDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPixelStretchDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSPixelStretchDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& pixelStretch = node.GetRenderProperties().GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return false;
    }
    needSync_ = true;
    stagingPixelStretch_ = pixelStretch;
    stagePixelStretchTileMode_ = node.GetRenderProperties().GetPixelStretchTileMode();
    const auto& boundsGeo = node.GetRenderProperties().GetBoundsGeometry();
    stagingBoundsGeoValid_ = boundsGeo && !boundsGeo->IsEmpty();
    stagingBoundsRect_ = node.GetRenderProperties().GetBoundsRect();
    return true;
}

void RSPixelStretchDrawable::SetPixelStretch(const std::optional<Vector4f>& pixelStretch)
{
    stagingPixelStretch_ = pixelStretch;
}

void RSPixelStretchDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    pixelStretch_ = std::move(stagingPixelStretch_);
    pixelStretchTileMode_ = stagePixelStretchTileMode_;
    boundsGeoValid_ = stagingBoundsGeoValid_;
    stagingBoundsGeoValid_ = false;
    boundsRect_ = stagingBoundsRect_;
    stagingBoundsRect_.Clear();
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPixelStretchDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPixelStretchDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSPIXELSTRETCHDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawPixelStretch(canvas, ptr->pixelStretch_, ptr->boundsRect_, ptr->boundsGeoValid_,
            static_cast<Drawing::TileMode>(ptr->pixelStretchTileMode_));
    };
}

RSDrawable::Ptr RSBorderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBorderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSBorderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto& border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    DrawBorder(properties, *updater.GetRecordingCanvas(), border, false);
    return true;
}

void RSBorderDrawable::DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
    const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        auto roundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
            RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline));
        auto innerRoundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
            RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(properties, border, isOutline));
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(roundRect, innerRoundRect);
        canvas.DetachBrush();
        return;
    }
    bool isZero = isOutline ? border->GetRadiusFour().IsZero() : properties.GetCornerRadius().IsZero();
    if (isZero && border->ApplyFourLine(pen)) {
        RectF rectf =
            isOutline ? properties.GetBoundsRect().MakeOutset(border->GetWidthFour()) : properties.GetBoundsRect();
        border->PaintFourLine(canvas, pen, rectf);
        return;
    }
    if (border->ApplyPathStyle(pen)) {
        auto borderWidth = border->GetWidth();
        RRect rrect = RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline);
        rrect.rect_.width_ -= borderWidth;
        rrect.rect_.height_ -= borderWidth;
        rrect.rect_.Move(borderWidth / PARAM_TWO, borderWidth / PARAM_TWO);
        Drawing::Path borderPath;
        borderPath.AddRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(rrect));
        canvas.AttachPen(pen);
        canvas.DrawPath(borderPath);
        canvas.DetachPen();
        return;
    }

    RSBorderGeo borderGeo;
    borderGeo.rrect = RSPropertyDrawableUtils::RRect2DrawingRRect(
        RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline));
    borderGeo.innerRRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
        RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(properties, border, isOutline));
    auto centerX = borderGeo.innerRRect.GetRect().GetLeft() + borderGeo.innerRRect.GetRect().GetWidth() / 2;
    auto centerY = borderGeo.innerRRect.GetRect().GetTop() + borderGeo.innerRRect.GetRect().GetHeight() / 2;
    borderGeo.center = { centerX, centerY };
    auto rect = borderGeo.rrect.GetRect();
    Drawing::AutoCanvasRestore acr(canvas, false);
    Drawing::SaveLayerOps slr(&rect, nullptr);
    canvas.SaveLayer(slr);
    border->DrawBorders(canvas, pen, borderGeo);
}

RSDrawable::Ptr RSOutlineDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSOutlineDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSOutlineDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto& outline = properties.GetOutline();
    if (!outline || !outline->HasBorder()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    RSBorderDrawable::DrawBorder(properties, *updater.GetRecordingCanvas(), outline, true);
    return true;
}

RSDrawable::Ptr RSPointLightDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPointLightDrawable>(node.GetRenderProperties()); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

Drawing::RecordingCanvas::DrawFunc RSPointLightDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPointLightDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSPOINTLIGHTDRAWABLE);
#endif
        ptr->DrawLight(canvas);
    };
}

bool RSPointLightDrawable::OnUpdate(const RSRenderNode& node)
{
    const auto& illuminatedPtr = properties_.GetIlluminated();
    if (!illuminatedPtr || !illuminatedPtr->IsIlluminatedValid()) {
        return false;
    }
    return true;
}

void RSPointLightDrawable::OnSync()
{
    lightSourcesAndPosVec_.clear();
    const auto& lightSourcesAndPosMap  = properties_.GetIlluminated()->GetLightSourcesAndPosMap();
    for (auto &pair : lightSourcesAndPosMap) {
        lightSourcesAndPosVec_.push_back(pair);
    }
    properties_.GetIlluminated()->ClearLightSourcesAndPosMap();
    if (lightSourcesAndPosVec_.empty()) {
        return;
    }
    if (lightSourcesAndPosVec_.size() > MAX_LIGHT_SOURCES) {
        std::sort(lightSourcesAndPosVec_.begin(), lightSourcesAndPosVec_.end(), [](const auto& x, const auto& y) {
            return x.second.x_ * x.second.x_ + x.second.y_ * x.second.y_ <
                   y.second.x_ * y.second.x_ + y.second.y_ * y.second.y_;
        });
    }
    illuminatedType_ = properties_.GetIlluminated()->GetIlluminatedType();
    borderWidth_ = std::ceil(properties_.GetIlluminatedBorderWidth());
    auto& rrect = properties_.GetRRect();
    if (illuminatedType_ == IlluminatedType::BORDER_CONTENT ||
        illuminatedType_ == IlluminatedType::BORDER ||
        illuminatedType_ == IlluminatedType::BLEND_BORDER ||
        illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT ||
        illuminatedType_ == IlluminatedType::FEATHERING_BORDER) {
        // half width and half height requires divide by 2.0f
        Vector4f width = { borderWidth_ / 2.0f };
        auto borderRRect = rrect.Inset(width);
        borderRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(borderRRect);
    }
    if (illuminatedType_ == IlluminatedType::BORDER_CONTENT ||
        illuminatedType_ == IlluminatedType::CONTENT ||
        illuminatedType_ == IlluminatedType::BLEND_CONTENT ||
        illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT ||
        illuminatedType_ == IlluminatedType::FEATHERING_BORDER) {
        contentRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(rrect);
    }
    if (properties_.GetBoundsGeometry()) {
        rect_ = properties_.GetBoundsGeometry()->GetAbsRect();
    }
}

void RSPointLightDrawable::DrawLight(Drawing::Canvas* canvas) const
{
    if (lightSourcesAndPosVec_.empty()) {
        return;
    }
    auto phongShaderBuilder = GetPhongShaderBuilder();
    if (!phongShaderBuilder) {
        return;
    }
    if (illuminatedType_ == IlluminatedType::FEATHERING_BORDER) {
        phongShaderBuilder = GetFeatheringBoardLightShaderBuilder();
        float rectWidth = contentRRect_.GetRect().GetWidth();
        float rectHeight = contentRRect_.GetRect().GetHeight();
        phongShaderBuilder->SetUniform("iResolution", rectWidth, rectHeight);
        phongShaderBuilder->SetUniform("contentBorderRadius",
            contentRRect_.GetCornerRadius(Drawing::RoundRect::CornerPos::TOP_LEFT_POS).GetX());
    }
    constexpr int vectorLen = 4;
    float lightPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float viewPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float lightColorArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    std::array<float, MAX_LIGHT_SOURCES> lightIntensityArray = { 0 };

    auto iter = lightSourcesAndPosVec_.begin();
    auto cnt = 0;
    while (iter != lightSourcesAndPosVec_.end() && cnt < MAX_LIGHT_SOURCES) {
        auto lightPos = iter->second;
        auto lightIntensity = iter->first->GetLightIntensity();
        auto lightColor = iter->first->GetLightColor();
        Vector4f lightColorVec =
            Vector4f(lightColor.GetRed(), lightColor.GetGreen(), lightColor.GetBlue(), lightColor.GetAlpha());
        for (int i = 0; i < vectorLen; i++) {
            lightPosArray[cnt * vectorLen + i] = lightPos[i];
            viewPosArray[cnt * vectorLen + i] = lightPos[i];
            lightColorArray[cnt * vectorLen + i] = lightColorVec[i] / UINT8_MAX;
        }
        lightIntensityArray[cnt] = lightIntensity;
        iter++;
        cnt++;
    }
    phongShaderBuilder->SetUniform("lightPos", lightPosArray, vectorLen * MAX_LIGHT_SOURCES);
    phongShaderBuilder->SetUniform("viewPos", viewPosArray, vectorLen * MAX_LIGHT_SOURCES);
    phongShaderBuilder->SetUniform("specularLightColor", lightColorArray, vectorLen * MAX_LIGHT_SOURCES);
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    ROSEN_LOGD("RSPointLightDrawable::DrawLight illuminatedType:%{public}d", illuminatedType_);
    if ((illuminatedType_ == IlluminatedType::BORDER_CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        DrawContentLight(*canvas, phongShaderBuilder, brush, lightIntensityArray);
        DrawBorderLight(*canvas, phongShaderBuilder, pen, lightIntensityArray);
    } else if ((illuminatedType_ == IlluminatedType::CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_CONTENT)) {
        DrawContentLight(*canvas, phongShaderBuilder, brush, lightIntensityArray);
    } else if ((illuminatedType_ == IlluminatedType::BORDER) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (illuminatedType_ == IlluminatedType::FEATHERING_BORDER)) {
        DrawBorderLight(*canvas, phongShaderBuilder, pen, lightIntensityArray);
    }
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightDrawable::GetPhongShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> phongShaderBuilder;
    if (phongShaderBuilder) {
        return phongShaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect;
    const static std::string lightString(R"(
        uniform vec4 lightPos[12];
        uniform vec4 viewPos[12];
        uniform vec4 specularLightColor[12];
        uniform float specularStrength[12];

        mediump vec4 main(vec2 drawing_coord) {
            vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            float shininess = 8.0;
            mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 12; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor; // diffuse reflection
                    vec4 specularColor = specularLightColor[i];
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor;
                }
            }
            return fragColor;
        }
    )");
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(lightString);
    if (!effect) {
        ROSEN_LOGE("light effect error");
        return phongShaderBuilder;
    }
    lightEffect = std::move(effect);
    phongShaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(lightEffect);
    return phongShaderBuilder;
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightDrawable::GetFeatheringBoardLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> featheringBoardLightShaderBuilder;
    if (featheringBoardLightShaderBuilder) {
        return featheringBoardLightShaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect;
    const static std::string lightString(R"(
        uniform vec2 iResolution;
        uniform float contentBorderRadius;
        uniform vec4 lightPos[12];
        uniform vec4 viewPos[12];
        uniform vec4 specularLightColor[12];
        uniform float specularStrength[12];

        float sdRoundedBox(vec2 p, vec2 b, float r)
        {
            vec2 q = abs(p) - b + r;
            return (min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r);
        }

        vec2 sdRoundedBoxGradient(vec2 p, vec2 b, float r)
        {
            vec2 signs = vec2(p.x >= 0.0 ? 1.0 : -1.0, p.y >= 0.0 ? 1.0 : -1.0);
            vec2 q = abs(p) - b + r;
            vec2 nor = (q.y > q.x) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
            nor = (q.x > 0.0 && q.y > 0.0) ? normalize(q) : nor;
            return signs * nor;
        }

        mediump vec4 main(vec2 drawing_coord)
        {
            float shininess = 8.0;
            mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
            vec2 halfResolution = iResolution.xy * 0.5;
            float sd = sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, contentBorderRadius)
                / halfResolution.y;
            vec2 grad = sdRoundedBoxGradient(drawing_coord.xy - halfResolution, halfResolution, contentBorderRadius);
            for (int i = 0; i < 12; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec2 lightGrad = sdRoundedBoxGradient(lightPos[i].xy - halfResolution, halfResolution,
                        contentBorderRadius); // lightGrad could be pre-computed
                    float angleEfficient = dot(grad, lightGrad);
                    if (angleEfficient > 0.0) {
                        vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                        vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                        vec3 halfwayDir = normalize(lightDir + viewDir);                             // half vector
                        // exponential relationship of angle
                        float spec = pow(max(halfwayDir.z, 0.0), shininess); // norm is (0.0, 0.0, 1.0)
                        spec *= specularStrength[i];
                        spec *= smoothstep(-0.28, 0.0, sd);
                        spec *= (smoothstep(1.0, 0.0, spec) * 0.2 + 0.8);
                        vec4 specularColor = specularLightColor[i];
                        fragColor += specularColor * (spec * angleEfficient);
                    }
                }
            }
            return fragColor;
        }
    )");
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(lightString);
    if (!effect) {
        ROSEN_LOGE("light effect error");
        return featheringBoardLightShaderBuilder;
    }
    featheringBoardLightShaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return featheringBoardLightShaderBuilder;
}

void RSPointLightDrawable::DrawContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    constexpr float contentIntensityCoefficient = 0.3f;
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i] * contentIntensityCoefficient;
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(shader);
    if ((illuminatedType_ == IlluminatedType::BLEND_CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&contentRRect_.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(contentRRect_);
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(contentRRect_);
        canvas.DetachBrush();
    }
}

void RSPointLightDrawable::DrawBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i];
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(borderWidth_);
    pen.SetWidth(borderWidth);
    if ((illuminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        Drawing::Brush maskPaint;
        pen.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&borderRRect_.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(borderRRect_);
        canvas.DetachPen();
        canvas.Restore();
    } else {
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(borderRRect_);
        canvas.DetachPen();
    }
}

RSDrawable::Ptr RSParticleDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSParticleDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSParticleDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    const auto& particles = particleVector.GetParticleVector();
    auto bounds = properties.GetDrawRegion();
    auto imageCount = particleVector.GetParticleImageCount();
    auto imageVector = particleVector.GetParticleImageVector();
    auto particleDrawable = std::make_shared<RSParticlesDrawable>(particles, imageVector, imageCount);
    if (particleDrawable != nullptr) {
        particleDrawable->Draw(canvas, bounds);
    }
    return true;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
