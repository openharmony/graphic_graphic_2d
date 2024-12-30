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

#include "modifier_ng/rs_overlay_render_modifier.h"

#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS::Rosen::ModifierNG {
constexpr float DEGREE_TO_RADIAN = M_PI / 180;
constexpr float DOUBLE = 2.f;
constexpr float DEFAULT_RADIUS = 100;
constexpr int MAX_ATLAS_COUNT = 2000;

const RSParticleRenderModifier::LegacyPropertyApplierMap RSParticleRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::PARTICLE_EMITTER_UPDATER,
        RSRenderModifier::PropertyApplyHelper<std::vector<std::shared_ptr<EmitterUpdater>>,
            &RSProperties::SetEmitterUpdater> },
    { RSPropertyType::PARTICLE_NOISE_FIELD, RSRenderModifier::PropertyApplyHelper<std::shared_ptr<ParticleNoiseFields>,
                                                &RSProperties::SetParticleNoiseFields> },
};

void RSParticleRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetEmitterUpdater({});
    properties.SetParticleNoiseFields({});
}

bool RSParticleRenderModifier::OnApply(RSModifierContext& context)
{
    const auto& particleVector = Getter<RSRenderParticleVector>(RSPropertyType::PARTICLE);
    if (particleVector.GetParticleSize() == 0) {
        ROSEN_LOGE("RSParticleRenderModifier::onApply particleVector has no particle stored");
        return false;
    }
    stagingParticles_ = particleVector.GetParticleVector();
    stagingImageCount_ = particleVector.GetParticleImageCount();
    stagingImageVector_ = particleVector.GetParticleImageVector();
    stagingCount_.resize(imageCount_);
    stagingImageRsxform_.resize(imageCount_);
    stagingImageTex_.resize(imageCount_);
    stagingImageColors_.resize(imageCount_);
    return true;
}

void RSParticleRenderModifier::OnSync()
{
    std::swap(particles_, stagingParticles_);
    std::swap(imageCount_, stagingImageCount_);
    std::swap(imageVector_, stagingImageVector_);
    std::swap(count_, stagingCount_);
    std::swap(imageRsxform_, stagingImageRsxform_);
    std::swap(imageTex_, stagingImageTex_);
    std::swap(imageColors_, stagingImageColors_);

    circleImage_.reset();
    pointCount_ = 0;
    pointRsxform_.clear();
    pointTex_.clear();
    pointColors_.clear();

    ClearStagingValues();
}

void RSParticleRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (particles_.empty()) {
        ROSEN_LOGE("RSParticleRenderModifier::Draw particles_ is empty");
        return;
    }
    for (const auto& particle : particles_) {
        if (particle != nullptr && particle->IsAlive()) {
            auto position = particle->GetPosition();
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            bool intersect = (position.x_ >= rect.GetLeft()) && (position.x_ < rect.GetRight()) &&
                             (position.y_ >= rect.GetTop()) && (position.y_ < rect.GetBottom());
            if (rect.IsEmpty() || !intersect || opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
            canvas.ClipRect(rect, Drawing::ClipOp::INTERSECT, true);
            if (particleType == ParticleType::POINTS) {
                CaculatePointAtlsArry(particle, position, opacity, scale);
            } else {
                CaculateImageAtlsArry(canvas, particle, position, opacity, scale);
            }
        }
    }
    DrawParticles(canvas);
}

void RSParticleRenderModifier::Purge()
{
    ClearStagingValues();
    OnSync();
}

std::shared_ptr<Drawing::Image> RSParticleRenderModifier::MakeCircleImage(int radius)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(radius * DOUBLE, radius * DOUBLE, format);
    bitmap.ClearWithColor(Drawing::Color::COLOR_TRANSPARENT);
    auto recordingCanvas = Drawing::Canvas();
    recordingCanvas.Bind(bitmap);
    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::CLEAR);
    Drawing::Rect r = { 0, 0, radius * DOUBLE, radius * DOUBLE };
    recordingCanvas.AttachBrush(brush);
    recordingCanvas.DrawRect(r);
    recordingCanvas.DetachBrush();
    brush.SetColor(Drawing::Color::COLOR_BLACK);
    brush.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    brush.SetAntiAlias(true);
    recordingCanvas.AttachBrush(brush);
    recordingCanvas.DrawOval(r);
    recordingCanvas.DetachBrush();
    return bitmap.MakeImage();
}

