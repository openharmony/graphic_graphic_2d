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

#include "render/rs_render_light_blur_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "draw/surface.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
const int MAX_DOWN_SAMPLE_NUM = 12;
const int DOWN_SAMPLE_BOUNDARY_PIXEL_SIZE = 100;
const int DOWN_SAMPLE_STEP = 4;
const float DOWN_SAMPLE_4X_OFFSET = 0.25f;
const Drawing::SamplingOptions SAMPLING_OPTIONS(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
}
std::shared_ptr<Drawing::RuntimeEffect> RSLightBlurShaderFilter::downSample4xAndMixShader_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSLightBlurShaderFilter::downSample4xShader_ = nullptr;

bool RSLightBlurShaderFilter::InitDownSample4xAndMixShader()
{
    if (downSample4xAndMixShader_ != nullptr) {
        return true;
    }

    static const std::string mixString(R"(
        uniform shader imageInput1;
        uniform shader imageInput2;
        uniform shader imageInput3;
        uniform float2 offset;
        const half COLOR1_WEIGHT = 0.2;
        const half COLOR2_WEIGHT = 0.3;
        const half COLOR3_WEIGHT = 0.5;
        const half COLOR_THRESHOLD = 5;
        const half COLOR_MAX = 255;

        half4 mixColor(half4 color1, half4 color2, half4 color3) {
            return color1 * COLOR1_WEIGHT + color2 * COLOR2_WEIGHT + color3 * COLOR3_WEIGHT;
        }

        half4 getImageInput3Color(float2 xy) {
            half4 c = imageInput3.eval(float2(xy.x + offset.x, xy.y + offset.y));
            c += imageInput3.eval(float2(xy.x - offset.x, xy.y + offset.y));
            c += imageInput3.eval(float2(xy.x + offset.x, xy.y - offset.y));
            c += imageInput3.eval(float2(xy.x - offset.x, xy.y - offset.y));
            return c * 0.25;
        }

        half4 main(float2 xy) {
            half4 color1 = imageInput1.eval(xy) * COLOR_MAX;
            half4 color2 = imageInput2.eval(xy) * COLOR_MAX;
            half4 color3 = getImageInput3Color(xy)  * COLOR_MAX;
            half4 mixedColor = mixColor(color1, color2, color3);
            half4 colorDist = mixedColor - color2;
            half dist = dot(colorDist, colorDist);
            if (dist < COLOR_THRESHOLD) {
                return clamp(mixedColor / COLOR_MAX, 0, 1);
            }
            half4 absColorDist = abs(colorDist);
            half4 weight = half4(colorDist.r * absColorDist.r, colorDist.g * absColorDist.g,
                colorDist.b * absColorDist.b, colorDist.a * absColorDist.a);
            return clamp((color2 + weight * COLOR_THRESHOLD / dist) / COLOR_MAX, 0, 1);
        }
    )");

    downSample4xAndMixShader_ = Drawing::RuntimeEffect::CreateForShader(mixString);
    if (downSample4xAndMixShader_ == nullptr) {
        ROSEN_LOGE("RSLightBlurShaderFilter::InitDownSample4xAndMixShader create shader failed");
        return false;
    }

    return true;
}

bool RSLightBlurShaderFilter::InitDownSample4xShader()
{
    if (downSample4xShader_ != nullptr) {
        return true;
    }

    static const std::string downSample4xString(R"(
        uniform shader imageInput;
        uniform float2 offset;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(float2(xy.x + offset.x, xy.y + offset.y));
            c += imageInput.eval(float2(xy.x - offset.x, xy.y + offset.y));
            c += imageInput.eval(float2(xy.x + offset.x, xy.y - offset.y));
            c += imageInput.eval(float2(xy.x - offset.x, xy.y - offset.y));
            return half4(c.rgba * 0.25);
        }
    )");

    downSample4xShader_ = Drawing::RuntimeEffect::CreateForShader(downSample4xString);
    if (downSample4xShader_ == nullptr) {
        ROSEN_LOGE("RSLightBlurShaderFilter::InitDownSample4xShader create shader failed");
        return false;
    }

    return true;
}

int RSLightBlurShaderFilter::GetRadius() const
{
    return radius_;
}

