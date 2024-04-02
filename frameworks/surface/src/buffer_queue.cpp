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

#include "buffer_queue.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <cinttypes>
#include <unistd.h>
#include <parameters.h>
#include <scoped_bytrace.h>

#include "buffer_utils.h"
#include "buffer_log.h"
#include "hitrace_meter.h"
#include "sandbox_utils.h"
#include "surface_buffer_impl.h"
#include "sync_fence.h"
#include "sync_fence_tracker.h"
#include "surface_utils.h"
#include "v1_1/buffer_handle_meta_key_type.h"

namespace OHOS {
namespace {
constexpr uint32_t UNIQUE_ID_OFFSET = 32;
constexpr uint32_t BUFFER_MEMSIZE_RATE = 1024;
constexpr uint32_t BUFFER_MEMSIZE_FORMAT = 2;
}

static const std::map<BufferState, std::string> BufferStateStrs = {
    {BUFFER_STATE_RELEASED,                    "0 <released>"},
    {BUFFER_STATE_REQUESTED,                   "1 <requested>"},
    {BUFFER_STATE_FLUSHED,                     "2 <flushed>"},
    {BUFFER_STATE_ACQUIRED,                    "3 <acquired>"},
};

static uint64_t GetUniqueIdImpl()
{
    static std::atomic<uint32_t> counter { 0 };
    static uint64_t id = static_cast<uint64_t>(GetRealPid()) << UNIQUE_ID_OFFSET;
    return id | counter++;
}

static bool IsLocalRender()
{
    return GetRealPid() == gettid();
}

BufferQueue::BufferQueue(const std::string &name, bool isShared)
    : name_(name), uniqueId_(GetUniqueIdImpl()), isShared_(isShared), isLocalRender_(IsLocalRender())
{
    BLOGND("ctor, Queue id: %{public}" PRIu64 " isShared: %{public}d", uniqueId_, isShared);
    if (isShared_ == true) {
        queueSize_ = 1;
    }
}

BufferQueue::~BufferQueue()
{
    BLOGND("dtor, Queue id: %{public}" PRIu64, uniqueId_);
    for (auto &[id, _] : bufferQueueCache_) {
        if (onBufferDeleteForRSMainThread_ != nullptr) {
            onBufferDeleteForRSMainThread_(id);
        }
        if (onBufferDeleteForRSHardwareThread_ != nullptr) {
            onBufferDeleteForRSHardwareThread_(id);
        }
    }
}

GSError BufferQueue::Init()
{
    return GSERROR_OK;
}

uint32_t BufferQueue::GetUsedSize()
{
    uint32_t used_size = bufferQueueCache_.size();
    return used_size;
}

GSError BufferQueue::PopFromFreeList(sptr<SurfaceBuffer> &buffer,
    const BufferRequestConfig &config)
{
    if (isShared_ == true && GetUsedSize() > 0) {
        buffer = bufferQueueCache_.begin()->second.buffer;
        return GSERROR_OK;
    }

    for (auto it = freeList_.begin(); it != freeList_.end(); it++) {
        if (bufferQueueCache_[*it].config == config) {
            buffer = bufferQueueCache_[*it].buffer;
            freeList_.erase(it);
            return GSERROR_OK;
        }
    }

    if (freeList_.empty() || GetUsedSize() < GetQueueSize()) {
        buffer = nullptr;
        return GSERROR_NO_BUFFER;
    }

    buffer = bufferQueueCache_[freeList_.front()].buffer;
    buffer->SetSurfaceBufferColorGamut(config.colorGamut);
    buffer->SetSurfaceBufferTransform(config.transform);
    freeList_.pop_front();
    return GSERROR_OK;
}

GSError BufferQueue::PopFromDirtyList(sptr<SurfaceBuffer> &buffer)
{
    if (isShared_ == true && GetUsedSize() > 0) {
        buffer = bufferQueueCache_.begin()->second.buffer;
        return GSERROR_OK;
    }

    if (!dirtyList_.empty()) {
        buffer = bufferQueueCache_[dirtyList_.front()].buffer;
        dirtyList_.pop_front();
        return GSERROR_OK;
    } else {
        buffer = nullptr;
        return GSERROR_NO_BUFFER;
    }
}

GSError BufferQueue::CheckRequestConfig(const BufferRequestConfig &config)
{
    uint32_t align = config.strideAlignment;
    bool isValidStrideAlignment = true;
    isValidStrideAlignment = isValidStrideAlignment && (SURFACE_MIN_STRIDE_ALIGNMENT <= align);
    isValidStrideAlignment = isValidStrideAlignment && (SURFACE_MAX_STRIDE_ALIGNMENT >= align);
    if (!isValidStrideAlignment) {
        BLOGN_INVALID("config.strideAlignment [%{public}d, %{public}d], now is %{public}d",
                      SURFACE_MIN_STRIDE_ALIGNMENT, SURFACE_MAX_STRIDE_ALIGNMENT, align);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (align & (align - 1)) {
        BLOGN_INVALID("config.strideAlignment is not power of 2 like 4, 8, 16, 32; now is %{public}d", align);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (config.colorGamut <= GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID ||
        config.colorGamut > GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020 + 1) {
        BLOGN_INVALID("config.colorGamut [0, %{public}d], now is %{public}d",
            static_cast<uint32_t>(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020),
            static_cast<uint32_t>(config.colorGamut));
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (config.transform < GraphicTransformType::GRAPHIC_ROTATE_NONE ||
        config.transform >= GraphicTransformType::GRAPHIC_ROTATE_BUTT) {
        BLOGN_INVALID("config.transform [0, %{public}d), now is %{public}d",
            GraphicTransformType::GRAPHIC_ROTATE_BUTT, config.transform);
        return GSERROR_INVALID_ARGUMENTS;
    }
    return GSERROR_OK;
}

GSError BufferQueue::CheckFlushConfig(const BufferFlushConfigWithDamages &config)
{
    for (decltype(config.damages.size()) i = 0; i < config.damages.size(); i++) {
        if (config.damages[i].w < 0 || config.damages[i].h < 0) {
            BLOGN_INVALID("config.damages width and height should >= 0, "
                "now damages[%{public}zu].w is %{public}d, .h is %{public}d, ",
                i, config.damages[i].w, config.damages[i].h);
            return GSERROR_INVALID_ARGUMENTS;
        }
    }
    return GSERROR_OK;
}

bool BufferQueue::QueryIfBufferAvailable()
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    bool ret = !freeList_.empty() || (GetUsedSize() < GetQueueSize());
    return ret;
}

static GSError DelegatorDequeueBuffer(wptr<ConsumerSurfaceDelegator>& delegator,
                                      const BufferRequestConfig& config,
                                      sptr<BufferExtraData>& bedata,
                                      struct IBufferProducer::RequestBufferReturnValue& retval)
{
    auto consumerDelegator = delegator.promote();
    if (consumerDelegator == nullptr) {
        BLOGE("Consumer surface delegator has been expired");
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto ret = consumerDelegator->DequeueBuffer(config, bedata, retval);
    if (ret != GSERROR_OK) {
        BLOGE("Consumer surface delegator failed to dequeuebuffer, err: %{public}d", ret);
        return ret;
    }

    ret = retval.buffer->Map();
    if (ret != GSERROR_OK) {
        BLOGE("Buffer map failed, err: %{public}d", ret);
        return ret;
    }
    retval.buffer->SetSurfaceBufferWidth(retval.buffer->GetWidth());
    retval.buffer->SetSurfaceBufferHeight(retval.buffer->GetHeight());

    return GSERROR_OK;
}

static void SetReturnValue(sptr<SurfaceBuffer>& buffer, sptr<BufferExtraData>& bedata,
                           struct IBufferProducer::RequestBufferReturnValue& retval)
{
    retval.sequence = buffer->GetSeqNum();
    bedata = buffer->GetExtraData();
    retval.fence = SyncFence::INVALID_FENCE;
}

void BufferQueue::SetSurfaceBufferHebcMetaLocked(sptr<SurfaceBuffer> buffer)
{
    using namespace HDI::Display::Graphic::Common;
    // usage does not contain BUFFER_USAGE_CPU_HW_BOTH, just return
    if (!(buffer->GetUsage() & BUFFER_USAGE_CPU_HW_BOTH)) {
        return;
    }

    V1_1::BufferHandleAttrKey key = V1_1::BufferHandleAttrKey::ATTRKEY_REQUEST_ACCESS_TYPE;
    std::vector<uint8_t> values;
    if (isCpuAccessable_) { // hebc is off
        values.push_back(static_cast<uint8_t>(V1_1::HebcAccessType::HEBC_ACCESS_CPU_ACCESS));
    } else { // hebc is on
        values.push_back(static_cast<uint8_t>(V1_1::HebcAccessType::HEBC_ACCESS_HW_ONLY));
    }

    buffer->SetMetadata(key, values);
}

GSError BufferQueue::RequestBufferCheckStatus()
{
    if (!GetStatus()) {
        BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
    }
    std::lock_guard<std::mutex> lockGuard(listenerMutex_);
    if (listener_ == nullptr && listenerClazz_ == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
    }

    return GSERROR_OK;
}

GSError BufferQueue::RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
    struct IBufferProducer::RequestBufferReturnValue &retval)
{
    if (wpCSurfaceDelegator_ != nullptr) {
        return DelegatorDequeueBuffer(wpCSurfaceDelegator_, config, bedata, retval);
    }

    GSError ret = GSERROR_OK;
    ret = RequestBufferCheckStatus();
    if (ret != GSERROR_OK) {
        return ret;
    }

    ScopedBytrace func(__func__);
    // check param
    ret = CheckRequestConfig(config);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE_API(CheckRequestConfig, ret);
        return ret;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    // dequeue from free list
    sptr<SurfaceBuffer>& buffer = retval.buffer;
    ret = PopFromFreeList(buffer, config);
    if (ret == GSERROR_OK) {
        return ReuseBuffer(config, bedata, retval);
    }

    // check queue size
    if (GetUsedSize() >= GetQueueSize()) {
        waitReqCon_.wait_for(lock, std::chrono::milliseconds(config.timeout),
            [this]() { return !freeList_.empty() || (GetUsedSize() < GetQueueSize()) || !GetStatus(); });
        if (!GetStatus()) {
            BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
        }
        // try dequeue from free list again
        ret = PopFromFreeList(buffer, config);
        if (ret == GSERROR_OK) {
            return ReuseBuffer(config, bedata, retval);
        } else if (GetUsedSize() >= GetQueueSize()) {
            BLOGND("all buffer are using, Queue id: %{public}" PRIu64, uniqueId_);
            return GSERROR_NO_BUFFER;
        }
    }

    ret = AllocBuffer(buffer, config);
    if (ret == GSERROR_OK) {
        SetSurfaceBufferHebcMetaLocked(buffer);
        SetReturnValue(buffer, bedata, retval);
        BLOGND("Success alloc Buffer[%{public}d %{public}d] id: %{public}d id: %{public}" PRIu64, config.width,
            config.height, retval.sequence, uniqueId_);
    } else {
        BLOGNE("Fail to alloc or map Buffer[%{public}d %{public}d] ret: %{public}d, id: %{public}" PRIu64,
            config.width, config.height, ret, uniqueId_);
    }

    return ret;
}

GSError BufferQueue::SetProducerCacheCleanFlag(bool flag)
{
    std::unique_lock<std::mutex> lock(mutex_);
    return SetProducerCacheCleanFlagLocked(flag);
}

GSError BufferQueue::SetProducerCacheCleanFlagLocked(bool flag)
{
    producerCacheClean_ = flag;
    producerCacheList_.clear();
    return GSERROR_OK;
}

bool BufferQueue::CheckProducerCacheList()
{
    for (auto &[id, _] : bufferQueueCache_) {
        if (std::find(producerCacheList_.begin(), producerCacheList_.end(), id) == producerCacheList_.end()) {
            return false;
        }
    }
    return true;
}

GSError BufferQueue::ReallocBuffer(const BufferRequestConfig &config,
    struct IBufferProducer::RequestBufferReturnValue &retval)
{
    if (isShared_) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }
    DeleteBufferInCache(retval.sequence);

    sptr<SurfaceBuffer> buffer = nullptr;
    auto sret = AllocBuffer(buffer, config);
    if (sret != GSERROR_OK) {
        BLOGN_FAILURE("realloc failed");
        return sret;
    }

    retval.buffer = buffer;
    retval.sequence = buffer->GetSeqNum();
    bufferQueueCache_[retval.sequence].config = config;
    return GSERROR_OK;
}

GSError BufferQueue::ReuseBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
    struct IBufferProducer::RequestBufferReturnValue &retval)
{
    ScopedBytrace func(__func__);
    if (retval.buffer == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }
    retval.sequence = retval.buffer->GetSeqNum();
    if (bufferQueueCache_.find(retval.sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }
    bool needRealloc = (config != bufferQueueCache_[retval.sequence].config);
    // config, realloc
    if (needRealloc) {
        auto sret = ReallocBuffer(config, retval);
        if (sret != GSERROR_OK) {
            return sret;
        }
    }

    bufferQueueCache_[retval.sequence].state = BUFFER_STATE_REQUESTED;
    retval.fence = bufferQueueCache_[retval.sequence].fence;
    bedata = retval.buffer->GetExtraData();
    SetSurfaceBufferHebcMetaLocked(retval.buffer);

    auto &dbs = retval.deletingBuffers;
    dbs.insert(dbs.end(), deletingList_.begin(), deletingList_.end());
    deletingList_.clear();

    if (needRealloc || isShared_ || producerCacheClean_ || retval.buffer->GetConsumerAttachBufferFlag()) {
        BLOGND("RequestBuffer Succ realloc Buffer[%{public}d %{public}d] with new config "\
            "qid: %{public}d attachFlag: %{public}d id: %{public}" PRIu64,
            config.width, config.height, retval.sequence, retval.buffer->GetConsumerAttachBufferFlag(), uniqueId_);
        if (producerCacheClean_) {
            producerCacheList_.push_back(retval.sequence);
            if (CheckProducerCacheList()) {
                SetProducerCacheCleanFlagLocked(false);
            }
        }
        retval.buffer->SetConsumerAttachBufferFlag(false);
    } else {
        BLOGND("RequestBuffer Succ Buffer[%{public}d %{public}d] in seq id: %{public}d "\
            "qid: %{public}" PRIu64 " releaseFence: %{public}d",
            config.width, config.height, retval.sequence, uniqueId_, retval.fence->Get());
        retval.buffer = nullptr;
    }

    ScopedBytrace bufferName(name_ + ":" + std::to_string(retval.sequence));
    if (IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP) && isLocalRender_) {
        static SyncFenceTracker releaseFenceThread("Release Fence");
        releaseFenceThread.TrackFence(retval.fence);
    }
    return GSERROR_OK;
}

GSError BufferQueue::CancelBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata)
{
    ScopedBytrace func(__func__);
    if (isShared_) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_OPERATING);
    }
    std::lock_guard<std::mutex> lockGuard(mutex_);

    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }

    if (bufferQueueCache_[sequence].state != BUFFER_STATE_REQUESTED &&
        bufferQueueCache_[sequence].state != BUFFER_STATE_ATTACHED) {
        return GSERROR_INVALID_OPERATING;
    }
    bufferQueueCache_[sequence].state = BUFFER_STATE_RELEASED;
    freeList_.push_back(sequence);
    bufferQueueCache_[sequence].buffer->SetExtraData(bedata);

    waitReqCon_.notify_all();
    waitAttachCon_.notify_all();
    BLOGND("Success Buffer id: %{public}d Queue id: %{public}" PRIu64, sequence, uniqueId_);

    return GSERROR_OK;
}

