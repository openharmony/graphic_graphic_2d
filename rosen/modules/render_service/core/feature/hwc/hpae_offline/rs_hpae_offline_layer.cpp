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

#include "feature/hwc/hpae_offline/rs_hpae_offline_layer.h"

#include "common/rs_optional_trace.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_util.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/main_thread/rs_uni_render_listener.h"
#include "platform/common/rs_log.h"
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"

namespace OHOS::Rosen {

RSHpaeOfflineLayer::RSHpaeOfflineLayer(const std::string& name, NodeId layerId)
    : surfaceHandler_(std::make_shared<RSSurfaceHandler>(layerId)), layerName_(name)
{
}

RSHpaeOfflineLayer::~RSHpaeOfflineLayer()
{
}

bool RSHpaeOfflineLayer::PreAllocBuffers(const BufferRequestConfig& config)
{
    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        if (!CreateSurface(listener)) {
            return false;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT("prealloc offline buffer.");
    GSError ret = surface_->PreAllocBuffers(config, bufferSize_);
    if (ret != GSERROR_OK) {
        ret = pSurface_->CleanCache(true);
        RS_OFFLINE_LOGW("offline prealloc buffer failed, clean cache: %{public}d", ret);
        return false;
    }
    RS_OFFLINE_LOGD("offline prealloc buffer success.");
    return true;
}

sptr<SurfaceBuffer> RSHpaeOfflineLayer::RequestSurfaceBuffer(
    BufferRequestConfig& config, int32_t& releaseFence)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: request offline buffer.");
    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        if (!CreateSurface(listener)) {
            return nullptr;
        }
    }
    sptr<SurfaceBuffer> buffer = nullptr;
    pSurface_->RequestBuffer(buffer, releaseFence, config);
    return buffer;
}

// reference to RSDisplayRenderNodeDrawable::CreateSurface
bool RSHpaeOfflineLayer::CreateSurface(sptr<IBufferConsumerListener>& listener)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: Create offline surface");
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && pSurface_ != nullptr) {
        RS_OFFLINE_LOGD("surface already created, return.");
        return true;
    }
    consumer = IConsumerSurface::Create(layerName_);
    if (consumer == nullptr) {
        RS_OFFLINE_LOGE("consumer create fail, get consumer surface fail.");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_OFFLINE_LOGE("register consumerListener fail.");
        return false;
    }
    auto producer = consumer->GetProducer();
    pSurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (!pSurface_) {
        RS_OFFLINE_LOGE("create surface fail, CreateSurfaceAsProducer fail.");
        return false;
    }
    pSurface_->SetQueueSize(bufferSize_);
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);
    return true;
}

void RSHpaeOfflineLayer::FlushSurfaceBuffer(
    sptr<SurfaceBuffer>& buffer, int32_t acquireFence, BufferFlushConfig& flushConfig)
{
    sptr<SyncFence> acquireFenceSp(new SyncFence(acquireFence));
    pSurface_->FlushBuffer(buffer, acquireFenceSp, flushConfig);
}

void RSHpaeOfflineLayer::CleanCache(bool cleanAll)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: clean offline buffer cache.");
    if (pSurface_ != nullptr) {
        GSError ret = pSurface_->CleanCache(cleanAll);
        RS_OFFLINE_LOGD("clean offline buffer cache, ret = %{public}d.", ret);
    }
}
} // OHOS::Rosen