Drawing::RSXform RSParticleRenderModifier::MakeRSXform(Vector2f center, Vector2f position, float spin, float scale)
{
    float cos = std::cos(spin * DEGREE_TO_RADIAN) * scale;
    float sin = std::sin(spin * DEGREE_TO_RADIAN) * scale;
    float tx = position.x_ - cos * center.x_ + sin * center.y_;
    float ty = position.y_ - sin * center.x_ - cos * center.y_;
    return Drawing::RSXform::Make(cos, sin, tx, ty);
}

void RSParticleRenderModifier::CaculatePointAtlsArry(
    const std::shared_ptr<RSRenderParticle>& particle, Vector2f position, float opacity, float scale)
{
    if (particle == nullptr) {
        return;
    }
    if (circleImage_ == nullptr) {
        circleImage_ = MakeCircleImage(DEFAULT_RADIUS);
    }
    auto radius = particle->GetRadius();
    Color color = particle->GetColor();
    auto alpha = color.GetAlpha();
    color.SetAlpha(alpha * opacity);
    pointRsxform_.push_back(
        MakeRSXform(Vector2f(DEFAULT_RADIUS, DEFAULT_RADIUS), position, 0.f, radius * scale / DEFAULT_RADIUS));
    pointTex_.push_back(Drawing::Rect(0, 0, DEFAULT_RADIUS * DOUBLE, DEFAULT_RADIUS * DOUBLE));
    pointColors_.push_back(Drawing::Color(color.AsArgbInt()).CastToColorQuad());
    pointCount_++;
}

void RSParticleRenderModifier::CaculateImageAtlsArry(Drawing::Canvas& canvas,
    const std::shared_ptr<RSRenderParticle>& particle, Vector2f position, float opacity, float scale)
{
    if (particle == nullptr) {
        return;
    }
    auto image = particle->GetImage();
    if (image == nullptr) {
        return;
    }
    auto pixelmap = image->GetPixelMap();
    if (pixelmap == nullptr) {
        return;
    }
    auto imageIndex = particle->GetImageIndex();
    if (imageIndex >= imageCount_) {
        return;
    }
    auto imageSize = particle->GetImageSize();
    auto spin = particle->GetSpin();
    float left = position.x_;
    float top = position.y_;
    float width = imageSize.x_;
    float height = imageSize.y_;
    RectF destRect(left, top, width, height);
    auto imageFit = image->GetImageFit();
    if (imageFit != ImageFit::FILL) {
        image->SetFrameRect(destRect);
        image->ApplyImageFit();
        Color color = particle->GetColor();
        auto alpha = color.GetAlpha();
        color.SetAlpha(alpha * opacity);
        imageRsxform_[imageIndex].push_back(
            MakeRSXform(Vector2f(pixelmap->GetWidth() / DOUBLE, pixelmap->GetHeight() / DOUBLE), position, spin,
                image->GetDstRect().GetWidth() / pixelmap->GetWidth() * scale));
        imageTex_[imageIndex].push_back(Drawing::Rect(0, 0, pixelmap->GetWidth(), pixelmap->GetHeight()));
        imageColors_[imageIndex].push_back(Drawing::Color(color.AsArgbInt()).CastToColorQuad());
        count_[imageIndex]++;
    } else {
        DrawImageFill(canvas, particle, position, opacity, scale);
    }
}