GSError BufferQueue::CheckBufferQueueCache(uint32_t sequence)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }

    if (isShared_ == false) {
        auto &state = bufferQueueCache_[sequence].state;
        if (state != BUFFER_STATE_REQUESTED && state != BUFFER_STATE_ATTACHED) {
            BLOGN_FAILURE_ID(sequence, "invalid state %{public}d", state);
            return GSERROR_NO_ENTRY;
        }
    }
    return GSERROR_OK;
}

GSError BufferQueue::FlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
    const sptr<SyncFence>& fence, const BufferFlushConfigWithDamages &config)
{
    ScopedBytrace func(__func__);
    if (!GetStatus()) {
        BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
    }
    // check param
    auto sret = CheckFlushConfig(config);
    if (sret != GSERROR_OK) {
        BLOGN_FAILURE_API(CheckFlushConfig, sret);
        return sret;
    }

    sret = CheckBufferQueueCache(sequence);
    if (sret != GSERROR_OK) {
        return sret;
    }

    {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ == nullptr && listenerClazz_ == nullptr) {
            CancelBuffer(sequence, bedata);
            return GSERROR_NO_CONSUMER;
        }
    }

    sret = DoFlushBuffer(sequence, bedata, fence, config);
    if (sret != GSERROR_OK) {
        return sret;
    }
    CountTrace(HITRACE_TAG_GRAPHIC_AGP, name_, static_cast<int32_t>(dirtyList_.size()));
    if (sret == GSERROR_OK) {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ != nullptr) {
            listener_->OnBufferAvailable();
        } else if (listenerClazz_ != nullptr) {
            listenerClazz_->OnBufferAvailable();
        }
    }
    BLOGND("Success Buffer seq id: %{public}d Queue id: %{public}" PRIu64 " AcquireFence:%{public}d",
        sequence, uniqueId_, fence->Get());

    if (wpCSurfaceDelegator_ != nullptr) {
        auto consumerDelegator = wpCSurfaceDelegator_.promote();
        if (consumerDelegator == nullptr) {
            BLOGE("Consumer surface delegator has been expired");
            return GSERROR_INVALID_ARGUMENTS;
        }
        sret = consumerDelegator->QueueBuffer(bufferQueueCache_[sequence].buffer, fence->Get());
        if (sret != GSERROR_OK) {
            BLOGNE("Consumer surface delegator failed to dequeuebuffer");
        }
    }
    return sret;
}

