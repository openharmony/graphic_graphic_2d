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
#include <scoped_bytrace.h>

#include "buffer_utils.h"
#include "buffer_log.h"
#include "buffer_manager.h"
#include "hitrace_meter.h"
#include "surface_buffer_impl.h"
#include "sync_fence.h"
#include "sandbox_utils.h"

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

BufferQueue::BufferQueue(const std::string &name, bool isShared)
    : name_(name), uniqueId_(GetUniqueIdImpl()), isShared_(isShared)
{
    BLOGNI("ctor, Queue id: %{public}" PRIu64 " isShared: %{public}d", uniqueId_, isShared);
    bufferManager_ = BufferManager::GetInstance();
    if (isShared_ == true) {
        queueSize_ = 1;
    }
}

BufferQueue::~BufferQueue()
{
    BLOGNI("dtor, Queue id: %{public}" PRIu64, uniqueId_);
    for (auto &[id, _] : bufferQueueCache_) {
        if (onBufferDelete_ != nullptr) {
            onBufferDelete_(id);
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

    if (freeList_.empty()) {
        buffer = nullptr;
        return GSERROR_NO_BUFFER;
    }

    buffer = bufferQueueCache_[freeList_.front()].buffer;
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

GSError BufferQueue::CheckFlushConfig(const BufferFlushConfig &config)
{
    if (config.damage.w < 0) {
        BLOGN_INVALID("config.damage.w >= 0, now is %{public}d", config.damage.w);
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (config.damage.h < 0) {
        BLOGN_INVALID("config.damage.h >= 0, now is %{public}d", config.damage.h);
        return GSERROR_INVALID_ARGUMENTS;
    }
    return GSERROR_OK;
}

GSError BufferQueue::RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
    struct IBufferProducer::RequestBufferReturnValue &retval)
{
    ScopedBytrace func(__func__);
    if (!GetStatus()) {
        BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
    }
    {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ == nullptr && listenerClazz_ == nullptr) {
            BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
        }
    }

    // check param
    GSError ret = CheckRequestConfig(config);
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
        retval.sequence = buffer->GetSeqNum();
        bedata = buffer->GetExtraData();
        retval.fence = SyncFence::INVALID_FENCE;
        BLOGND("Success alloc Buffer[%{public}d %{public}d] id: %{public}d id: %{public}" PRIu64, config.width,
            config.height, retval.sequence, uniqueId_);
    } else {
        BLOGNE("Fail to alloc or map Buffer[%{public}d %{public}d] ret: %{public}d, id: %{public}" PRIu64,
            config.width, config.height, ret, uniqueId_);
    }

    return ret;
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
    }

    bufferQueueCache_[retval.sequence].state = BUFFER_STATE_REQUESTED;
    retval.fence = bufferQueueCache_[retval.sequence].fence;
    bedata = retval.buffer->GetExtraData();

    auto &dbs = retval.deletingBuffers;
    dbs.insert(dbs.end(), deletingList_.begin(), deletingList_.end());
    deletingList_.clear();

    if (needRealloc || isShared_ || producerCacheClean_) {
        BLOGND("RequestBuffer Succ realloc Buffer[%{public}d %{public}d] with new config "\
            "qid: %{public}d id: %{public}" PRIu64, config.width, config.height, retval.sequence, uniqueId_);
        if (producerCacheClean_) {
            producerCacheList_.push_back(retval.sequence);
            if (CheckProducerCacheList()) {
                SetProducerCacheCleanFlagLocked(false);
            }
        }
    } else {
        BLOGND("RequestBuffer Succ Buffer[%{public}d %{public}d] in seq id: %{public}d "\
            "qid: %{public}" PRIu64 " releaseFence: %{public}d",
            config.width, config.height, retval.sequence, uniqueId_, retval.fence->Get());
        retval.buffer = nullptr;
    }

    ScopedBytrace bufferName(name_ + ":" + std::to_string(retval.sequence));
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
        BLOGN_FAILURE_ID(sequence, "not found in cache");
        return GSERROR_NO_ENTRY;
    }

    if (bufferQueueCache_[sequence].state != BUFFER_STATE_REQUESTED) {
        BLOGN_FAILURE_ID(sequence, "state is not BUFFER_STATE_REQUESTED");
        return GSERROR_INVALID_OPERATING;
    }
    bufferQueueCache_[sequence].state = BUFFER_STATE_RELEASED;
    freeList_.push_back(sequence);
    bufferQueueCache_[sequence].buffer->SetExtraData(bedata);

    waitReqCon_.notify_all();
    BLOGND("Success Buffer id: %{public}d Queue id: %{public}" PRIu64, sequence, uniqueId_);

    return GSERROR_OK;
}

