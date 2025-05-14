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

#include "platform/common/rs_log.h"
#include "render/rs_particles_drawable.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {
constexpr float DEGREE_TO_RADIAN = M_PI / 180;
constexpr float DOUBLE = 2.f;
constexpr float DEFAULT_RADIUS = 100;
constexpr int MAX_ATLAS_COUNT = 2000;
RSParticlesDrawable::RSParticlesDrawable(const std::vector<std::shared_ptr<RSRenderParticle>>& particles,
    std::vector<std::shared_ptr<RSImage>>& imageVector, size_t imageCount)
    : particles_(particles), imageVector_(imageVector), imageCount_(imageCount)
{
    count_.resize(imageCount_);
    imageRsxform_.resize(imageCount_);
    imageTex_.resize(imageCount_);
    imageColors_.resize(imageCount_);
}

std::shared_ptr<Drawing::Image> RSParticlesDrawable::MakeCircleImage(int radius)
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

/**
 *  A compressed form of a rotation+scale matrix.
 *
 *  [ cos     -sin    tx ]
 *  [ sin      cos    ty ]
 *  [  0        0      1 ]
 */
Drawing::RSXform RSParticlesDrawable::MakeRSXform(Vector2f center, Vector2f position, float spin, float scale)
{
    float cos = std::cos(spin * DEGREE_TO_RADIAN) * scale;
    float sin = std::sin(spin * DEGREE_TO_RADIAN) * scale;
    float tx = position.x_ - cos * center.x_ + sin * center.y_;
    float ty = position.y_ - sin * center.x_ - cos * center.y_;
    return Drawing::RSXform::Make(cos, sin, tx, ty);
}

void RSParticlesDrawable::CaculatePointAtlsArry(
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

void RSParticlesDrawable::CaculateImageAtlsArry(Drawing::Canvas& canvas,
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
    auto drawImage = image->GetImage();
    if (pixelmap == nullptr && drawImage == nullptr) {
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
        auto width = pixelmap == nullptr ? drawImage->GetWidth() : pixelmap->GetWidth();
        auto height = pixelmap == nullptr ? drawImage->GetHeight() : pixelmap->GetHeight();
        imageRsxform_[imageIndex].push_back(
            MakeRSXform(Vector2f(width / DOUBLE, height / DOUBLE), position, spin,
                image->GetDstRect().GetWidth() / width * scale));
        imageTex_[imageIndex].push_back(Drawing::Rect(0, 0, width, height));
        imageColors_[imageIndex].push_back(Drawing::Color(color.AsArgbInt()).CastToColorQuad());
        count_[imageIndex]++;
    } else {
        DrawImageFill(canvas, particle, position, opacity, scale);
    }
}

void RSParticlesDrawable::DrawImageFill(Drawing::Canvas& canvas, const std::shared_ptr<RSRenderParticle>& particle,
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

void RSParticlesDrawable::Draw(Drawing::Canvas& canvas, std::shared_ptr<RectF> bounds)
{
    if (particles_.empty()) {
        ROSEN_LOGE("RSParticlesDrawable::Draw particles_ is empty");
        return;
    }
    for (const auto& particle : particles_) {
        if (particle != nullptr && particle->IsAlive()) {
            auto position = particle->GetPosition();
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            if (bounds == nullptr || !(bounds->Intersect(position.x_, position.y_)) || opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
            auto clipBounds = Drawing::Rect(
                bounds->left_, bounds->top_, bounds->left_ + bounds->width_, bounds->top_ + bounds->height_);
            canvas.ClipRect(clipBounds, Drawing::ClipOp::INTERSECT, true);
            if (particleType == ParticleType::POINTS) {
                CaculatePointAtlsArry(particle, position, opacity, scale);
            } else {
                CaculateImageAtlsArry(canvas, particle, position, opacity, scale);
            }
        }
    }
    DrawParticles(canvas);
}

void RSParticlesDrawable::DrawParticles(Drawing::Canvas& canvas)
{
    if (circleImage_ != nullptr) {
        DrawCircle(canvas);
    }
    if (imageCount_ > 0) {
        DrawImages(canvas);
    }
}

void RSParticlesDrawable::DrawCircle(Drawing::Canvas& canvas)
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

bool RSParticlesDrawable::CheckImageNull(std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Drawing::Image>& drawImage)
{
    if (image) {
        return false;
    }

    if (drawImage) {
        image = drawImage;
        return false;
    }

    return true;
}

void RSParticlesDrawable::DrawImages(Drawing::Canvas& canvas)
{
    while (imageCount_--) {
        if (imageVector_[imageCount_] != nullptr) {
            std::shared_ptr<Drawing::Image> image = nullptr;
            auto pixelmap = imageVector_[imageCount_]->GetPixelMap();
            if (pixelmap) {
                image = RSPixelMapUtil::ExtractDrawingImage(pixelmap);
            }

            if (CheckImageNull(image, imageVector_[imageCount_]->GetImage())) {
                ROSEN_LOGE("RSParticlesDrawable::Draw !pixel and !image_");
                return;
            }
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            int count = count_[imageCount_];
            Drawing::RSXform* rsxform = imageRsxform_[imageCount_].data();
            Drawing::Rect* tex = imageTex_[imageCount_].data();
            Drawing::ColorQuad* colors = imageColors_[imageCount_].data();
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

} // namespace Rosen
} // namespace OHOS