GSError BufferQueue::GetLastFlushedBuffer(sptr<SurfaceBuffer>& buffer,
    sptr<SyncFence>& fence, float matrix[16])
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(lastFlusedSequence_) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    auto &state = bufferQueueCache_[lastFlusedSequence_].state;
    if (state == BUFFER_STATE_REQUESTED) {
        BLOGN_FAILURE_ID(lastFlusedSequence_, "invalid state %{public}d", state);
        return GSERROR_NO_ENTRY;
    }
    if (bufferQueueCache_[lastFlusedSequence_].buffer->GetUsage() & BUFFER_USAGE_PROTECTED) {
        BLOGE("Not allowed to obtain protect surface buffer");
        return OHOS::GSERROR_NO_PERMISSION;
    }
    buffer = bufferQueueCache_[lastFlusedSequence_].buffer;
    fence = lastFlusedFence_;
    Rect damage = {};
    if (buffer != nullptr) {
        damage.w = buffer->GetWidth();
        damage.h = buffer->GetHeight();
    }
    auto utils = SurfaceUtils::GetInstance();
    utils->ComputeTransformMatrix(matrix, buffer, lastFlushedTransform_, damage);
    return GSERROR_OK;
}

void BufferQueue::DumpToFile(uint32_t sequence)
{
    static bool dumpBufferEnabled = system::GetParameter("persist.dumpbuffer.enabled", "0") != "0";
    if (!dumpBufferEnabled || access("/data/bq_dump", F_OK) == -1) {
        return;
    }

    sptr<SurfaceBuffer>& buffer = bufferQueueCache_[sequence].buffer;
    if (buffer == nullptr) {
        return;
    }

    ScopedBytrace func(__func__);
    struct timeval now;
    gettimeofday(&now, nullptr);
    constexpr int secToUsec = 1000 * 1000;
    int64_t nowVal = (int64_t)now.tv_sec * secToUsec + (int64_t)now.tv_usec;

    std::stringstream ss;
    ss << "/data/bq_" << GetRealPid() << "_" << name_ << "_" << nowVal << "_" << buffer->GetWidth()
        << "x" << buffer->GetHeight() << ".raw";
    std::ofstream rawDataFile(ss.str(), std::ofstream::binary);
    if (!rawDataFile.good()) {
        BLOGE("open failed: (%{public}d)%{public}s", errno, strerror(errno));
        return;
    }
    rawDataFile.write(static_cast<const char *>(buffer->GetVirAddr()), buffer->GetSize());
    rawDataFile.close();
}

