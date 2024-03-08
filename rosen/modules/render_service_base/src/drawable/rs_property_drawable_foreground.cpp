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
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"

namespace OHOS::Rosen {
namespace {
constexpr int PARAM_TWO = 2;
constexpr int MAX_LIGHT_SOURCES = 4;
} // namespace
std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::phongShaderBuilder_ = nullptr;

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
        if (canvas && ptr->aiInvert_.has_value()) {
            RSPropertyDrawableUtils::DrawBinarization(canvas, ptr->aiInvert_);
        }
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
        if (canvas && ptr->filter_) {
            RSPropertyDrawableUtils::DrawColorFilter(canvas, ptr->filter_);
        }
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
        if (canvas) {
            RSPropertyDrawableUtils::DrawLightUpEffect(canvas, ptr->lightUpEffectDegree_);
        }
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

RSDrawable::Ptr RSForegroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSForegroundFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingFilter_ = rsFilter;
    return true;
}

void RSForegroundFilterDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    filter_ = std::move(stagingFilter_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSForegroundFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSForegroundFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (canvas && ptr->filter_) {
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_, true);
        }
    };
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
    const auto& boundsGeo = node.GetRenderProperties().GetBoundsGeometry();
    stagingBoundsGeoValid_ = boundsGeo && !boundsGeo->IsEmpty();
    stagingBoundsRect_ = node.GetRenderProperties().GetBoundsRect();
    return true;
}

void RSPixelStretchDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    pixelStretch_ = std::move(stagingPixelStretch_);
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
        if (canvas && ptr->pixelStretch_.has_value()) {
            RSPropertyDrawableUtils::DrawPixelStretch(
                canvas, ptr->pixelStretch_, ptr->boundsRect_, ptr->boundsGeoValid_);
        }
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
    auto& border = node.GetRenderProperties().GetBorder();
    if (!border || !border->HasBorder()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    DrawBorder(properties, *updater.GetRecordingCanvas(), properties.GetBorder(), false);
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
    bool isZero = isOutline ? properties.GetCornerRadius().IsZero() : border->GetRadiusFour().IsZero();
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

    Drawing::AutoCanvasRestore acr(canvas, true);
    auto rrect = RSPropertyDrawableUtils::RRect2DrawingRRect(
        RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline));
    canvas.ClipRoundRect(rrect, Drawing::ClipOp::INTERSECT, true);
    auto innerRoundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
        RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(properties, border, isOutline));
    canvas.ClipRoundRect(innerRoundRect, Drawing::ClipOp::DIFFERENCE, true);
    Drawing::scalar centerX = innerRoundRect.GetRect().GetLeft() + innerRoundRect.GetRect().GetWidth() / 2;
    Drawing::scalar centerY = innerRoundRect.GetRect().GetTop() + innerRoundRect.GetRect().GetHeight() / 2;
    Drawing::Point center = { centerX, centerY };
    auto rect = rrect.GetRect();
    Drawing::SaveLayerOps slr(&rect, nullptr);
    canvas.SaveLayer(slr);
    border->PaintTopPath(canvas, pen, rrect, center);
    border->PaintRightPath(canvas, pen, rrect, center);
    border->PaintBottomPath(canvas, pen, rrect, center);
    border->PaintLeftPath(canvas, pen, rrect, center);
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
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    RSBorderDrawable::DrawBorder(properties, *updater.GetRecordingCanvas(), properties.GetOutline(), true);
    return true;
}

