/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
using namespace Rosen;
using namespace Drawing;

LayerContext::LayerContext(IRect dst, IRect src, uint32_t zorder, LayerType layerType)
    : dst_(dst), src_(src), zorder_(zorder), layerType_(layerType)
{
    cSurface_ = Surface::CreateSurfaceAsConsumer();
    cSurface_->SetDefaultWidthAndHeight(src.w, src.h);
    cSurface_->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);

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

void LayerContext::OnBufferAvailable() {}

const std::shared_ptr<HdiLayerInfo> LayerContext::GetHdiLayer()
{
    return hdiLayer_;
}

SurfaceError LayerContext::DrawBuffer(TestFunc testFunc)
{
    OHOS::sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    BufferRequestConfig config = {
        .width = src_.w,
        .height = src_.h,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = pSurface_->GetDefaultUsage(),
    };

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

    auto addr = static_cast<uint8_t*>(buffer->GetVirAddr());
    LOGI("buffer w:%{public}d h:%{public}d stride:%{public}d", buffer->GetWidth(), buffer->GetHeight(),
        buffer->GetBufferHandle()->stride);
    DrawBaseLayer(addr, buffer->GetWidth(), buffer->GetHeight(), testFunc);

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

SurfaceError LayerContext::FillHDILayer()
{
    OHOS::sptr<SurfaceBuffer> buffer = nullptr;
    int32_t acquireFence = -1;
    int64_t timestamp;
    OHOS::Rect damage;
    SurfaceError ret = cSurface_->AcquireBuffer(buffer, acquireFence, timestamp, damage);
    UniqueFd acquireFenceFd(acquireFence);
    if (ret != SURFACE_ERROR_OK) {
        LOGE("Acquire buffer failed");
        return ret;
    }

    LayerAlpha alpha = { .enPixelAlpha = true };

    hdiLayer_->SetSurface(cSurface_);
    auto acquireSyncFence = new SyncFence(acquireFenceFd.Release());
    hdiLayer_->SetBuffer(buffer, acquireSyncFence, prevBuffer_, prevFence_);
    hdiLayer_->SetZorder(static_cast<int32_t>(zorder_));
    hdiLayer_->SetAlpha(alpha);
    hdiLayer_->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    hdiLayer_->SetVisibleRegion(1, src_);
    hdiLayer_->SetDirtyRegion(src_);
    hdiLayer_->SetLayerSize(dst_);
    hdiLayer_->SetBlendType(BlendType::BLEND_SRCOVER);
    hdiLayer_->SetCropRect(src_);
    hdiLayer_->SetPreMulti(false);

    prevBuffer_ = buffer;
    prevFence_ = acquireSyncFence;

    return ret;
}

void LayerContext::DrawBaseLayer(void* image, int width, int height, TestFunc testFunc)
{
    Bitmap bitmap;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE };
    bitmap.Build(width, height, format);

    Canvas canvas;
    canvas.Bind(bitmap);
    if (layerType_ == LayerType::LAYER_LAUNCHER) {
        canvas.Clear(Color::COLOR_WHITE);
        testFunc(canvas, width, height);
    }
    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    auto ret = memcpy_s(image, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        LOGE("memcpy_s failed");
    }
}