GSError BufferQueue::DoFlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
    const sptr<SyncFence>& fence, const BufferFlushConfigWithDamages &config)
{
    ScopedBytrace bufferName(name_ + ":" + std::to_string(sequence));
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    if (bufferQueueCache_[sequence].isDeleting) {
        DeleteBufferInCache(sequence);
        BLOGN_SUCCESS_ID(sequence, "delete");
        return GSERROR_OK;
    }

    bufferQueueCache_[sequence].state = BUFFER_STATE_FLUSHED;
    dirtyList_.push_back(sequence);
    bufferQueueCache_[sequence].buffer->SetExtraData(bedata);
    bufferQueueCache_[sequence].fence = fence;
    bufferQueueCache_[sequence].damages = config.damages;
    lastFlusedSequence_ = sequence;
    lastFlusedFence_ = fence;
    lastFlushedTransform_ = transform_;

    uint64_t usage = static_cast<uint32_t>(bufferQueueCache_[sequence].config.usage);
    if (usage & BUFFER_USAGE_CPU_WRITE) {
        // api flush
        auto sret = bufferQueueCache_[sequence].buffer->FlushCache();
        if (sret != GSERROR_OK) {
            BLOGN_FAILURE_ID_API(sequence, FlushCache, sret);
            return sret;
        }
    }

    bufferQueueCache_[sequence].timestamp = config.timestamp;

    if (IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP) && isLocalRender_) {
        static SyncFenceTracker acquireFenceThread("Acquire Fence");
        acquireFenceThread.TrackFence(fence);
    }
    // if you need dump SurfaceBuffer to file, you should execute hdc shell param set persist.dumpbuffer.enabled 1
    // and reboot your device
    DumpToFile(sequence);
    return GSERROR_OK;
}

GSError BufferQueue::AcquireBuffer(sptr<SurfaceBuffer> &buffer,
    sptr<SyncFence> &fence, int64_t &timestamp, std::vector<Rect> &damages)
{
    ScopedBytrace func(__func__);
    // dequeue from dirty list
    std::lock_guard<std::mutex> lockGuard(mutex_);
    GSError ret = PopFromDirtyList(buffer);
    if (ret == GSERROR_OK) {
        uint32_t sequence = buffer->GetSeqNum();
        bufferQueueCache_[sequence].state = BUFFER_STATE_ACQUIRED;

        fence = bufferQueueCache_[sequence].fence;
        timestamp = bufferQueueCache_[sequence].timestamp;
        damages = bufferQueueCache_[sequence].damages;
        ScopedBytrace bufferName(name_ + ":" + std::to_string(sequence));
        BLOGND("Success Buffer seq id: %{public}d Queue id: %{public}" PRIu64 " AcquireFence:%{public}d",
            sequence, uniqueId_, fence->Get());
    } else if (ret == GSERROR_NO_BUFFER) {
        BLOGND("there is no dirty buffer");
    }

    CountTrace(HITRACE_TAG_GRAPHIC_AGP, name_, static_cast<int32_t>(dirtyList_.size()));
    return ret;
}

void BufferQueue::ListenerBufferReleasedCb(sptr<SurfaceBuffer> &buffer, const sptr<SyncFence> &fence)
{
    {
        std::lock_guard<std::mutex> lockGuard(onBufferReleaseMutex_);
        if (onBufferRelease_ != nullptr) {
            ScopedBytrace func("OnBufferRelease_");
            sptr<SurfaceBuffer> buf = buffer;
            auto sret = onBufferRelease_(buf);
            if (sret == GSERROR_OK) {   // need to check why directly return?
                // We think that onBufferRelase is not used by anyone, so delete 'return sret' temporarily;
            }
        }
    }

    sptr<IProducerListener> listener;
    {
        std::lock_guard<std::mutex> lockGuard(producerListenerMutex_);
        listener = producerListener_;
    }

    if (listener != nullptr) {
        ScopedBytrace func("onBufferReleasedForProducer");
        if (listener->OnBufferReleased() != GSERROR_OK) {
            BLOGN_FAILURE_ID(buffer->GetSeqNum(), "OnBufferReleased failed, Queue id: %{public}" PRIu64 "", uniqueId_);
        }
        if (listener->OnBufferReleasedWithFence(buffer, fence) != GSERROR_OK) {
            BLOGN_FAILURE_ID(buffer->GetSeqNum(), "OnBufferReleasedWithFence failed, Queue id: %{public}" PRIu64 "",
                             uniqueId_);
        }
    }
}

