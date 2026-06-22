/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef ROSEN_CROSS_PLATFORM
#include "rs_delegate_composite_buffer_manager.h"
#include "command/rs_delegate_composite_command.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

constexpr size_t MAX_CACHE_SIZE = 16;
static bool g_enableDfx = RSSystemProperties::GetRsDelegateCompositeCleanCacheDfxEnable();
#define DFX_LOGD(enableDfx, format, ...) \
    ((enableDfx) ? (void) HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__) : \
                   (void) HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__))

bool RSDelegateCompositeBufferManager::SetBuffer(sptr<SurfaceBuffer> buffer,
    UniqueFd fence_fd, const BufferReleaseCallback& callback)
{
    if (!buffer || !callback) {
        return false;
    }
    RS_TRACE_NAME_FMT("RSDelegateCompositeBufferManager::SetBuffer begin: nodeId=%llu, bufferSeqNum=%u",
        nodeId_, buffer->GetSeqNum());
    if (buffer_ && release_callback_) {
        release_callback_(std::move(fence_fd_));
    }
    buffer_ = std::move(buffer);
    fence_fd_ = std::move(fence_fd);
    release_callback_ = callback;
    needSyncBufferToNode_ = true;

    DFX_LOGD(g_enableDfx, "RsDelegateCompositeCleanCacheDfx: SetBuffer bufferSeqnum=%{public}u",
        buffer_->GetSeqNum());
    std::vector<RSTransactionConfig> configList;
    auto config = GenerateTransactionConfigIfNecessary(desiredPresentTime_);
    if (config.nodeId != 0) {
        configList.push_back(config);
    }
#ifndef ROSEN_CROSS_PLATFORM
    RS_TRACE_NAME_FMT("RSDelegateCompositeBufferManager::SetBuffer configListSize=%u, config.nodeId=%llu",
        configList.size(), nodeId_);
    if (configList.size() != 0) {
        std::unique_ptr<RSCommand> command = std::make_unique<TransactionBufferCommand>(configList, nodeId_);
        if (rsTransactionHandle_) {
            rsTransactionHandle_->AddCommand(command, true, FollowType::NONE, 0);
        }
    }
#endif
    return true;
}

void RSDelegateCompositeBufferManager::SetDamageRegion(std::vector<Rect> rects)
{
    damage_region_ = std::move(rects);
}

void RSDelegateCompositeBufferManager::SetBufferTransform(GraphicTransformType transform)
{
    if (buffer_transform_ != transform) {
        buffer_transform_ = transform;
        needSyncBufferToNode_ = true;
    }
}

void RSDelegateCompositeBufferManager::GenerateBufferParams(SetBufferParams &params,
    const int64_t &desire_present_time)
{
    if (!buffer_) {
        return;
    }
    buffer_->SetSurfaceBufferTransform(buffer_transform_);
    params.buffer = std::move(buffer_);
    params.fence = new SyncFence(fence_fd_.Release());
    if (damage_region_.empty()) {
        params.damages.push_back(Rect { 0, 0, params.buffer->GetWidth(), params.buffer->GetHeight() });
    } else {
        params.damages = std::move(damage_region_);
        for (auto& rect : params.damages) {
            rect.y = params.buffer->GetHeight() - rect.y - rect.h;
        }
    }
    params.timestamp = desire_present_time;
}

RSTransactionConfig RSDelegateCompositeBufferManager::GenerateTransactionConfigIfNecessary(int64_t desiredPresentTime)
{
    RSTransactionConfig config;
    if (needSyncBufferToNode_) {
        SetBufferParams params;
        GenerateBufferParams(params, desiredPresentTime);
        if (!params.buffer) {
            return config;
        }
        config = GenerateTransactionConfig(std::move(params), [cb = release_callback_](int fence) { cb(fence); });
        release_callback_ = {};
        needSyncBufferToNode_ = false;
    }
    return config;
}

void RSDelegateCompositeBufferManager::CleanBuffer(bool cleanAll)
{
    RS_TRACE_NAME_FMT("RSDelegateCompositeBufferManager::CleanBuffer, node:[name: %s, id: %" PRIu64 "], cleanAll=%d",
        name_.c_str(), nodeId_, cleanAll);
    uint32_t bufferSequence = 0;

    if (psurface_) {
        psurface_->CleanCache(false, bufferSequence);
        {
            std::lock_guard<std::mutex> lock(cacheMutex_);
            RS_TRACE_NAME_FMT("transactionCacheForSetBuffer=%d", transactionCacheForSetBuffer_.size());
            auto iter = transactionCacheForSetBuffer_.find(bufferSequence);
            if (iter != transactionCacheForSetBuffer_.end()) {
                setBufferCountAfterCleanCache_ = 0;
                preCachebuffer_ = iter->second.buffer;
                if (preCachebuffer_) {
                    RS_TRACE_NAME_FMT("Mark PreCacheBuffer: bufId=%" PRIu64 ", bufSeqNum=%u",
                        preCachebuffer_->GetBufferId(), preCachebuffer_->GetSeqNum());
                }
            }
            transactionCacheForSetBuffer_.clear();
        }
    }
}

