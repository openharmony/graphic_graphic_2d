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

#include "pipeline/rs_hardware_thread.h"

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_trace.h"
#include "hdi_backend.h"

namespace OHOS::Rosen {
RSHardwareThread& RSHardwareThread::Instance()
{
    static RSHardwareThread instance;
    return instance;
}

void RSHardwareThread::Start()
{
    RS_LOGI("RSHardwareThread::Start()!");
    hdiBackend_ = HdiBackend::GetInstance();
    runner_ = AppExecFwk::EventRunner::Create("RSHardwareThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    fallbackCb_ = std::bind(&RSHardwareThread::Redraw, this,std::placeholders::_1, std::placeholders::_2);
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, this);
    if (handler_) {
        ScheduleTask([=]() {
            auto screenManager = CreateOrGetScreenManager();
            if (screenManager == nullptr || !screenManager->Init()) {
                RS_LOGE("RSHardwareThread CreateOrGetScreenManager or init fail.");
                return;
            }
        }).wait();
    }
}

void RSHardwareThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSHardwareThread::ReleaseBuffer(sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence,
    sptr<Surface> cSurface)
{
    if (cSurface == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread:: ReleaseBuffer failed, no consumer!");
        return;
    }

    if (buffer != nullptr) {
        RS_TRACE_NAME("RSHardwareThread::ReleaseBuffer");
        auto ret = cSurface->ReleaseBuffer(buffer, releaseFence);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGE("RsDebug RSHardwareThread::ReleaseBuffer failed(ret: %d)!",
                buffer->GetSeqNum(), ret);
            return;
        }
        // reset prevBuffer if we release it successfully,
        // to avoid releasing the same buffer next frame in some situations.
        buffer = nullptr;
        releaseFence = SyncFence::INVALID_FENCE;
    }
}

void RSHardwareThread::ReleaseLayers(OutputPtr output, const std::unordered_map<uint32_t, LayerPtr>& layerMap)
{
    RS_TRACE_BEGIN("RSHardwareThread::ReleaseLayers");
    // get present timestamp from and set present timestamp to surface
    for (const auto& [id, layer] : layerMap) {
        if (layer == nullptr || layer->GetLayerInfo()->GetSurface() == nullptr) {
            RS_LOGW("RSHardwareThread::ReleaseLayers: layer or layer's cSurface is nullptr");
            continue;
        }
        LayerPresentTimestamp(layer->GetLayerInfo(), layer->GetLayerInfo()->GetSurface());
    }

    // set all layers' releaseFence.
    const auto layersReleaseFence = hdiBackend_->GetLayersReleaseFence(output);
    if (layersReleaseFence.size() == 0) {
        RS_LOGE("RSHardwareThread::ReleaseLayers: no layer needs to release");
    }
    for (const auto& [layer, fence] : layersReleaseFence) {
        if (layer == nullptr) {
            continue;
        }
        auto preBuffer = layer->GetPreBuffer();
        auto consumer = layer->GetSurface();
        ReleaseBuffer(preBuffer, fence, consumer);
    }
    RS_TRACE_END();
}

void RSHardwareThread::CommitAndReleaseLayers(OutputPtr output, const std::vector<LayerInfoPtr>& layers)
{
    if (!handler_) {
        RS_LOGE("RSHardwareThread::CommitAndReleaseLayers handler is nullptr");
        return;
    }
    RSTaskMessage::RSTask task = [this, output = output, layers = layers]() {
        RS_TRACE_NAME("RSHardwareThread::CommitAndReleaseLayers");
        output->SetLayerInfo(layers);
        hdiBackend_->Repaint(output);
        auto layerMap = output->GetLayers();
        ReleaseLayers(output, layerMap);
    };
    PostTask(task);
}

void RSHardwareThread::OnPrepareComplete(sptr<Surface>& surface,
    const PrepareCompleteParam& param, void* data)
{
    // unused data.
    (void)(data);

    if (!param.needFlushFramebuffer) {
        RS_LOGD("RsDebug RSHardwareThread::OnPrepareComplete: no need to flush frame buffer");
        return;
    }

    if (fallbackCb_ != nullptr) {
        fallbackCb_(surface, param.layers);
    }
}

void RSHardwareThread::Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers)
{
    (void)(surface);
    (void)(layers);
    // planning: RS RSHardwareThread will support Redraw function when HWC composition is ready.
}

// private func, guarantee the layer and surface are valid
void RSHardwareThread::LayerPresentTimestamp(const LayerInfoPtr& layer, const sptr<Surface>& surface) const
{
    if (!layer->IsSupportedPresentTimestamp()) {
        return;
    }
    const auto& buffer = layer->GetBuffer();
    if (buffer == nullptr) {
        return;
    }
    if (surface->SetPresentTimestamp(buffer->GetSeqNum(), layer->GetPresentTimestamp()) != GSERROR_OK) {
        RS_LOGD("RsDebug RSUniRenderComposerAdapter::LayerPresentTimestamp: SetPresentTimestamp failed");
    }
}
}