GSError BufferQueue::ReleaseBuffer(sptr<SurfaceBuffer> &buffer, const sptr<SyncFence>& fence)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    uint32_t sequence = buffer->GetSeqNum();
    ScopedBytrace bufferName(std::string(__func__) + "," + name_ + ":" + std::to_string(sequence));
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
            return GSERROR_NO_ENTRY;
        }

        if (isShared_ == false) {
            const auto &state = bufferQueueCache_[sequence].state;
            if (state != BUFFER_STATE_ACQUIRED && state != BUFFER_STATE_ATTACHED) {
                BLOGND("invalid state");
                return GSERROR_NO_ENTRY;
            }
        }

        bufferQueueCache_[sequence].state = BUFFER_STATE_RELEASED;
        bufferQueueCache_[sequence].fence = fence;

        if (bufferQueueCache_[sequence].isDeleting) {
            DeleteBufferInCache(sequence);
            BLOGND("Succ delete Buffer seq id: %{public}d Queue id: %{public}" PRIu64 " in cache", sequence, uniqueId_);
        } else {
            freeList_.push_back(sequence);
            BLOGND("Succ push Buffer seq id: %{public}d Qid: %{public}" PRIu64 " to free list,"
                " releaseFence: %{public}d", sequence, uniqueId_, fence->Get());
        }
        waitReqCon_.notify_all();
        waitAttachCon_.notify_all();
    }
    ListenerBufferReleasedCb(buffer, fence);

    return GSERROR_OK;
}

GSError BufferQueue::AllocBuffer(sptr<SurfaceBuffer> &buffer,
    const BufferRequestConfig &config)
{
    ScopedBytrace func(__func__);
    sptr<SurfaceBuffer> bufferImpl = new SurfaceBufferImpl();
    uint32_t sequence = bufferImpl->GetSeqNum();

    BufferRequestConfig updateConfig = config;
    updateConfig.usage |= defaultUsage;

    GSError ret = bufferImpl->Alloc(updateConfig);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE_ID_API(sequence, Alloc, ret);
        return ret;
    }

    BufferElement ele = {
        .buffer = bufferImpl,
        .state = BUFFER_STATE_REQUESTED,
        .isDeleting = false,
        .config = config,
        .fence = SyncFence::INVALID_FENCE,
    };

    if (config.usage & BUFFER_USAGE_PROTECTED) {
        BLOGD("handle usage is BUFFER_USAGE_PROTECTED, do not Map/UnMap");
        bufferQueueCache_[sequence] = ele;
        buffer = bufferImpl;
        return ret;
    }

    ret = bufferImpl->Map();
    if (ret == GSERROR_OK) {
        BLOGND("Success seq:%{public}d Map", sequence);
        bufferQueueCache_[sequence] = ele;
        buffer = bufferImpl;
    } else {
        BLOGN_FAILURE_ID(sequence, "Map failed");
    }
    return ret;
}

void BufferQueue::DeleteBufferInCache(uint32_t sequence)
{
    auto it = bufferQueueCache_.find(sequence);
    if (it != bufferQueueCache_.end()) {
        if (onBufferDeleteForRSMainThread_ != nullptr) {
            onBufferDeleteForRSMainThread_(sequence);
        }
        if (onBufferDeleteForRSHardwareThread_ != nullptr) {
            onBufferDeleteForRSHardwareThread_(sequence);
        }
        bufferQueueCache_.erase(it);
        deletingList_.push_back(sequence);
    }
}

uint32_t BufferQueue::GetQueueSize()
{
    return queueSize_;
}

void BufferQueue::DeleteBuffersLocked(int32_t count)
{
    ScopedBytrace func(__func__);
    if (count <= 0) {
        return;
    }

    while (!freeList_.empty()) {
        DeleteBufferInCache(freeList_.front());
        freeList_.pop_front();
        count--;
        if (count <= 0) {
            return;
        }
    }

    while (!dirtyList_.empty()) {
        DeleteBufferInCache(dirtyList_.front());
        dirtyList_.pop_front();
        count--;
        if (count <= 0) {
            return;
        }
    }

    for (auto&& ele : bufferQueueCache_) {
        ele.second.isDeleting = true;
        // we don't have to do anything
        count--;
        if (count <= 0) {
            break;
        }
    }
}

GSError BufferQueue::AttachBufferUpdateStatus(std::unique_lock<std::mutex> &lock, uint32_t sequence, int32_t timeOut)
{
    BufferState state = bufferQueueCache_[sequence].state;
    if (state == BUFFER_STATE_RELEASED) {
        bufferQueueCache_[sequence].state = BUFFER_STATE_ATTACHED;
    } else {
        waitAttachCon_.wait_for(lock, std::chrono::milliseconds(timeOut),
            [this, sequence]() { return (bufferQueueCache_[sequence].state == BUFFER_STATE_RELEASED); });
        if (bufferQueueCache_[sequence].state == BUFFER_STATE_RELEASED) {
            bufferQueueCache_[sequence].state = BUFFER_STATE_ATTACHED;
        } else {
            BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
        }
    }

    for (auto iter = freeList_.begin(); iter != freeList_.end(); iter++) {
        if (sequence == *iter) {
            freeList_.erase(iter);
            break;
        }
    }
    return GSERROR_OK;
}

void BufferQueue::AttachBufferUpdateBufferInfo(sptr<SurfaceBuffer>& buffer)
{
    buffer->Map();
    buffer->SetSurfaceBufferWidth(buffer->GetWidth());
    buffer->SetSurfaceBufferHeight(buffer->GetHeight());
}

GSError BufferQueue::AttachBufferToQueue(sptr<SurfaceBuffer> &buffer, InvokerType invokerType)
{
    ScopedBytrace func(__func__);
    if (buffer == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        uint32_t sequence = buffer->GetSeqNum();
        if (bufferQueueCache_.find(sequence) != bufferQueueCache_.end()) {
            BLOGN_FAILURE_ID(sequence, "buffer is already in cache");
            return GSERROR_API_FAILED;
        }
        BufferElement ele;
        ele = {
            .buffer = buffer,
            .isDeleting = false,
            .config = *(buffer->GetBufferRequestConfig()),
            .fence = SyncFence::INVALID_FENCE,
        };
        if (invokerType == InvokerType::PRODUCER_INVOKER) {
            ele.state = BUFFER_STATE_REQUESTED;
        } else {
            ele.state = BUFFER_STATE_ACQUIRED;
        }
        AttachBufferUpdateBufferInfo(buffer);
        bufferQueueCache_[sequence] = ele;
        queueSize_++;
    }
    return GSERROR_OK;
}

