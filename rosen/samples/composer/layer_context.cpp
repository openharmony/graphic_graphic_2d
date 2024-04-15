/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "layer_context.h"

#include <securec.h>
#include "hdi_log.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
#define LOGI(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(            \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloComposer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define LOGE(fmt, ...) ::OHOS::HiviewDFX::HiLog::Error(           \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloComposer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
}

LayerContext::LayerContext(GraphicIRect dst, GraphicIRect src, uint32_t zorder, LayerType layerType)
    : cSurface_(IConsumerSurface::Create()), dst_(dst), src_(src), zorder_(zorder), layerType_(layerType)
{
    cSurface_->SetDefaultWidthAndHeight(src.w, src.h);
    cSurface_->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);

    OHOS::sptr<IBufferProducer> producer = cSurface_->GetProducer();
    pSurface_ = Surface::CreateSurfaceAsProducer(producer);
    cSurface_->RegisterConsumerListener(this);

    hdiLayer_ = HdiLayerInfo::CreateHdiLayerInfo();
    LOGI("%{public}s: create surface w:%{public}d, h:%{public}d", __func__, src.w, src.h);
}

LayerContext::~LayerContext()
{
    cSurface_ = nullptr;
    pSurface_ = nullptr;
    prevBuffer_ = nullptr;
    hdiLayer_ = nullptr;
}

void LayerContext::OnBufferAvailable()
{
}

void LayerContext::SetTestClientStatus(bool status)
{
    testClient_ = status;
}

void LayerContext::SetTestRotateStatus(bool status)
{
    testRotate_ = status;
}

void LayerContext::SetTestYUVStatus(bool status)
{
    testYUV_ = status;
}

void LayerContext::SetTestLayerColor(bool status)
{
    testLayerColor_ = status;
}

OHOS::Rosen::LayerType LayerContext::GetLayerType() const
{
    return layerType_;
}

const std::shared_ptr<HdiLayerInfo> LayerContext::GetHdiLayer()
{
    return hdiLayer_;
}

SurfaceError LayerContext::DrawBufferColor()
{
    OHOS::sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    BufferRequestConfig config = {
        .width = src_.w,
        .height = src_.h,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = pSurface_->GetDefaultUsage(),
    };

    if (layerType_ >= LayerType::LAYER_EXTRA && testYUV_) {
        config.format = GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
    }

    SurfaceError ret = pSurface_->RequestBuffer(buffer, releaseFence, config);
    if (ret != 0) {
        LOGE("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
        return ret;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(100); // 100 ms

    if (buffer == nullptr) {
        LOGE("%s: buffer is nullptr", __func__);
        return SURFACE_ERROR_NULLPTR;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    LOGI("buffer w:%{public}d h:%{public}d stride:%{public}d", buffer->GetWidth(),
          buffer->GetHeight(), buffer->GetBufferHandle()->stride);
    DrawColor(addr, buffer->GetWidth(), buffer->GetHeight());

    BufferFlushConfig flushConfig = {
        .damage = {
        .w = src_.w,
        .h = src_.h,
        },
    };

    ret = pSurface_->FlushBuffer(buffer, -1, flushConfig);
    if (ret != SURFACE_ERROR_OK) {
        LOGE("FlushBuffer failed");
    }

    return ret;
}

void LayerContext::FillHDILayer()
{
    GraphicLayerAlpha alpha = { .enPixelAlpha = true };
    hdiLayer_->SetZorder(static_cast<int32_t>(zorder_));
    hdiLayer_->SetAlpha(alpha);
    if (layerType_ >= LayerType::LAYER_EXTRA) {
        SetLayerTransformType();
    }

    SetLayerCompositionType();
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(src_);
    hdiLayer_->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(src_);
    hdiLayer_->SetDirtyRegions(dirtyRegions);
    hdiLayer_->SetLayerSize(dst_);

    if (testLayerColor_) {
        hdiLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);
    } else {
        hdiLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    }

    hdiLayer_->SetPreMulti(true);
    hdiLayer_->SetCropRect(src_);
}

SurfaceError LayerContext::FillHDIBuffer()
{
    OHOS::sptr<SurfaceBuffer> buffer = nullptr;
    int32_t acquireFence = -1;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = cSurface_->AcquireBuffer(buffer, acquireFence, timestamp, damage);
    sptr<SyncFence> acquireSyncFence = new SyncFence(acquireFence);
    if (ret != SURFACE_ERROR_OK) {
        LOGE("Acquire buffer failed");
        return ret;
    }

    hdiLayer_->SetSurface(cSurface_);

    if (testLayerColor_) {
        if (layerType_ != LayerType::LAYER_LAUNCHER) {
            hdiLayer_->SetBuffer(buffer, acquireSyncFence);
        } else {
            const uint32_t COLOR_R = 255;   // 255 is red color
            const uint32_t COLOR_G = 255;   // 255 is green color
            const uint32_t COLOR_B = 255;   // 255 is blue color
            const uint32_t COLOR_A = 255;   // 255 is alpha
            GraphicLayerColor color = {
                .r = COLOR_R,
                .g = COLOR_G,
                .b = COLOR_B,
                .a = COLOR_A
            };

            hdiLayer_->SetLayerColor(color);
        }
    } else {
        hdiLayer_->SetBuffer(buffer, acquireSyncFence);
    }

    prevBuffer_ = buffer;
    prevFence_ = acquireSyncFence;

    return ret;
}

void LayerContext::SetLayerTransformType()
{
    if (!testRotate_) {
        return;
    }

    static int32_t count = 0;
    if (count >= 2000) { // 2000 is max cycle num
        count = 0;
    }

    if (count >= 100 && count <= 200) { // 100-200 ROTATE_90
        hdiLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    } else if (count >= 500 && count <= 600) { // 500-600 ROTATE_180
        hdiLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_180);
    } else if (count >= 900 && count <= 1000) { // 900-1000 ROTATE_270
        hdiLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_270);
    } else if (count >= 1300 && count <= 1400) { // 1300-1400 ROTATE_NONE
        hdiLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    }

    count++;
}

