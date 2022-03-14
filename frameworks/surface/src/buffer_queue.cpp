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

#include <display_type.h>
#include <scoped_bytrace.h>

#include "buffer_log.h"
#include "buffer_manager.h"

namespace OHOS {
namespace {
constexpr uint32_t UNIQUE_ID_OFFSET = 32;
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
    static uint64_t id = static_cast<uint64_t>(::getpid()) << UNIQUE_ID_OFFSET;
    return id | counter++;
}

BufferQueue::BufferQueue(const std::string &name, bool isShared)
    : name_(name), uniqueId_(GetUniqueIdImpl()), isShared_(isShared)
{
    BLOGNI("ctor, Queue id: %{public}" PRIu64 "", uniqueId_);
    bufferManager_ = BufferManager::GetInstance();
    if (isShared_ == true) {
        queueSize_ = 1;
    }
}

BufferQueue::~BufferQueue()
{
    BLOGNI("dtor, Queue id: %{public}" PRIu64 "", uniqueId_);
    std::lock_guard<std::mutex> lockGuard(mutex_);
    for (auto it = bufferQueueCache_.begin(); it != bufferQueueCache_.end(); it++) {
        FreeBuffer(it->second.buffer);
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

GSError BufferQueue::PopFromFreeList(sptr<SurfaceBufferImpl> &buffer,
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

GSError BufferQueue::PopFromDirtyList(sptr<SurfaceBufferImpl> &buffer)
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
    if (config.width <= 0) {
        BLOGN_INVALID("config.width is greater than 0, now is %{public}d", config.width);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (config.height <= 0) {
        BLOGN_INVALID("config.height is greater than 0, now is %{public}d", config.height);
        return GSERROR_INVALID_ARGUMENTS;
    }

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

    if (config.format < 0 || config.format > PIXEL_FMT_BUTT) {
        BLOGN_INVALID("config.format [0, %{public}d], now is %{public}d", PIXEL_FMT_BUTT, config.format);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (config.colorGamut <= SurfaceColorGamut::COLOR_GAMUT_INVALID ||
        config.colorGamut > SurfaceColorGamut::COLOR_GAMUT_DISPLAY_BT2020 + 1) {
        BLOGN_INVALID("config.colorGamut [0, %{public}d], now is %{public}d",
            static_cast<uint32_t>(SurfaceColorGamut::COLOR_GAMUT_DISPLAY_BT2020),
            static_cast<uint32_t>(config.colorGamut));
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

GSError BufferQueue::RequestBuffer(const BufferRequestConfig &config, BufferExtraData &bedata,
    struct IBufferProducer::RequestBufferReturnValue &retval)
{
    ScopedBytrace func(__func__);
    if (listener_ == nullptr && listenerClazz_ == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_NO_CONSUMER);
    }

    // check param
    GSError ret = CheckRequestConfig(config);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE_API(CheckRequestConfig, ret);
        return ret;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    // dequeue from free list
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = PopFromFreeList(bufferImpl, config);
    if (ret == GSERROR_OK) {
        retval.buffer = bufferImpl;
        return ReuseBuffer(config, bedata, retval);
    }

    // check queue size
    if (GetUsedSize() >= GetQueueSize()) {
        waitReqCon_.wait_for(lock, std::chrono::milliseconds(config.timeout),
            [this]() { return !freeList_.empty() || (GetUsedSize() < GetQueueSize()); });
        // try dequeue from free list again
        ret = PopFromFreeList(bufferImpl, config);
        if (ret == GSERROR_OK) {
            retval.buffer = bufferImpl;
            return ReuseBuffer(config, bedata, retval);
        } else if (GetUsedSize() >= GetQueueSize()) {
            BLOGN_FAILURE("all buffer are using, Queue id: %{public}" PRIu64 "", uniqueId_);
            return GSERROR_NO_BUFFER;
        }
    }

    ret = AllocBuffer(bufferImpl, config);
    if (ret == GSERROR_OK) {
        retval.sequence = bufferImpl->GetSeqNum();

        bufferImpl->GetExtraData(bedata);
        retval.buffer = bufferImpl;
        retval.fence = -1;
        BLOGD("Success alloc Buffer id: %{public}d Queue id: %{public}" PRIu64 "", retval.sequence, uniqueId_);
    } else {
        BLOGE("Fail to alloc or map buffer ret: %{public}d, Queue id: %{public}" PRIu64 "", ret, uniqueId_);
    }

    return ret;
}

GSError BufferQueue::ReuseBuffer(const BufferRequestConfig &config, BufferExtraData &bedata,
    struct IBufferProducer::RequestBufferReturnValue &retval)
{
    ScopedBytrace func(__func__);
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    retval.sequence = bufferImpl->GetSeqNum();
    bool needRealloc = (config != bufferQueueCache_[retval.sequence].config);
    // config, realloc
    if (needRealloc) {
        if (isShared_) {
            BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
        }
        DeleteBufferInCache(retval.sequence);

        auto sret = AllocBuffer(bufferImpl, config);
        if (sret != GSERROR_OK) {
            BLOGN_FAILURE("realloc failed");
            return sret;
        }

        retval.buffer = bufferImpl;
        retval.sequence = bufferImpl->GetSeqNum();
        retval.fence = -1;
        bufferQueueCache_[retval.sequence].config = config;
    }

    bufferQueueCache_[retval.sequence].state = BUFFER_STATE_REQUESTED;
    retval.fence = bufferQueueCache_[retval.sequence].fence;
    bufferImpl->GetExtraData(bedata);

    auto &dbs = retval.deletingBuffers;
    dbs.insert(dbs.end(), deletingList_.begin(), deletingList_.end());
    deletingList_.clear();

    if (needRealloc) {
        BLOGD("RequestBuffer Success realloc Buffer with new config id: %{public}d Queue id: %{public}" PRIu64 "",
            retval.sequence, uniqueId_);
    } else {
        BLOGD("RequestBuffer Success Buffer in cache id: %{public}d Queue id: %{public}" PRIu64 "",
            retval.sequence, uniqueId_);
        retval.buffer = nullptr;
    }

    return GSERROR_OK;
}

GSError BufferQueue::CancelBuffer(int32_t sequence, const BufferExtraData &bedata)
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
    BLOGD("Success Buffer id: %{public}d Queue id: %{public}" PRIu64 "", sequence, uniqueId_);

    return GSERROR_OK;
}

GSError BufferQueue::FlushBuffer(int32_t sequence, const BufferExtraData &bedata,
    int32_t fence, const BufferFlushConfig &config)
{
    ScopedBytrace func(__func__);
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

    if (listener_ == nullptr && listenerClazz_ == nullptr) {
        CancelBuffer(sequence, bedata);
        return GSERROR_NO_CONSUMER;
    }

    ScopedBytrace bufferIPCSend("BufferIPCSend");
    sret = DoFlushBuffer(sequence, bedata, fence, config);
    if (sret != GSERROR_OK) {
        return sret;
    }
    BLOGD("Success Buffer id: %{public}d Queue id: %{public}" PRIu64 "", sequence, uniqueId_);

    if (sret == GSERROR_OK) {
        BLOGN_SUCCESS_ID(sequence, "OnBufferAvailable Start");
        if (listener_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnBufferAvailable");
            listener_->OnBufferAvailable();
        } else if (listenerClazz_ != nullptr) {
            ScopedBytrace bufferIPCSend("OnBufferAvailable");
            listenerClazz_->OnBufferAvailable();
        }
        BLOGN_SUCCESS_ID(sequence, "OnBufferAvailable End");
    }
    return sret;
}

void BufferQueue::DumpToFile(int32_t sequence)
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
    ss << "/data/bq_" << getpid() << "_" << name_ << "_" << nowVal << ".raw";

    sptr<SurfaceBufferImpl> &buffer = bufferQueueCache_[sequence].buffer;
    std::ofstream rawDataFile(ss.str(), std::ofstream::binary);
    if (!rawDataFile.good()) {
        BLOGE("open failed: (%{public}d)%{public}s", errno, strerror(errno));
        return;
    }
    rawDataFile.write(static_cast<const char *>(buffer->GetVirAddr()), buffer->GetSize());
    rawDataFile.close();
}

GSError BufferQueue::DoFlushBuffer(int32_t sequence, const BufferExtraData &bedata,
    int32_t fence, const BufferFlushConfig &config)
{
    ScopedBytrace func(__func__);
    std::lock_guard<std::mutex> lockGuard(mutex_);
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

    uint32_t usage = static_cast<uint32_t>(bufferQueueCache_[sequence].config.usage);
    if (usage & HBM_USE_CPU_WRITE) {
        // api flush
        auto sret = bufferManager_->FlushCache(bufferQueueCache_[sequence].buffer);
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

GSError BufferQueue::AcquireBuffer(sptr<SurfaceBufferImpl> &buffer,
    int32_t &fence, int64_t &timestamp, Rect &damage)
{
    ScopedBytrace func(__func__);
    // dequeue from dirty list
    std::lock_guard<std::mutex> lockGuard(mutex_);
    GSError ret = PopFromDirtyList(buffer);
    if (ret == GSERROR_OK) {
        int32_t sequence = buffer->GetSeqNum();
        if (isShared_ == false && bufferQueueCache_[sequence].state != BUFFER_STATE_FLUSHED) {
            BLOGNW("Warning [%{public}d], Reason: state is not BUFFER_STATE_FLUSHED", sequence);
        }
        bufferQueueCache_[sequence].state = BUFFER_STATE_ACQUIRED;

        fence = bufferQueueCache_[sequence].fence;
        timestamp = bufferQueueCache_[sequence].timestamp;
        damage = bufferQueueCache_[sequence].damage;

    BLOGD("Success Buffer id: %{public}d Queue id: %{public}" PRIu64 "", sequence, uniqueId_);
    } else if (ret == GSERROR_NO_BUFFER) {
        BLOGN_FAILURE("there is no dirty buffer");
    }

    return ret;
}

GSError BufferQueue::ReleaseBuffer(sptr<SurfaceBufferImpl> &buffer, int32_t fence)
{
    ScopedBytrace func(__func__);
    int32_t sequence = buffer->GetSeqNum();
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
            BLOGN_FAILURE_ID(sequence, "not find in cache");
            return GSERROR_NO_ENTRY;
        }

        if (isShared_ == false) {
            auto &state = bufferQueueCache_[sequence].state;
            if (state != BUFFER_STATE_ACQUIRED && state != BUFFER_STATE_ATTACHED) {
                BLOGN_FAILURE_ID(sequence, "invalid state");
                return GSERROR_NO_ENTRY;
            }
        }
    }

    if (onBufferRelease != nullptr) {
        ScopedBytrace func("OnBufferRelease");
        sptr<SurfaceBuffer> buf = buffer;
        BLOGNI("onBufferRelease start");
        auto sret = onBufferRelease(buf);
        BLOGNI("onBufferRelease end return %{public}s", GSErrorStr(sret).c_str());

        if (sret == GSERROR_OK) {   // need to check why directly return?
            return sret;
        }
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    bufferQueueCache_[sequence].state = BUFFER_STATE_RELEASED;
    bufferQueueCache_[sequence].fence = fence;

    if (bufferQueueCache_[sequence].isDeleting) {
        DeleteBufferInCache(sequence);
        BLOGD("Success delete Buffer id: %{public}d Queue id: %{public}" PRIu64 " in cache", sequence, uniqueId_);
    } else {
        freeList_.push_back(sequence);
        BLOGD("Success push Buffer id: %{public}d Queue id: %{public}" PRIu64 " to free list", sequence, uniqueId_);
    }
    waitReqCon_.notify_all();
    return GSERROR_OK;
}

GSError BufferQueue::AllocBuffer(sptr<SurfaceBufferImpl> &buffer,
    const BufferRequestConfig &config)
{
    ScopedBytrace func(__func__);
    buffer = new SurfaceBufferImpl();
    int32_t sequence = buffer->GetSeqNum();

    GSError ret = bufferManager_->Alloc(config, buffer);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE_ID_API(sequence, Alloc, ret);
        return ret;
    }

    BufferElement ele = {
        .buffer = buffer,
        .state = BUFFER_STATE_REQUESTED,
        .isDeleting = false,
        .config = config,
        .fence = -1
    };

    ret = bufferManager_->Map(buffer);
    if (ret == GSERROR_OK) {
        BLOGN_SUCCESS_ID(sequence, "Map");
        bufferQueueCache_[sequence] = ele;
        return GSERROR_OK;
    }

    GSError freeRet = bufferManager_->Free(buffer);
    if (freeRet != GSERROR_OK) {
        BLOGN_FAILURE_ID(sequence, "Map failed, Free failed");
    } else {
        BLOGN_FAILURE_ID(sequence, "Map failed, Free success");
    }

    return ret;
}

GSError BufferQueue::FreeBuffer(sptr<SurfaceBufferImpl> &buffer)
{
    BLOGND("Free [%{public}d]", buffer->GetSeqNum());
    buffer->SetEglData(nullptr);
    bufferManager_->Unmap(buffer);
    bufferManager_->Free(buffer);
    return GSERROR_OK;
}

void BufferQueue::DeleteBufferInCache(int32_t sequence)
{
    auto it = bufferQueueCache_.find(sequence);
    if (it != bufferQueueCache_.end()) {
        FreeBuffer(it->second.buffer);
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
        if (ele.second.state == BUFFER_STATE_ACQUIRED) {
            FreeBuffer(ele.second.buffer);
        }

        count--;
        if (count <= 0) {
            break;
        }
    }
}

GSError BufferQueue::AttachBuffer(sptr<SurfaceBufferImpl> &buffer)
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

    int32_t sequence = buffer->GetSeqNum();
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

GSError BufferQueue::DetachBuffer(sptr<SurfaceBufferImpl> &buffer)
{
    ScopedBytrace func(__func__);
    if (isShared_) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_OPERATING);
    }

    if (buffer == nullptr) {
        BLOGN_FAILURE_RET(GSERROR_INVALID_ARGUMENTS);
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    int32_t sequence = buffer->GetSeqNum();
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
        BLOGN_INVALID("queue size (%{public}d) > %{public}d", queueSize, SURFACE_MAX_QUEUE_SIZE);
        return GSERROR_INVALID_ARGUMENTS;
    }

    DeleteBuffers(queueSize_ - queueSize);
    queueSize_ = queueSize;

    BLOGN_SUCCESS("queue size: %{public}d, Queue id: %{public}" PRIu64 "", queueSize, uniqueId_);
    return GSERROR_OK;
}

GSError BufferQueue::GetName(std::string &name)
{
    name = name_;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterConsumerListener(sptr<IBufferConsumerListener> &listener)
{
    listener_ = listener;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterConsumerListener(IBufferConsumerListenerClazz *listener)
{
    listenerClazz_ = listener;
    return GSERROR_OK;
}

GSError BufferQueue::UnregisterConsumerListener()
{
    listener_ = nullptr;
    listenerClazz_ = nullptr;
    return GSERROR_OK;
}

GSError BufferQueue::RegisterReleaseListener(OnReleaseFunc func)
{
    onBufferRelease = func;
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

GSError BufferQueue::CleanCache()
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    auto it = bufferQueueCache_.begin();
    while (it != bufferQueueCache_.end()) {
        bufferQueueCache_.erase(it++);
    }
    freeList_.clear();
    dirtyList_.clear();
    deletingList_.clear();
    waitReqCon_.notify_all();
    return GSERROR_OK;
}

uint64_t BufferQueue::GetUniqueId() const
{
    return uniqueId_;
}

void BufferQueue::DumpCache(std::string &result)
{
    for (auto it = bufferQueueCache_.begin(); it != bufferQueueCache_.end(); it++) {
        BufferElement element = it->second;
        result += "        sequence = " + std::to_string(it->first) +
            ", state = " + BufferStateStrs.at(element.state) +
            ", timestamp = " + std::to_string(element.timestamp);
        result += ", damageRect = [" + std::to_string(element.damage.x) + ", " +
            std::to_string(element.damage.y) + ", " +
            std::to_string(element.damage.w) + ", " +
            std::to_string(element.damage.h) + "],";
        result += " config = [" + std::to_string(element.config.width) + "x" +
            std::to_string(element.config.height) + ", " +
            std::to_string(element.config.strideAlignment) + ", " +
            std::to_string(element.config.format) +", " +
            std::to_string(element.config.usage) + ", " +
            std::to_string(element.config.timeout) + "].\n";
    }
}

void BufferQueue::Dump(std::string &result)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    result.append("    BufferQueue:\n");
    result += "      default-size = [" + std::to_string(defaultWidth) + "x" + std::to_string(defaultHeight) + "]" +
        ", FIFO = " + std::to_string(queueSize_) +
        ", name = " + name_ +
        ", uniqueId = " + std::to_string(uniqueId_) + ".\n";

    result.append("      bufferQueueCache:\n");
    DumpCache(result);
}
}; // namespace OHOS