GSError BufferQueue::DetachBufferFromQueue(sptr<SurfaceBuffer> &buffer, InvokerType invokerType)
{
    ScopedBytrace func(__func__);
    if (buffer == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        uint32_t sequence = buffer->GetSeqNum();
        if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
            BLOGN_FAILURE_ID(sequence, "not find in cache");
            return GSERROR_NO_ENTRY;
        }
        if (invokerType == InvokerType::PRODUCER_INVOKER) {
            if (bufferQueueCache_[sequence].state != BUFFER_STATE_REQUESTED) {
                BLOGN_FAILURE_ID(sequence, "producer state is not requested");
                return GSERROR_INVALID_OPERATING;
            }
        } else {
            if (bufferQueueCache_[sequence].state != BUFFER_STATE_ACQUIRED) {
                BLOGN_FAILURE_ID(sequence, "consumer state is not acquired");
                return GSERROR_INVALID_OPERATING;
            }
        }
        if (queueSize_ > 0) {
            queueSize_--;
            bufferQueueCache_.erase(sequence);
        } else {
            BLOGN_FAILURE_ID(sequence, "there has no buffer");
            return GSERROR_INVALID_OPERATING;
        }
    }
    return GSERROR_OK;
}

GSError BufferQueue::AttachBuffer(sptr<SurfaceBuffer> &buffer, int32_t timeOut)
{
    ScopedBytrace func(__func__);
    {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (!GetStatus() || (listener_ == nullptr && listenerClazz_ == nullptr)) {
            BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
        }
    }

    if (isShared_ || buffer == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_OPERATING);
    }

    uint32_t sequence = buffer->GetSeqNum();
    std::unique_lock<std::mutex> lock(mutex_);
    if (bufferQueueCache_.find(sequence) != bufferQueueCache_.end()) {
        return AttachBufferUpdateStatus(lock, sequence, timeOut);
    }

    BufferElement ele = {
        .buffer = buffer,
        .state = BUFFER_STATE_ATTACHED,
        .config = {
            .width = buffer->GetWidth(),
            .height = buffer->GetHeight(),
            .strideAlignment = 0x8,
            .format = buffer->GetFormat(),
            .usage = buffer->GetUsage(),
            .timeout = timeOut,
        },
        .damages = { { .w = buffer->GetWidth(), .h = buffer->GetHeight(), } },
    };
    AttachBufferUpdateBufferInfo(buffer);
    int32_t usedSize = static_cast<int32_t>(GetUsedSize());
    int32_t queueSize = static_cast<int32_t>(GetQueueSize());
    if (usedSize >= queueSize) {
        int32_t freeSize = static_cast<int32_t>(dirtyList_.size() + freeList_.size());
        if (freeSize >= usedSize - queueSize + 1) {
            DeleteBuffersLocked(usedSize - queueSize + 1);
            bufferQueueCache_[sequence] = ele;
            BLOGN_SUCCESS_ID(sequence, "release");
            return GSERROR_OK;
        } else {
            BLOGN_FAILURE_RET(GSERROR_OUT_OF_RANGE);
        }
    } else {
        bufferQueueCache_[sequence] = ele;
        BLOGN_SUCCESS_ID(sequence, "no release");
        return GSERROR_OK;
    }
}

GSError BufferQueue::DetachBuffer(sptr<SurfaceBuffer> &buffer)
{
    ScopedBytrace func(__func__);
    if (isShared_) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_OPERATING);
    }

    if (buffer == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    uint32_t sequence = buffer->GetSeqNum();
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }

    if (bufferQueueCache_[sequence].state == BUFFER_STATE_REQUESTED) {
        BLOGN_SUCCESS_ID(sequence, "requested");
    } else if (bufferQueueCache_[sequence].state == BUFFER_STATE_ACQUIRED) {
        BLOGN_SUCCESS_ID(sequence, "acquired");
    } else {
        BLOGN_FAILURE_ID_RET(sequence, GSERROR_NO_ENTRY);
    }
    if (onBufferDeleteForRSMainThread_ != nullptr) {
        onBufferDeleteForRSMainThread_(sequence);
    }
    if (onBufferDeleteForRSHardwareThread_ != nullptr) {
        onBufferDeleteForRSHardwareThread_(sequence);
    }
    bufferQueueCache_.erase(sequence);
    return GSERROR_OK;
}