void RSParticleRenderModifier::DrawImageFill(Drawing::Canvas& canvas, const std::shared_ptr<RSRenderParticle>& particle,
    Vector2f position, float opacity, float scale)
{
    if (particle == nullptr) {
        return;
    }
    auto image = particle->GetImage();
    if (image == nullptr) {
        return;
    }
    auto imageSize = particle->GetImageSize();
    auto spin = particle->GetSpin();
    float left = position.x_;
    float top = position.y_;
    float right = position.x_ + imageSize.x_ * scale;
    float bottom = position.y_ + imageSize.y_ * scale;
    canvas.Save();
    canvas.Translate(position.x_, position.y_);
    canvas.Rotate(spin, imageSize.x_ * scale / DOUBLE, imageSize.y_ * scale / DOUBLE);
    image->SetScale(scale);
    image->SetImageRepeat(0);
    Drawing::Rect rect { left, top, right, bottom };
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetAlphaF(opacity);
    canvas.AttachBrush(brush);
    image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
    canvas.DetachBrush();
    canvas.Restore();
}

void RSParticleRenderModifier::DrawParticles(Drawing::Canvas& canvas)
{
    if (circleImage_ != nullptr) {
        DrawCircle(canvas);
    }
    if (imageCount_ > 0) {
        DrawImages(canvas);
    }
}

void RSParticleRenderModifier::DrawCircle(Drawing::Canvas& canvas)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::RSXform* rsxform = pointRsxform_.data();
    Drawing::Rect* tex = pointTex_.data();
    Drawing::ColorQuad* colors = pointColors_.data();
    canvas.AttachBrush(brush);
    while (pointCount_ > MAX_ATLAS_COUNT) {
        canvas.DrawAtlas(circleImage_.get(), rsxform, tex, colors, MAX_ATLAS_COUNT, Drawing::BlendMode::DST_IN,
            Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), nullptr);
        pointCount_ -= MAX_ATLAS_COUNT;
        rsxform += MAX_ATLAS_COUNT;
        tex += MAX_ATLAS_COUNT;
        colors += MAX_ATLAS_COUNT;
    }
    canvas.DrawAtlas(circleImage_.get(), rsxform, tex, colors, pointCount_, Drawing::BlendMode::DST_IN,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), nullptr);
    canvas.DetachBrush();
}

void RSParticleRenderModifier::DrawImages(Drawing::Canvas& canvas)
{
    int imgIdx = imageCount_;
    while (imgIdx--) {
        if (imageVector_[imgIdx] != nullptr) {
            auto pixelmap = imageVector_[imgIdx]->GetPixelMap();
            if (!pixelmap) {
                ROSEN_LOGE("RSParticleRenderModifier::Draw !pixel");
                return;
            }
            auto image = RSPixelMapUtil::ExtractDrawingImage(pixelmap);
            if (!image) {
                ROSEN_LOGE("RSParticleRenderModifier::Draw !image");
                return;
            }
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            int count = count_[imgIdx];
            Drawing::RSXform* rsxform = imageRsxform_[imgIdx].data();
            Drawing::Rect* tex = imageTex_[imgIdx].data();
            Drawing::ColorQuad* colors = imageColors_[imgIdx].data();
            canvas.AttachBrush(brush);
            while (count > MAX_ATLAS_COUNT) {
                canvas.DrawAtlas(image.get(), rsxform, tex, colors, MAX_ATLAS_COUNT, Drawing::BlendMode::SRC_IN,
                    Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), nullptr);
                count -= MAX_ATLAS_COUNT;
                rsxform += MAX_ATLAS_COUNT;
                tex += MAX_ATLAS_COUNT;
                colors += MAX_ATLAS_COUNT;
            }
            canvas.DrawAtlas(image.get(), rsxform, tex, colors, count, Drawing::BlendMode::SRC_IN,
                Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), nullptr);
            canvas.DetachBrush();
        }
    }
}

void RSParticleRenderModifier::ClearStagingValues()
{
    stagingParticles_.clear();
    stagingImageCount_ = 0;
    stagingImageVector_.clear();
    stagingCount_.clear();
    stagingImageRsxform_.clear();
    stagingImageTex_.clear();
    stagingImageColors_.clear();
}
} // namespace OHOS::Rosen::ModifierNG