void LayerContext::SetLayerCompositionType()
{
    if (layerType_ >= LayerType::LAYER_EXTRA && testClient_) {
        hdiLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    } else if (layerType_ == LayerType::LAYER_LAUNCHER && testLayerColor_) {
        hdiLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    } else {
        hdiLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    }
}

void LayerContext::DrawColor(void *image, int width, int height)
{
    if (layerType_ >= LayerType::LAYER_EXTRA) {
        DrawExtraColor(image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    } else {
        DrawBaseColor(image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }
}

void LayerContext::DrawExtraColor(void *image, uint32_t width, uint32_t height)
{
    frameCounter_ = frameCounter_ % 60; // 60 is cycle size
    if (frameCounter_ == 0) {
        colorIndex_ = colorIndex_ % colors_.size();
        color_ = colors_[colorIndex_];
        colorIndex_++;
    }
    frameCounter_++;

    YUVPixel pixelValueYUV;
    if (testYUV_) {
        LOGI("DrawExtraColor with PIXEL_FMT_YCBCR_420_SP format.");
        ConvertRBGA2YUV(color_, &pixelValueYUV);
        DrawYUVColor(image, width, height, pixelValueYUV);
        return;
    }

    LOGI("DrawExtraColor with GRAPHIC_PIXEL_FMT_RGBA_8888 format.");
    uint32_t *pixel = static_cast<uint32_t *>(image);
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0;  y < height; y++) {
            if (testRotate_ && x <= 50) { // 0-50 is different color
                *pixel++ = 0xffff1111;
            } else {
                *pixel++ = color_;
            }
        }
    }
}

void LayerContext::DrawBaseColor(void *image, uint32_t width, uint32_t height)
{
    static uint32_t value = 0x00;
    if (layerType_ == LayerType::LAYER_STATUS) {
        value = 0xfff0000f;
    } else if (layerType_ == LayerType::LAYER_LAUNCHER) {
        value = 0xffffffff;
    } else {
        value = 0xff00ffff;
    }

    uint32_t *pixel = static_cast<uint32_t *>(image);
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0;  y < height; y++) {
            *pixel++ = value;
        }
    }
}

void LayerContext::ConvertRBGA2YUV(uint32_t pixelValueRBGA, YUVPixel *pixelValueYUV)
{
    // Get the components of pixelValueRBGA
    uint8_t R = pixelValueRBGA >> RBGA_R_MOVEBITS;
    uint8_t B = pixelValueRBGA >> RBGA_B_MOVEBITS;
    uint8_t G = pixelValueRBGA >> RBGA_G_MOVEBITS;

    // Convert pixel from RBGA formate to YUV formate with the formula:
    // fixed formula : Y = 0.299 * R + 0.587 * G + 0.114 * B;
    pixelValueYUV->y = 0.299 * R + 0.587 * G + 0.114 * B;
    // fixed formula : U = -0.169 * R - 0.331 * G + 0.500 * B + 128;
    pixelValueYUV->u = -0.169 * R - 0.331 * G + 0.500 * B + 128;
    // fixed formula : V = 0.500 * R - 0.419 * G - 0.081 * B + 128;
    pixelValueYUV->v = 0.500 * R - 0.419 * G - 0.081 * B + 128;
}

void LayerContext::DrawYUVColor(void *image, uint32_t width, uint32_t height, YUVPixel pixelValueYUV)
{
    uint8_t *pixel = static_cast<uint8_t *>(image);
    width = (width / PIXEL_LINE_ALIGNMENT + 1) * PIXEL_LINE_ALIGNMENT;
    for (uint32_t index = 0; index < width * height * PIXEL_YCBCR420_BYTE; index++) {
        if (index < width * height) {
            *pixel++ = pixelValueYUV.y;
            continue;
        }
        if (index % PIXEL_YCBCR420_UV_NUM == 0) {
            *pixel++ = pixelValueYUV.u;
        } else {
            *pixel++ = pixelValueYUV.v;
        }
    }
}