void RSDelegateCompositeBufferManager::SetDesiredPresentTime(int64_t desiredPresentTime)
{
    desiredPresentTime_ = desiredPresentTime;
}

void RSDelegateCompositeBufferManager::GenerateTransactionConfigInner(SetBufferParams& params,
    RSTransactionConfig& config)
{
    uint32_t seqNum = params.buffer->GetSeqNum();
    config.nodeId = nodeId_;
    config.transaction = new Transaction(params.buffer);
    config.transaction->SetFence(params.fence);
    config.transaction->SetDamages(params.damages);
    config.transaction->SetUiTimestamp(params.timestamp);
    config.transaction->SetDesiredPresentTimestamp(params.desiredPresentTimestamp);
    config.transaction->SetAutoTimestamp(params.isAutoTimestamp);
    config.transaction->SetHdrMetaDataType(params.hdrMetaDataType);
    config.transaction->SetMetaData(params.metaData);
    config.transaction->SetMetaDataKey(params.metaDataKey);
    config.transaction->SetMetaDataSet(params.metaDataSet);
    config.hasBuffer = false;

    {
        std::lock_guard<std::mutex> lock(cacheMutex_);
        uint32_t bufferSequence = params.buffer->GetSeqNum();
        RS_TRACE_NAME_FMT("node:[name: %s, id: %" PRIu64 "], queueSize=%u, bufferSeqNum=%u",
            name_.c_str(), nodeId_, transactionCacheForSetBuffer_.size(), seqNum);
        auto iter = transactionCacheForSetBuffer_.find(bufferSequence);
        if (iter == transactionCacheForSetBuffer_.end()) {
            RS_TRACE_NAME_FMT("not find buffer: bufferSeqNum=%u", seqNum);
            config.hasBuffer = true;
            if (transactionCacheForSetBuffer_.size() >= MAX_CACHE_SIZE) {
                GenerateTransactionConfigDelList(config);
            }
        }
        SetBufferCacheInfo cacheInfo;
        cacheInfo.buffer = params.buffer;
        cacheInfo.state = TransactionState::TRANSACTION_STATE_QUEUED;
        transactionCacheForSetBuffer_[bufferSequence] = cacheInfo;
        if (++setBufferCountAfterCleanCache_ >= 2) { // 2 is cur buffer and pre buffer
            if (preCachebuffer_ != nullptr) {
                RS_TRACE_NAME_FMT("ReleasePreCacheBuffer: bufId=%" PRIu64 ", bufSeqNum=%u",
                    preCachebuffer_->GetBufferId(), preCachebuffer_->GetSeqNum());
                preCachebuffer_ = nullptr;
            }
            setBufferCountAfterCleanCache_ = -1;
        }
    }

    if (RSSystemProperties::GetDebugTraceEnabled()) { // dfx
        std::lock_guard<std::mutex> lock(cacheMutex_);
        RS_TRACE_NAME_FMT("transactionCacheForSetBuffer size=%u, node:[name: %s, id: %" PRIu64 "]",
            transactionCacheForSetBuffer_.size(), name_.c_str(), nodeId_);
        for (auto & [bufferSequence, cacheInfo] : transactionCacheForSetBuffer_) {
            RS_TRACE_NAME_FMT("bufferSequence=%u, state=%u", bufferSequence, cacheInfo.state);
        }
    }
}

void RSDelegateCompositeBufferManager::GenerateTransactionConfigDelList(RSTransactionConfig& config)
{
    for (auto iter = transactionCacheForSetBuffer_.begin(); iter != transactionCacheForSetBuffer_.end();) {
        if (iter->second.state == TransactionState::TRANSACTION_STATE_RELEASED) {
            RS_TRACE_NAME_FMT("seqNum=%u, state=RELEASED", iter->first);
            config.delList.push_back(iter->first);
            iter = transactionCacheForSetBuffer_.erase(iter);
        } else {
        RS_TRACE_NAME_FMT("seqNum=%u, state=QUEUED", iter->first);
        ++iter;
        }
    }
}