GSError BufferQueue::RegisterSurfaceDelegator(sptr<IRemoteObject> client, sptr<Surface> cSurface)
{
    sptr<ConsumerSurfaceDelegator> surfaceDelegator = ConsumerSurfaceDelegator::Create();
    if (surfaceDelegator == nullptr) {
        BLOGE("Failed to register consumer delegator because the surface delegator is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (!surfaceDelegator->SetClient(client)) {
        BLOGE("Failed to set client");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (!surfaceDelegator->SetBufferQueue(this)) {
        BLOGE("Failed to set bufferqueue");
        return GSERROR_INVALID_ARGUMENTS;
    }

    surfaceDelegator->SetSurface(cSurface);
    wpCSurfaceDelegator_ = surfaceDelegator;
    return GSERROR_OK;
}

GSError BufferQueue::SetQueueSize(uint32_t queueSize)
{
    if (isShared_ == true && queueSize != 1) {
        BLOGN_INVALID("shared queue, size must be 1");
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (queueSize <= 0) {
        BLOGN_INVALID("queue size (%{public}d) <= 0", queueSize);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (queueSize > SURFACE_MAX_QUEUE_SIZE) {
        BLOGN_INVALID("invalid queueSize[%{public}d] > SURFACE_MAX_QUEUE_SIZE[%{public}d]",
            queueSize, SURFACE_MAX_QUEUE_SIZE);
        return GSERROR_INVALID_ARGUMENTS;
    }
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        DeleteBuffersLocked(queueSize_ - queueSize);
    }
    // if increase the queue size, try to wakeup the blocked thread
    if (queueSize > queueSize_) {
        queueSize_ = queueSize;
        waitReqCon_.notify_all();
    } else {
        queueSize_ = queueSize;
    }

    BLOGND("queue size: %{public}d, Queue id: %{public}" PRIu64, queueSize_, uniqueId_);
    return GSERROR_OK;
}

GSError BufferQueue::GetName(std::string &name)
{
    name = name_;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterConsumerListener(sptr<IBufferConsumerListener> &listener)
{
    std::lock_guard<std::mutex> lockGuard(listenerMutex_);
    listener_ = listener;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterConsumerListener(IBufferConsumerListenerClazz *listener)
{
    std::lock_guard<std::mutex> lockGuard(listenerMutex_);
    listenerClazz_ = listener;
    return GSERROR_OK;
}

GSError BufferQueue::UnregisterConsumerListener()
{
    std::lock_guard<std::mutex> lockGuard(listenerMutex_);
    listener_ = nullptr;
    listenerClazz_ = nullptr;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterReleaseListener(OnReleaseFunc func)
{
    std::lock_guard<std::mutex> lockGuard(onBufferReleaseMutex_);
    onBufferRelease_ = func;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterProducerReleaseListener(sptr<IProducerListener> listener)
{
    std::lock_guard<std::mutex> lockGuard(producerListenerMutex_);
    producerListener_ = listener;
    return GSERROR_OK;
}

GSError BufferQueue::UnRegisterProducerReleaseListener()
{
    std::lock_guard<std::mutex> lockGuard(producerListenerMutex_);
    producerListener_ = nullptr;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterDeleteBufferListener(OnDeleteBufferFunc func, bool isForUniRedraw)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (isForUniRedraw) {
        if (onBufferDeleteForRSHardwareThread_ != nullptr) {
            return GSERROR_OK;
        }
        onBufferDeleteForRSHardwareThread_ = func;
    } else {
        if (onBufferDeleteForRSMainThread_ != nullptr) {
            return GSERROR_OK;
        }
        onBufferDeleteForRSMainThread_ = func;
    }
    return GSERROR_OK;
}

GSError BufferQueue::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    if (width <= 0) {
        BLOGN_INVALID("defaultWidth is greater than 0, now is %{public}d", width);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (height <= 0) {
        BLOGN_INVALID("defaultHeight is greater than 0, now is %{public}d", height);
        return GSERROR_INVALID_ARGUMENTS;
    }

    defaultWidth = width;
    defaultHeight = height;
    return GSERROR_OK;
}

int32_t BufferQueue::GetDefaultWidth()
{
    return defaultWidth;
}

int32_t BufferQueue::GetDefaultHeight()
{
    return defaultHeight;
}

GSError BufferQueue::SetDefaultUsage(uint64_t usage)
{
    defaultUsage = usage;
    return GSERROR_OK;
}

uint64_t BufferQueue::GetDefaultUsage()
{
    return defaultUsage;
}

void BufferQueue::ClearLocked()
{
    for (auto &[id, _] : bufferQueueCache_) {
        if (onBufferDeleteForRSMainThread_ != nullptr) {
            onBufferDeleteForRSMainThread_(id);
        }
        if (onBufferDeleteForRSHardwareThread_ != nullptr) {
            onBufferDeleteForRSHardwareThread_(id);
        }
    }
    bufferQueueCache_.clear();
    freeList_.clear();
    dirtyList_.clear();
    deletingList_.clear();
}

GSError BufferQueue::GoBackground()
{
    BLOGND("GoBackground, Queue id: %{public}" PRIu64, uniqueId_);
    {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnGoBackground");
            listener_->OnGoBackground();
        } else if (listenerClazz_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnGoBackground");
            listenerClazz_->OnGoBackground();
        }
    }
    std::lock_guard<std::mutex> lockGuard(mutex_);
    ClearLocked();
    waitReqCon_.notify_all();
    SetProducerCacheCleanFlagLocked(false);
    return GSERROR_OK;
}

GSError BufferQueue::CleanCache()
{
    {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnCleanCache");
            listener_->OnCleanCache();
        } else if (listenerClazz_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnCleanCache");
            listenerClazz_->OnCleanCache();
        }
    }
    std::lock_guard<std::mutex> lockGuard(mutex_);
    ClearLocked();
    waitReqCon_.notify_all();
    return GSERROR_OK;
}

GSError BufferQueue::OnConsumerDied()
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    ClearLocked();
    waitReqCon_.notify_all();
    return GSERROR_OK;
}

uint64_t BufferQueue::GetUniqueId() const
{
    return uniqueId_;
}

GSError BufferQueue::SetTransform(GraphicTransformType transform)
{
    transform_ = transform;
    return GSERROR_OK;
}

GraphicTransformType BufferQueue::GetTransform() const
{
    return transform_;
}

GSError BufferQueue::SetTransformHint(GraphicTransformType transformHint)
{
    transformHint_ = transformHint;
    return GSERROR_OK;
}

GraphicTransformType BufferQueue::GetTransformHint() const
{
    return transformHint_;
}

GSError BufferQueue::IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
                                      std::vector<bool> &supporteds) const
{
    supporteds.clear();
    for (uint32_t index = 0; index < infos.size(); index++) {
        if (infos[index].format == GRAPHIC_PIXEL_FMT_RGBA_8888 ||
            infos[index].format == GRAPHIC_PIXEL_FMT_YCRCB_420_SP) {
            supporteds.push_back(true);
        } else {
            supporteds.push_back(false);
        }
    }
    return GSERROR_OK;
}

GSError BufferQueue::SetScalingMode(uint32_t sequence, ScalingMode scalingMode)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    bufferQueueCache_[sequence].scalingMode = scalingMode;
    return GSERROR_OK;
}

GSError BufferQueue::GetScalingMode(uint32_t sequence, ScalingMode &scalingMode)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    scalingMode = bufferQueueCache_.at(sequence).scalingMode;
    return GSERROR_OK;
}

GSError BufferQueue::SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (metaData.size() == 0) {
        BLOGN_INVALID("metaData size is 0");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    bufferQueueCache_[sequence].metaData.clear();
    bufferQueueCache_[sequence].metaData = metaData;
    bufferQueueCache_[sequence].hdrMetaDataType = HDRMetaDataType::HDR_META_DATA;
    return GSERROR_OK;
}

GSError BufferQueue::SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
                                    const std::vector<uint8_t> &metaData)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (key < GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X ||
        key > GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR_VIVID) {
        BLOGN_INVALID("key [%{public}d, %{public}d), now is %{public}d",
                      GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X,
                      GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR_VIVID, key);
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (metaData.size() == 0) {
        BLOGN_INVALID("metaData size is 0");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    bufferQueueCache_[sequence].metaDataSet.clear();
    bufferQueueCache_[sequence].key = key;
    bufferQueueCache_[sequence].metaDataSet = metaData;
    bufferQueueCache_[sequence].hdrMetaDataType = HDRMetaDataType::HDR_META_DATA_SET;
    return GSERROR_OK;
}

GSError BufferQueue::QueryMetaDataType(uint32_t sequence, HDRMetaDataType &type)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    type = bufferQueueCache_.at(sequence).hdrMetaDataType;
    return GSERROR_OK;
}

GSError BufferQueue::GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData> &metaData)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    metaData.clear();
    metaData = bufferQueueCache_.at(sequence).metaData;
    return GSERROR_OK;
}

GSError BufferQueue::GetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey &key,
                                    std::vector<uint8_t> &metaData)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    metaData.clear();
    key = bufferQueueCache_.at(sequence).key;
    metaData = bufferQueueCache_.at(sequence).metaDataSet;
    return GSERROR_OK;
}