GSError BufferQueue::FlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
    const sptr<SyncFence>& fence, const BufferFlushConfig &config)
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

    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
            BLOGN_FAILURE_ID(sequence, "not found in cache");
            return GSERROR_NO_ENTRY;
        }

        if (isShared_ == false) {
            auto &state = bufferQueueCache_[sequence].state;
            if (state != BUFFER_STATE_REQUESTED && state != BUFFER_STATE_ATTACHED) {
                BLOGN_FAILURE_ID(sequence, "invalid state %{public}d", state);
                return GSERROR_NO_ENTRY;
            }
        }
    }

    {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ == nullptr && listenerClazz_ == nullptr) {
            CancelBuffer(sequence, bedata);
            return GSERROR_NO_CONSUMER;
        }
    }

    ScopedBytrace bufferIPCSend("BufferIPCSend");
    sret = DoFlushBuffer(sequence, bedata, fence, config);
    if (sret != GSERROR_OK) {
        return sret;
    }
    CountTrace(HITRACE_TAG_GRAPHIC_AGP, name_, static_cast<int32_t>(dirtyList_.size()));
    if (sret == GSERROR_OK) {
        std::lock_guard<std::mutex> lockGuard(listenerMutex_);
        if (listener_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnBufferAvailable");
            listener_->OnBufferAvailable();
        } else if (listenerClazz_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnBufferAvailable");
            listenerClazz_->OnBufferAvailable();
        }
    }
    BLOGND("Success Buffer seq id: %{public}d Queue id: %{public}" PRIu64 " AcquireFence:%{public}d",
        sequence, uniqueId_, fence->Get());
    return sret;
}

void BufferQueue::DumpToFile(uint32_t sequence)
{
    if (access("/data/bq_dump", F_OK) == -1) {
        return;
    }

    ScopedBytrace func(__func__);
    struct timeval now;
    gettimeofday(&now, nullptr);
    constexpr int secToUsec = 1000 * 1000;
    int64_t nowVal = (int64_t)now.tv_sec * secToUsec + (int64_t)now.tv_usec;

    std::stringstream ss;
    ss << "/data/bq_" << GetRealPid() << "_" << name_ << "_" << nowVal << ".raw";

    sptr<SurfaceBuffer>& buffer = bufferQueueCache_[sequence].buffer;
    std::ofstream rawDataFile(ss.str(), std::ofstream::binary);
    if (!rawDataFile.good()) {
        BLOGE("open failed: (%{public}d)%{public}s", errno, strerror(errno));
        return;
    }
    rawDataFile.write(static_cast<const char *>(buffer->GetVirAddr()), buffer->GetSize());
    rawDataFile.close();
}

GSError BufferQueue::DoFlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
    const sptr<SyncFence>& fence, const BufferFlushConfig &config)
{
    ScopedBytrace func(__func__);
    ScopedBytrace bufferName(name_ + ":" + std::to_string(sequence));
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not found in cache");
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
    bufferQueueCache_[sequence].damage = config.damage;

    uint64_t usage = static_cast<uint32_t>(bufferQueueCache_[sequence].config.usage);
    if (usage & BUFFER_USAGE_CPU_WRITE) {
        // api flush
        auto sret = bufferQueueCache_[sequence].buffer->FlushCache();
        if (sret != GSERROR_OK) {
            BLOGN_FAILURE_ID_API(sequence, FlushCache, sret);
            return sret;
        }
    }

    if (config.timestamp == 0) {
        struct timeval tv = {};
        gettimeofday(&tv, nullptr);
        constexpr int32_t secToUsec = 1000000;
        bufferQueueCache_[sequence].timestamp = (int64_t)tv.tv_usec + (int64_t)tv.tv_sec * secToUsec;
    } else {
        bufferQueueCache_[sequence].timestamp = config.timestamp;
    }

    DumpToFile(sequence);
    return GSERROR_OK;
}