RSDrawable::Ptr RSPointLightDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPointLightDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSPointLightDrawable::OnUpdate(const RSRenderNode& node)
{
    auto lightBuilder = GetPhongShaderBuilder();
    if (!lightBuilder) {
        ROSEN_LOGE("RSPointLightDrawable::OnUpdate lightBuilder is null.");
        return false;
    }
    const RSProperties& properties = node.GetRenderProperties();
    const auto& lightSources = properties.GetIlluminated()->GetLightSources();
    if (lightSources.empty()) {
        ROSEN_LOGE("RSPointLightDrawable::OnUpdate lightSourceList is empty.");
        return false;
    }
    const auto& geoPtr = properties.GetBoundsGeometry();
    if (!geoPtr || geoPtr->IsEmpty()) {
        ROSEN_LOGE("RSPointLightDrawable::OnUpdate geoPtr is null or empty.");
        return false;
    }

    auto iter = lightSources.begin();
    auto cnt = 0;
    Drawing::Matrix44 lightPositionMatrix;
    Drawing::Matrix44 viewPosMatrix;
    Vector4f lightIntensityV4;
    while (iter != lightSources.end() && cnt < MAX_LIGHT_SOURCES) {
        auto lightPos = RSPointLightManager::Instance()->CalculateLightPosForIlluminated((*iter), geoPtr);
        lightIntensityV4[cnt] = (*iter)->GetLightIntensity();
        lightPositionMatrix.SetCol(cnt, lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_);
        viewPosMatrix.SetCol(cnt, lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_);
        iter++;
        cnt++;
    }
    lightBuilder->SetUniform("lightPos", lightPositionMatrix);
    lightBuilder->SetUniform("viewPos", viewPosMatrix);
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    auto illuminatedType = properties.GetIlluminated()->GetIlluminatedType();
    ROSEN_LOGD("RSPointLightDrawable::OnUpdate illuminatedType:%{public}d", illuminatedType);
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    if (illuminatedType == IlluminatedType::CONTENT || illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, brush, lightIntensityV4);
    }
    if (illuminatedType == IlluminatedType::BORDER || illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawBorderLight(properties, canvas, lightBuilder, pen, lightIntensityV4);
    }
    return true;
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightDrawable::GetPhongShaderBuilder()
{
    if (phongShaderBuilder_) {
        return phongShaderBuilder_;
    }
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect;
    std::string lightString(R"(
        uniform mat4 lightPos;
        uniform mat4 viewPos;
        uniform vec4 specularStrength;

        mediump vec4 main(vec2 drawing_coord) {
            vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0);
            float shininess = 8.0;
            mediump vec4 fragColor;
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 4; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor; // diffuse reflection
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor;
                }
            }
            return fragColor;
        }
    )");
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(lightString);
    if (!effect) {
        ROSEN_LOGE("light effect error");
        return phongShaderBuilder_;
    }
    lightEffect = std::move(effect);
    phongShaderBuilder_ = std::make_shared<Drawing::RuntimeShaderBuilder>(lightEffect);
    return phongShaderBuilder_;
}

void RSPointLightDrawable::DrawContentLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Vector4f& lightIntensity)
{
    auto contentStrength = lightIntensity * 0.3f;
    lightBuilder->SetUniformVec4(
        "specularStrength", contentStrength.x_, contentStrength.y_, contentStrength.z_, contentStrength.w_);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
}

void RSPointLightDrawable::DrawBorderLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen, Vector4f& lightIntensity)
{
    lightBuilder->SetUniformVec4(
        "specularStrength", lightIntensity.x_, lightIntensity.y_, lightIntensity.z_, lightIntensity.w_);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(properties.GetIlluminatedBorderWidth());
    pen.SetWidth(borderWidth);
    auto borderRect = properties.GetRRect().rect_;
    float borderRadius = properties.GetRRect().radius_[0].x_;
    auto borderRRect = RRect(RectF(borderRect.left_ + borderWidth / 2.0f, borderRect.top_ + borderWidth / 2.0f,
                                 borderRect.width_ - borderWidth, borderRect.height_ - borderWidth),
        borderRadius - borderWidth / 2.0f, borderRadius - borderWidth / 2.0f);
    canvas.AttachPen(pen);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(borderRRect));
    canvas.DetachPen();
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
    for (const auto& particle : particles) {
        if (particle != nullptr && particle->IsAlive()) {
            // Get particle properties
            auto position = particle->GetPosition();
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            if (!(bounds->Intersect(position.x_, position.y_)) || opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            brush.SetAlphaF(opacity);
            auto clipBounds = Drawing::Rect(
                bounds->left_, bounds->top_, bounds->left_ + bounds->width_, bounds->top_ + bounds->height_);
            canvas.ClipRect(clipBounds, Drawing::ClipOp::INTERSECT, true);

            if (particleType == ParticleType::POINTS) {
                auto radius = particle->GetRadius();
                Color color = particle->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
                brush.SetColor(color.AsArgbInt());
                canvas.AttachBrush(brush);
                canvas.DrawCircle(Drawing::Point(position.x_, position.y_), radius * scale);
                canvas.DetachBrush();
            } else {
                auto imageSize = particle->GetImageSize();
                auto image = particle->GetImage();
                float left = position.x_;
                float top = position.y_;
                float right = position.x_ + imageSize.x_ * scale;
                float bottom = position.y_ + imageSize.y_ * scale;
                canvas.Save();
                canvas.Translate(position.x_, position.y_);
                canvas.Rotate(particle->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
                RectF destRect(left, top, right, bottom);
                image->SetDstRect(destRect);
                image->SetScale(scale);
                image->SetImageRepeat(0);
                Drawing::Rect rect { left, top, right, bottom };
                canvas.AttachBrush(brush);
                image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
                canvas.DetachBrush();
                canvas.Restore();
            }
        }
    }
    return true;
}
} // namespace OHOS::Rosen