RSTransactionConfig RSDelegateCompositeBufferManager::GenerateTransactionConfig(SetBufferParams params,
    BufferReleaseCallback releaseCallback)
{
    uint32_t seqNum = params.buffer->GetSeqNum();
    RSTransactionConfig config;
    GenerateTransactionConfigInner(params, config);

    if (releaseCallback) {
        std::lock_guard<std::mutex> lock(bufferReleaseCallbacksMutex_);
        if (bufferReleaseCallbacks_.find(seqNum) != bufferReleaseCallbacks_.end()) {
            RS_LOGD("bufferReleaseCallbacks_ contains %{public}u", seqNum);
        }
        bufferReleaseCallbacks_[seqNum] = releaseCallback;
    }
    return config;
}

GSError RSDelegateCompositeBufferManager::ReleaseBuffer(uint32_t sequence, sptr<SyncFence> fence)
{
    BufferReleaseCallback releaseCallback;
    {
        std::unique_lock<std::mutex> lock(bufferReleaseCallbacksMutex_);
        auto it = bufferReleaseCallbacks_.find(sequence);
        if (it != bufferReleaseCallbacks_.end()) {
            releaseCallback = it->second;
            bufferReleaseCallbacks_.erase(it);
        }
    }

    if (releaseCallback) {
        RS_TRACE_NAME_FMT("surface Node ReleaseBuffer(releaseCallback): nodeId=%llu, sequence=%u",
            nodeId_, sequence);
        DFX_LOGD(g_enableDfx, "RsDelegateCompositeCleanCacheDfx: releaseCallback buffer bufferSeqnum=%{public}u",
            sequence);
        releaseCallback(fence ? fence->Dup() : -1);
    }

    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto iter = transactionCacheForSetBuffer_.find(sequence);
    if (iter != transactionCacheForSetBuffer_.end()) {
        RS_TRACE_NAME_FMT(
            "surface Node ReleaseBuffer(releaseCallback): nodeId=%llu, sequence=%u, TRANSACTION_STATE_RELEASED",
            nodeId_, sequence);
        iter->second.state = TransactionState::TRANSACTION_STATE_RELEASED;
    }
    return GSERROR_OK;
}

void RSDelegateCompositeBufferManager::SetDelegateDstRect(float positionX, float positionY,
    float positionZ, float positionW)
{
    RectF rect(positionX, positionY, positionZ, positionW);
    std::unique_ptr<RSCommand> command = std::make_unique<TransactionBufferCommand>(rect, false, nodeId_);
    if (rsTransactionHandle_) {
        rsTransactionHandle_->AddCommand(command, true, FollowType::NONE, 0);
    }
}

void RSDelegateCompositeBufferManager::SetDelegateSrcRect(float positionX, float positionY,
    float positionZ, float positionW)
{
    RectF rect(positionX, positionY, positionZ, positionW);
    std::unique_ptr<RSCommand> command = std::make_unique<TransactionBufferCommand>(rect, true, nodeId_);
    if (rsTransactionHandle_) {
        rsTransactionHandle_->AddCommand(command, true, FollowType::NONE, 0);
    }
}

bool RSDelegateCompositeBufferManager::SetDelegateMode(bool isDelegateMode)
{
    if (!rsUIContext_) {
        RS_LOGE("RSDelegateCompositeBufferManager::SetDelegateMode fail: rsUIContext invalid");
        return false;
    }

    auto renderInterface = rsUIContext_->GetRSRenderInterface();
    if (!renderInterface) {
        RS_LOGE("RSDelegateCompositeBufferManager::SetDelegateMode fail: renderInterface invalid");
        return false;
    }
    return renderInterface->SetDelegateMode(nodeId_, isDelegateMode);
}

RSDelegateCompositeBufferManager::RSDelegateCompositeBufferManager(std::shared_ptr<RSUIContext> rsUIContext,
    sptr<Surface> psurface, const NodeId& nodeId, const std::string& name)
{
    rsUIContext_ = rsUIContext;
    psurface_ = psurface;
    nodeId_ = nodeId;
    name_ = name;

    if (rsUIContext_) {
        rsTransactionHandle_ = rsUIContext_->GetRSTransaction();
    }
}

RSDelegateCompositeBufferManager::~RSDelegateCompositeBufferManager()
{
    std::map<uint32_t, BufferReleaseCallback> releaseCallbacks;
    {
        std::lock_guard<std::mutex> lock(bufferReleaseCallbacksMutex_);
        releaseCallbacks = std::move(bufferReleaseCallbacks_);
    }
    for (auto& seqAndCallback : releaseCallbacks) {
        seqAndCallback.second(-1);
    }
    rsTransactionHandle_ = nullptr;
}
} // namespace Rosen
} // namespace OHOS
#endif