GSError BufferQueue::AcquireBuffer(sptr<SurfaceBuffer> &buffer,
    sptr<SyncFence> &fence, int64_t &timestamp, Rect &damage)
{
    ScopedBytrace func(__func__);
    // dequeue from dirty list
    std::lock_guard<std::mutex> lockGuard(mutex_);
    GSError ret = PopFromDirtyList(buffer);
    if (ret == GSERROR_OK) {
        uint32_t sequence = buffer->GetSeqNum();
        if (isShared_ == false && bufferQueueCache_[sequence].state != BUFFER_STATE_FLUSHED) {
            BLOGNW("Warning [%{public}d], Reason: state is not BUFFER_STATE_FLUSHED", sequence);
        }
        bufferQueueCache_[sequence].state = BUFFER_STATE_ACQUIRED;

        fence = bufferQueueCache_[sequence].fence;
        timestamp = bufferQueueCache_[sequence].timestamp;
        damage = bufferQueueCache_[sequence].damage;

        ScopedBytrace bufferName(name_ + ":" + std::to_string(sequence));
        BLOGND("Success Buffer seq id: %{public}d Queue id: %{public}" PRIu64 " AcquireFence:%{public}d",
            sequence, uniqueId_, fence->Get());
    } else if (ret == GSERROR_NO_BUFFER) {
        BLOGN_FAILURE("there is no dirty buffer");
    }

    CountTrace(HITRACE_TAG_GRAPHIC_AGP, name_, static_cast<int32_t>(dirtyList_.size()));
    return ret;
}

GSError BufferQueue::ReleaseBuffer(sptr<SurfaceBuffer> &buffer, const sptr<SyncFence>& fence)
{
    if (buffer == nullptr) {
        BLOGE("invalid parameter: buffer is null, please check");
        return GSERROR_INVALID_ARGUMENTS;
    }

    uint32_t sequence = buffer->GetSeqNum();
    ScopedBytrace bufferName(std::string(__func__) + "," + name_ + ":" + std::to_string(sequence));
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
            BLOGN_FAILURE_ID(sequence, "not find in cache, Queue id: %{public}" PRIu64, uniqueId_);
            return GSERROR_NO_ENTRY;
        }

        if (isShared_ == false) {
            const auto &state = bufferQueueCache_[sequence].state;
            if (state != BUFFER_STATE_ACQUIRED && state != BUFFER_STATE_ATTACHED) {
                BLOGN_FAILURE_ID(sequence, "invalid state");
                return GSERROR_NO_ENTRY;
            }
        }
    }

    if (onBufferRelease != nullptr) {
        ScopedBytrace func("OnBufferRelease");
        sptr<SurfaceBuffer> buf = buffer;
        auto sret = onBufferRelease(buf);
        if (sret == GSERROR_OK) {   // need to check why directly return?
            return sret;
        }
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not find in cache, Queue id: %{public}" PRIu64 "", uniqueId_);
        return GSERROR_NO_ENTRY;
    }
    bufferQueueCache_[sequence].state = BUFFER_STATE_RELEASED;
    bufferQueueCache_[sequence].fence = fence;

    if (bufferQueueCache_[sequence].isDeleting) {
        DeleteBufferInCache(sequence);
        BLOGND("Succ delete Buffer seq id: %{public}d Queue id: %{public}" PRIu64 " in cache", sequence, uniqueId_);
    } else {
        freeList_.push_back(sequence);
        BLOGND("Succ push Buffer seq id: %{public}d Qid: %{public}" PRIu64 " to free list, releaseFence: %{public}d",
            sequence, uniqueId_, fence->Get());
    }
    waitReqCon_.notify_all();
    return GSERROR_OK;
}

GSError BufferQueue::AllocBuffer(sptr<SurfaceBuffer> &buffer,
    const BufferRequestConfig &config)
{
    ScopedBytrace func(__func__);
    sptr<SurfaceBuffer> bufferImpl = new SurfaceBufferImpl();
    uint32_t sequence = bufferImpl->GetSeqNum();

    GSError ret = bufferImpl->Alloc(config);
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

    ret = bufferImpl->Map();
    if (ret == GSERROR_OK) {
        BLOGN_SUCCESS_ID(sequence, "Map");
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
        if (onBufferDelete_ != nullptr) {
            onBufferDelete_(sequence);
        }
        bufferQueueCache_.erase(it);
        deletingList_.push_back(sequence);
    }
}

uint32_t BufferQueue::GetQueueSize()
{
    return queueSize_;
}