Drawing::RectI RSLightBlurShaderFilter::GetDownSampleRect(int width, int height) const
{
    int downSampleWidth = width <= DOWN_SAMPLE_STEP ? 1 : width / DOWN_SAMPLE_STEP;
    int downSampleHeight = height <= DOWN_SAMPLE_STEP ? 1 : height / DOWN_SAMPLE_STEP;
    return Drawing::RectI(0, 0, downSampleWidth, downSampleHeight);
}

std::shared_ptr<Drawing::Image> RSLightBlurShaderFilter::GetDownSampleImage(Drawing::Image& srcImage,
    Drawing::Surface& surface, Drawing::RectI& src, Drawing::RectI& dst) const
{
    int dstWidth = dst.GetWidth();
    int dstHeight = dst.GetHeight();
    if (dstWidth <= 0 || dstHeight <= 0) {
        return nullptr;
    }

    auto offscreenSurface = surface.MakeSurface(dstWidth, dstHeight);
    if (offscreenSurface == nullptr) {
        return nullptr;
    }

    auto offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return nullptr;
    }

    Drawing::Brush brush;
    offscreenCanvas->AttachBrush(brush);
    offscreenCanvas->DrawImageRect(srcImage, Drawing::Rect(src), Drawing::Rect(dst), SAMPLING_OPTIONS);
    offscreenCanvas->DetachBrush();
    return offscreenSurface->GetImageSnapshot();
}

std::shared_ptr<Drawing::Image> RSLightBlurShaderFilter::GetDownSampleImage4x(
    const std::shared_ptr<Drawing::Image>& srcImage, Drawing::Canvas& canvas,
    Drawing::RectI& src, Drawing::RectI& dst) const
{
    if (srcImage == nullptr) {
        return nullptr;
    }

    int dstWidth = dst.GetWidth();
    int dstHeight = dst.GetHeight();
    if (dstWidth <= 0 || dstHeight <= 0) {
        return nullptr;
    }

    if (downSample4xShader_ == nullptr) {
        return nullptr;
    }

    float scaleW = src.GetWidth() > 0 ? (dstWidth / static_cast<float>(src.GetWidth())) : 1.f;
    float scaleH = src.GetHeight() > 0 ? (dstHeight / static_cast<float>(src.GetHeight())) : 1.f;
    Drawing::Matrix matrix;
    matrix.SetScale(scaleW, scaleH);
    auto originImageInfo = srcImage->GetImageInfo();
    auto middleInfo = Drawing::ImageInfo(dstWidth, dstHeight, originImageInfo.GetColorType(),
        originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());

    Drawing::RuntimeShaderBuilder downSample4xBuilder(downSample4xShader_);
    downSample4xBuilder.SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*srcImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, SAMPLING_OPTIONS, matrix));

    downSample4xBuilder.SetUniform("offset", DOWN_SAMPLE_4X_OFFSET, DOWN_SAMPLE_4X_OFFSET);
    return downSample4xBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, middleInfo, false);
}

std::shared_ptr<Drawing::Image> RSLightBlurShaderFilter::GetDownSample4xAndMixImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image) const
{
    if (downSample4xAndMixShader_ == nullptr) {
        return nullptr;
    }

    if (image == nullptr) {
        return nullptr;
    }

    auto twoFrameBeforeImage = lightBlurResultCache_[0];
    auto oneFrameBeforeImage = lightBlurResultCache_[1];
    if (twoFrameBeforeImage == nullptr || oneFrameBeforeImage == nullptr) {
        return nullptr;
    }

    Drawing::Matrix matrix;
    matrix.SetScale(1, 1);
    Drawing::Matrix matrix2;
    float scaleW = image->GetWidth() > 0 ? 1.f / image->GetWidth() : 1.f;
    float scaleH = image->GetHeight() > 0 ? 1.f / image->GetHeight() : 1.f;
    matrix2.SetScale(scaleW, scaleH);

    auto originImageInfo = image->GetImageInfo();
    auto middleInfo = Drawing::ImageInfo(1, 1, originImageInfo.GetColorType(),
        originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());

    Drawing::RuntimeShaderBuilder downsample4xAndMixBuilder(downSample4xAndMixShader_);
    downsample4xAndMixBuilder.SetChild("imageInput1", Drawing::ShaderEffect::CreateImageShader(*twoFrameBeforeImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, SAMPLING_OPTIONS, matrix));
    downsample4xAndMixBuilder.SetChild("imageInput2", Drawing::ShaderEffect::CreateImageShader(*oneFrameBeforeImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, SAMPLING_OPTIONS, matrix));
    downsample4xAndMixBuilder.SetChild("imageInput3", Drawing::ShaderEffect::CreateImageShader(*image,
    Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, SAMPLING_OPTIONS, matrix2));
    downsample4xAndMixBuilder.SetUniform("offset", DOWN_SAMPLE_4X_OFFSET, DOWN_SAMPLE_4X_OFFSET);
    return downsample4xAndMixBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, middleInfo, false);
}

