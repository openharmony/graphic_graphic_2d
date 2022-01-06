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

#include "pipeline/rs_hardware_processor.h"

#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "stdlib.h"

namespace OHOS {
namespace Rosen {

RSHardwareProcessor::RSHardwareProcessor() {}

RSHardwareProcessor::~RSHardwareProcessor() {}

void RSHardwareProcessor::Init(ScreenId id)
{
    backend_ = HdiBackend::GetInstance();
    backend_->RegPrepareComplete(std::bind(&RSHardwareProcessor::Redraw, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3), nullptr);
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        ROSEN_LOGE("RSHardwareProcessor::Init ScreenManager is nullptr");
        return;
    }
    output_ = screenManager_->GetOutput(id);
    screenManager_->GetScreenActiveMode(id, curScreenInfo);
    ROSEN_LOGI("RSHardwareProcessor::Init screen w:%{public}d, w:%{public}d",
        curScreenInfo.GetScreenWidth(), curScreenInfo.GetScreenHeight());
}

void RSHardwareProcessor::PostProcess()
{
    if (output_ == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::PostProcess output is nullptr");
        return;
    }

    output_->SetLayerInfo(layers_);
    std::vector<std::shared_ptr<HdiOutput>> outputs{output_};
    if (backend_) {
        backend_->Repaint(outputs);
    }
}

void RSHardwareProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    ROSEN_LOGI("RsDebug RSHardwareProcessor::ProcessSurface start");
    if (!output_) {
        ROSEN_LOGE("RSHardwareProcessor::ProcessSurface output is nullptr");
        return;
    }
    if (node.GetAvailableBufferCount() == 0 && !node.GetBuffer()) {
        ROSEN_LOGI("RsDebug RSHardwareProcessor::ProcessSurface have no Available Buffer and Node have no buffer");
        return;
    }
    auto surfaceConsumer = node.GetConsumer();
    if (!surfaceConsumer) {
        ROSEN_LOGI("RSHardwareProcessor::ProcessSurface output is nullptr");
        return;
    }

    if (node.GetAvailableBufferCount() >= 1) {
        OHOS::sptr<SurfaceBuffer> cbuffer = nullptr;
        int32_t fence = -1;
        int64_t timestamp;
        Rect damage;
        auto sret = surfaceConsumer->AcquireBuffer(cbuffer, fence, timestamp, damage);
        if (!cbuffer || sret != OHOS::SURFACE_ERROR_OK) {
            return;
        }
        node.SetBuffer(cbuffer);
        node.SetFence(fence);
        if (node.ReduceAvailableBuffer() >= 1) {
            if (auto mainThread = RSMainThread::Instance()) {
                mainThread->RequestNextVSync();
            }
        }
    }

    IRect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = node.GetBuffer()->GetWidth();
    srcRect.h = node.GetBuffer()->GetHeight();

    IRect dstRect;
    dstRect.x = node.GetRenderProperties().GetBoundsPositionX();
    dstRect.y = node.GetRenderProperties().GetBoundsPositionY();
    dstRect.w = node.GetRenderProperties().GetBoundsWidth();
    dstRect.h = node.GetRenderProperties().GetBoundsHeight();
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetSurface(surfaceConsumer);
    layer->SetBuffer(node.GetBuffer(), node.GetFence());
    layer->SetZorder(node.GetRenderProperties().GetPositionZ());
    layer->SetAlpha(alpha_);
    layer->SetLayerSize(dstRect);
    layer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    layer->SetVisibleRegion(1, srcRect);
    layer->SetDirtyRegion(srcRect);
    layer->SetBlendType(BlendType::BLEND_SRCOVER);
    layer->SetCropRect(srcRect);
    ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface surfaceNode [%d %d %d %d] buffer [%d %d] buffaddr:%p, z:%d",
        dstRect.x, dstRect.y, dstRect.w, dstRect.h, srcRect.w, srcRect.h,
        node.GetBuffer().GetRefPtr(), layer->GetZorder());
    layers_.emplace_back(layer);
}

void RSHardwareProcessor::Redraw(sptr<Surface>& surface, const struct PrepareCompleteParam& param, void* data)
{
    if (!param.needFlushFramebuffer) {
        ROSEN_LOGE("RSHardwareProcessor::Redraw: no need to flush Framebuffer.");
        return;
    }

    if (surface == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::Redraw: surface is null.");
        return;
    }

    BufferRequestConfig requestConfig = {
        .width = curScreenInfo.GetScreenWidth(),
        .height = curScreenInfo.GetScreenHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_BGRA_8888,      // [TODO] different soc need different format
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    auto canvas = CreateCanvas(surface, requestConfig);
    if (canvas == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::Redraw: canvas is null.");
        return;
    }

    for (auto layer : param.layers) {
        if (layer == nullptr) {
            continue;
        }
        SkMatrix matrix;
        matrix.reset();
        matrix.setTranslate(layer->GetLayerSize().x, layer->GetLayerSize().y);
        DrawBuffer(canvas.get(), matrix, layer->GetBuffer());
    }
    BufferFlushConfig flushConfig = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = curScreenInfo.GetScreenWidth(),
            .h = curScreenInfo.GetScreenHeight(),
        },
    };
    FlushBuffer(surface, flushConfig);
}
} // namespace Rosen
} // namespace OHOS