void BufferQueue::DeleteBuffers(int32_t count)
{
    ScopedBytrace func(__func__);
    if (count <= 0) {
        return;
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
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

GSError BufferQueue::AttachBuffer(sptr<SurfaceBuffer> &buffer)
{
    ScopedBytrace func(__func__);
    if (isShared_) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_OPERATING);
    }

    if (buffer == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    BufferElement ele = {
        .buffer = buffer,
        .state = BUFFER_STATE_ATTACHED,
        .config = {
            .width = buffer->GetWidth(),
            .height = buffer->GetHeight(),
            .strideAlignment = 0x8,
            .format = buffer->GetFormat(),
            .usage = buffer->GetUsage(),
            .timeout = 0,
        },
        .damage = {
            .w = ele.config.width,
            .h = ele.config.height,
        }
    };

    uint32_t sequence = buffer->GetSeqNum();
    int32_t usedSize = static_cast<int32_t>(GetUsedSize());
    int32_t queueSize = static_cast<int32_t>(GetQueueSize());
    if (usedSize >= queueSize) {
        int32_t freeSize = static_cast<int32_t>(dirtyList_.size() + freeList_.size());
        if (freeSize >= usedSize - queueSize + 1) {
            DeleteBuffers(usedSize - queueSize + 1);
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
        BLOGN_FAILURE_ID(sequence, "not find in cache");
        return GSERROR_NO_ENTRY;
    }

    if (bufferQueueCache_[sequence].state == BUFFER_STATE_REQUESTED) {
        BLOGN_SUCCESS_ID(sequence, "requested");
    } else if (bufferQueueCache_[sequence].state == BUFFER_STATE_ACQUIRED) {
        BLOGN_SUCCESS_ID(sequence, "acquired");
    } else {
        BLOGN_FAILURE_ID_RET(sequence, GSERROR_NO_ENTRY);
    }
    if (onBufferDelete_ != nullptr) {
        onBufferDelete_(sequence);
    }
    bufferQueueCache_.erase(sequence);
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

    DeleteBuffers(queueSize_ - queueSize);

    // if increase the queue size, try to wakeup the blocked thread
    if (queueSize > queueSize_) {
        queueSize_ = queueSize;
        waitReqCon_.notify_all();
    } else {
        queueSize_ = queueSize;
    }

    BLOGN_SUCCESS("queue size: %{public}d, Queue id: %{public}" PRIu64, queueSize_, uniqueId_);
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
    onBufferRelease = func;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterDeleteBufferListener(OnDeleteBufferFunc func)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (onBufferDelete_ != nullptr) {
        return GSERROR_OK;
    }
    onBufferDelete_ = func;
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

GSError BufferQueue::SetDefaultUsage(uint32_t usage)
{
    defaultUsage = usage;
    return GSERROR_OK;
}

uint32_t BufferQueue::GetDefaultUsage()
{
    return defaultUsage;
}

void BufferQueue::ClearLocked()
{
    for (auto &[id, _] : bufferQueueCache_) {
        if (onBufferDelete_ != nullptr) {
            onBufferDelete_(id);
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

GSError BufferQueue::IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
                                      std::vector<bool> &supporteds) const
{
    GSError ret = bufferManager_->IsSupportedAlloc(infos, supporteds);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE_API(IsSupportedAlloc, ret);
    }
    return ret;
}

GSError BufferQueue::SetScalingMode(uint32_t sequence, ScalingMode scalingMode)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not find in cache");
        return GSERROR_NO_ENTRY;
    }
    bufferQueueCache_[sequence].scalingMode = scalingMode;
    return GSERROR_OK;
}

GSError BufferQueue::GetScalingMode(uint32_t sequence, ScalingMode &scalingMode)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not find in cache");
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
        BLOGN_FAILURE_ID(sequence, "not find in cache");
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
        BLOGN_FAILURE_ID(sequence, "not find in cache");
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
        BLOGN_FAILURE_ID(sequence, "not find in cache");
        return GSERROR_NO_ENTRY;
    }
    type = bufferQueueCache_.at(sequence).hdrMetaDataType;
    return GSERROR_OK;
}

GSError BufferQueue::GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData> &metaData)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not find in cache");
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
        BLOGN_FAILURE_ID(sequence, "not find in cache");
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
        BLOGN_FAILURE_ID(sequence, "not find in cache");
        return GSERROR_NO_ENTRY;
    }
    bufferQueueCache_[sequence].presentTimestamp = timestamp;
    return GSERROR_OK;
}

GSError BufferQueue::GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type, int64_t &time)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not find in cache");
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
        result += ", damageRect = [" + std::to_string(element.damage.x) + ", " +
            std::to_string(element.damage.y) + ", " +
            std::to_string(element.damage.w) + ", " +
            std::to_string(element.damage.h) + "],";
        result += " config = [" + std::to_string(element.config.width) + "x" +
            std::to_string(element.config.height) + ", " +
            std::to_string(element.config.strideAlignment) + ", " +
            std::to_string(element.config.format) +", " +
            std::to_string(element.config.usage) + ", " +
            std::to_string(element.config.timeout) + "],";

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