void RSLightBlurShaderFilter::ApplyLightBlur(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const LightBlurParameter& para)
{
    if (image == nullptr) {
        ROSEN_LOGE("RSLightBlurShaderFilter apply light blur failed, image is nullptr");
        return;
    }

    auto width = image->GetWidth();
    auto height = image->GetHeight();
    if (width <= 0 || height <= 0) {
        ROSEN_LOGE("RSLightBlurShaderFilter apply light blur failed, image width or height <= 0");
        return;
    }

    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSLightBlurShaderFilter apply light blur failed, surface is nullptr");
        return;
    }

    if (NeedClearLightBlurResultCache(canvas)) {
        ClearLightBlurResultCache();
    }

    bool hasDownSample4xShader = InitDownSample4xShader();
    bool canDownSample4xAndMixImage = InitDownSample4xAndMixShader() &&
        lightBlurResultCache_[0] != nullptr && lightBlurResultCache_[1] != nullptr;

    auto curImage = image;
    for (int i = 0; i < MAX_DOWN_SAMPLE_NUM; ++i) {
        if (width == 1 && height == 1) {
            break;
        }
        Drawing::RectI srcRect(0, 0, width, height);
        Drawing::RectI dstRect = GetDownSampleRect(width, height);
        std::shared_ptr<Drawing::Image> downSampleImage = nullptr;

        if (dstRect.GetWidth() == 1 && dstRect.GetHeight() == 1 && canDownSample4xAndMixImage) {
            downSampleImage = GetDownSample4xAndMixImage(canvas, curImage);
        } else if (width + height <= DOWN_SAMPLE_BOUNDARY_PIXEL_SIZE && hasDownSample4xShader) {
            downSampleImage = GetDownSampleImage4x(curImage, canvas, srcRect, dstRect);
        } else {
            downSampleImage = GetDownSampleImage(*curImage, *surface, srcRect, dstRect);
        }
        if (downSampleImage == nullptr) {
            break;
        }
        curImage = downSampleImage;
        width = dstRect.GetWidth();
        height = dstRect.GetHeight();
    }

    UpdateLightBlurResultCache(curImage);
    DrawImageOnCanvas(canvas, *curImage, para);
}

void RSLightBlurShaderFilter::DrawImageOnCanvas(Drawing::Canvas& canvas,
    const Drawing::Image& image, const LightBlurParameter& para)
{
    auto brush = para.brush;
    Drawing::Matrix matrix;
    matrix.SetScale(para.dst.GetWidth(), para.dst.GetHeight());
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, SAMPLING_OPTIONS, matrix);
    if (imageShader == nullptr) {
        canvas.AttachBrush(brush);
        canvas.DrawImageRect(image, para.dst, SAMPLING_OPTIONS);
        canvas.DetachBrush();
        return;
    }
    // the 1x1 shader loss is smaller than DrawImageRect in test.
    brush.SetShaderEffect(imageShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(para.dst);
    canvas.DetachBrush();
}

void RSLightBlurShaderFilter::UpdateLightBlurResultCache(const std::shared_ptr<Drawing::Image>& image)
{
    if (lightBlurResultCache_[1] == nullptr) {
        lightBlurResultCache_[0] = image;
        lightBlurResultCache_[1] = image;
        return;
    }
    lightBlurResultCache_[0] = lightBlurResultCache_[1];
    lightBlurResultCache_[1] = image;
}

bool RSLightBlurShaderFilter::NeedClearLightBlurResultCache(Drawing::Canvas& canvas) const
{
    return (lightBlurResultCache_[0] != nullptr && !lightBlurResultCache_[0]->IsValid(canvas.GetGPUContext().get())) ||
        (lightBlurResultCache_[1] != nullptr && !lightBlurResultCache_[1]->IsValid(canvas.GetGPUContext().get()));
}

void RSLightBlurShaderFilter::ClearLightBlurResultCache()
{
    lightBlurResultCache_[0] = nullptr;
    lightBlurResultCache_[1] = nullptr;
}
} // namespace Rosen
} // namespace OHOS