GSError BufferQueue::SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    bool tunnelHandleChange = false;
    if (tunnelHandle_ == nullptr) {
        if (handle == nullptr) {
            BLOGN_INVALID("tunnel handle is nullptr");
            return GSERROR_INVALID_ARGUMENTS;
        }
        tunnelHandleChange = true;
    } else {
        tunnelHandleChange = tunnelHandle_->Different(handle);
    }
    if (!tunnelHandleChange) {
        BLOGNW("same tunnel handle, please check");
        return GSERROR_NO_ENTRY;
    }
    tunnelHandle_ = handle;
    {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnTunnelHandleChange");
            listener_->OnTunnelHandleChange();
        } else if (listenerClazz_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnTunnelHandleChange");
            listenerClazz_->OnTunnelHandleChange();
        } else {
            return GSERROR_NO_CONSUMER;
        }
    }
    return GSERROR_OK;
}

sptr<SurfaceTunnelHandle> BufferQueue::GetTunnelHandle()
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    return tunnelHandle_;
}

GSError BufferQueue::SetPresentTimestamp(uint32_t sequence, const GraphicPresentTimestamp &timestamp)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    bufferQueueCache_[sequence].presentTimestamp = timestamp;
    return GSERROR_OK;
}

GSError BufferQueue::GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type, int64_t &time)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    if (type != bufferQueueCache_.at(sequence).presentTimestamp.type) {
        BLOGN_FAILURE_ID(sequence, "PresentTimestampType [%{public}d] is not supported, the supported type "\
        "is [%{public}d]", type, bufferQueueCache_.at(sequence).presentTimestamp.type);
        return GSERROR_NO_ENTRY;
    }
    switch (type) {
        case GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_DELAY: {
            time = bufferQueueCache_.at(sequence).presentTimestamp.time;
            return GSERROR_OK;
        }
        case GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_TIMESTAMP: {
            time = bufferQueueCache_.at(sequence).presentTimestamp.time - bufferQueueCache_.at(sequence).timestamp;
            return GSERROR_OK;
        }
        default: {
            BLOGN_FAILURE_ID(sequence, "unsupported type!");
            return GSERROR_TYPE_ERROR;
        }
    }
}

void BufferQueue::DumpCache(std::string &result)
{
    for (auto it = bufferQueueCache_.begin(); it != bufferQueueCache_.end(); it++) {
        BufferElement element = it->second;
        if (BufferStateStrs.find(element.state) != BufferStateStrs.end()) {
            result += "        sequence = " + std::to_string(it->first) +
                ", state = " + BufferStateStrs.at(element.state) +
                ", timestamp = " + std::to_string(element.timestamp);
        }
        for (decltype(element.damages.size()) i = 0; i < element.damages.size(); i++) {
            result += ", damagesRect = [" + std::to_string(i) + "] = [" +
            std::to_string(element.damages[i].x) + ", " +
            std::to_string(element.damages[i].y) + ", " +
            std::to_string(element.damages[i].w) + ", " +
            std::to_string(element.damages[i].h) + "],";
        }
        result += " config = [" + std::to_string(element.config.width) + "x" +
            std::to_string(element.config.height) + ", " +
            std::to_string(element.config.strideAlignment) + ", " +
            std::to_string(element.config.format) +", " +
            std::to_string(element.config.usage) + ", " +
            std::to_string(element.config.timeout) + ", " +
            std::to_string(element.config.colorGamut) + ", " +
            std::to_string(element.config.transform) + "],";

        result += " scalingMode = " + std::to_string(element.scalingMode) + ",";
        result += " HDR = " + std::to_string(element.hdrMetaDataType) + ", ";

        double bufferMemSize = 0;
        if (element.buffer != nullptr) {
            result += " bufferWith = " + std::to_string(element.buffer->GetWidth()) +
                    ", bufferHeight = " + std::to_string(element.buffer->GetHeight());
            bufferMemSize = static_cast<double>(element.buffer->GetSize()) / BUFFER_MEMSIZE_RATE;
        }

        std::ostringstream ss;
        ss.precision(BUFFER_MEMSIZE_FORMAT);
        ss.setf(std::ios::fixed);
        ss << bufferMemSize;
        std::string str = ss.str();
        result += ", bufferMemSize = " + str + "(KiB).\n";
    }
}

void BufferQueue::Dump(std::string &result)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    std::ostringstream ss;
    ss.precision(BUFFER_MEMSIZE_FORMAT);
    ss.setf(std::ios::fixed);
    static double allSurfacesMemSize = 0;
    uint32_t totalBufferListSize = 0;
    double memSizeInKB = 0;

    for (auto it = bufferQueueCache_.begin(); it != bufferQueueCache_.end(); it++) {
        BufferElement element = it->second;
        if (element.buffer != nullptr) {
            totalBufferListSize += element.buffer->GetSize();
        }
    }
    memSizeInKB = static_cast<double>(totalBufferListSize) / BUFFER_MEMSIZE_RATE;

    allSurfacesMemSize += memSizeInKB;
    uint32_t resultLen = result.size();
    std::string dumpEndFlag = "dumpend";
    std::string dumpEndIn(result, resultLen - dumpEndFlag.size(), resultLen - 1);
    if (dumpEndIn == dumpEndFlag) {
        ss << allSurfacesMemSize;
        std::string dumpEndStr = ss.str();
        result.erase(resultLen - dumpEndFlag.size(), resultLen - 1);
        result += dumpEndStr + " KiB.\n";
        allSurfacesMemSize = 0;
        return;
    }

    ss.str("");
    ss << memSizeInKB;
    std::string str = ss.str();
    result.append("    BufferQueue:\n");
    result += "      default-size = [" + std::to_string(defaultWidth) + "x" + std::to_string(defaultHeight) + "]" +
        ", FIFO = " + std::to_string(queueSize_) +
        ", name = " + name_ +
        ", uniqueId = " + std::to_string(uniqueId_) +
        ", usedBufferListLen = " + std::to_string(GetUsedSize()) +
        ", freeBufferListLen = " + std::to_string(freeList_.size()) +
        ", dirtyBufferListLen = " + std::to_string(dirtyList_.size()) +
        ", totalBuffersMemSize = " + str + "(KiB).\n";

    result.append("      bufferQueueCache:\n");
    DumpCache(result);
}

bool BufferQueue::GetStatus() const
{
    return isValidStatus_;
}

void BufferQueue::SetStatus(bool status)
{
    isValidStatus_ = status;
    waitReqCon_.notify_all();
}
}; // namespace OHOS
