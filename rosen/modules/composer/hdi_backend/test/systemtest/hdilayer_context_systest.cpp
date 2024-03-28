/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include "hdilayer_context_systest.h"

namespace OHOS {
namespace Rosen {
namespace MockSys {
class SurfaceListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

HdiLayerContext::HdiLayerContext(GraphicIRect dstRect, GraphicIRect srcRect, uint32_t zOrder)
    : cSurface_(IConsumerSurface::Create()), srcRect_(srcRect), dstRect_(dstRect), zOrder_(zOrder)
{
    cSurface_->SetDefaultWidthAndHeight(srcRect_.w, srcRect_.h);
    cSurface_->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);
    sptr<IBufferConsumerListener> listener = new SurfaceListener();
    cSurface_->RegisterConsumerListener(listener);
    sptr<IBufferProducer> producer = cSurface_->GetProducer();
    pSurface_ = Surface::CreateSurfaceAsProducer(producer);
    hdiLayer_ = HdiLayerInfo::CreateHdiLayerInfo();
}

HdiLayerContext::~HdiLayerContext()
{
    cSurface_ = nullptr;
    pSurface_ = nullptr;
    hdiLayer_ = nullptr;
}

std::shared_ptr<HdiLayerInfo> HdiLayerContext::GetHdiLayer()
{
    return hdiLayer_;
}

GSError HdiLayerContext::DrawBufferColor()
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    BufferRequestConfig config = {
        .width = srcRect_.w,
        .height = srcRect_.h,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = pSurface_->GetDefaultUsage(),
    };

    GSError ret = pSurface_->RequestBuffer(buffer, releaseFence, config);
    if (ret != 0) {
        return ret;
    }
    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(10); // 10 ms

    if (buffer == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    DrawColor(addr, (uint32_t)buffer->GetWidth(), (uint32_t)buffer->GetHeight());

    BufferFlushConfig flushConfig = {
        .damage = {
        .w = srcRect_.w,
        .h = srcRect_.h,
        },
    };

    ret = pSurface_->FlushBuffer(buffer, -1, flushConfig);
    return ret;
}

void HdiLayerContext::DrawColor(void *image, uint32_t width, uint32_t height)
{
    static uint32_t value = 0xff00ffff;
    uint32_t *pixel = static_cast<uint32_t *>(image);
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0;  y < height; y++) {
            *pixel++ = value;
        }
    }
}

GSError HdiLayerContext::FillHdiLayer()
{
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t acquireFence = -1;
    int64_t timestamp;
    Rect damage;
    GSError ret = cSurface_->AcquireBuffer(buffer, acquireFence, timestamp, damage);
    sptr<SyncFence> acquireSyncFence = new SyncFence(acquireFence);
    if (ret != SURFACE_ERROR_OK) {
        return ret;
    }
    GraphicLayerAlpha alpha = { .enPixelAlpha = true };
    hdiLayer_->SetSurface(cSurface_);
    hdiLayer_->SetBuffer(buffer, acquireSyncFence);
    hdiLayer_->SetZorder(static_cast<int32_t>(zOrder_));
    hdiLayer_->SetAlpha(alpha);
    hdiLayer_->SetLayerSize(dstRect_);
    hdiLayer_->SetCropRect(srcRect_);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(srcRect_);
    hdiLayer_->SetDirtyRegions(dirtyRegions);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(srcRect_);
    hdiLayer_->SetVisibleRegions(visibleRegions);
    hdiLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    hdiLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    hdiLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    hdiLayer_->SetPreMulti(false);
    return ret;
}

} // namespace MockSys
} // namespace Rosen
} // namespace OHOS