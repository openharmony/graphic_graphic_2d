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
#include <unistd.h>

#include <display_type.h>
#include <graphic_bytrace.h>

#include "buffer_log.h"
#include "buffer_manager.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "BufferQueue" };
}

BufferQueue::BufferQueue(const std::string &name, bool isShared)
    : name_(name), isShared_(isShared)
{
    BLOGNI("ctor");
    bufferManager_ = BufferManager::GetInstance();
    if (isShared_ == true) {
        queueSize_ = 1;
    }
}

BufferQueue::~BufferQueue()
{
    BLOGNI("dtor");
    std::lock_guard<std::mutex> lockGuard(mutex_);
    for (auto it = bufferQueueCache_.begin(); it != bufferQueueCache_.end(); it++) {
        FreeBuffer(it->second.buffer);
    }
}

SurfaceError BufferQueue::Init()
{
    return SURFACE_ERROR_OK;
}

uint32_t BufferQueue::GetUsedSize()
{
    uint32_t used_size = bufferQueueCache_.size();
    return used_size;
}

SurfaceError BufferQueue::PopFromFreeList(sptr<SurfaceBufferImpl> &buffer,
    const BufferRequestConfig &config)
{
    if (isShared_ == true && GetUsedSize() > 0) {
        buffer = bufferQueueCache_.begin()->second.buffer;
        return SURFACE_ERROR_OK;
    }

    for (auto it = freeList_.begin(); it != freeList_.end(); it++) {
        if (bufferQueueCache_[*it].config == config) {
            buffer = bufferQueueCache_[*it].buffer;
            freeList_.erase(it);
            return SURFACE_ERROR_OK;
        }
    }

    if (freeList_.empty()) {
        buffer = nullptr;
        return SURFACE_ERROR_NO_BUFFER;
    }

    buffer = bufferQueueCache_[freeList_.front()].buffer;
    freeList_.pop_front();
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::PopFromDirtyList(sptr<SurfaceBufferImpl> &buffer)
{
    if (isShared_ == true && GetUsedSize() > 0) {
        buffer = bufferQueueCache_.begin()->second.buffer;
        return SURFACE_ERROR_OK;
    }

    if (!dirtyList_.empty()) {
        buffer = bufferQueueCache_[dirtyList_.front()].buffer;
        dirtyList_.pop_front();
        return SURFACE_ERROR_OK;
    } else {
        buffer = nullptr;
        return SURFACE_ERROR_NO_BUFFER;
    }
}

SurfaceError BufferQueue::CheckRequestConfig(const BufferRequestConfig &config)
{
    if (config.width <= 0 || config.width > SURFACE_MAX_WIDTH) {
        BLOGN_INVALID("config.width (0, %{public}d], now is %{public}d", SURFACE_MAX_WIDTH, config.width);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (config.height <= 0 || config.height > SURFACE_MAX_HEIGHT) {
        BLOGN_INVALID("config.height (0, %{public}d], now is %{public}d", SURFACE_MAX_HEIGHT, config.height);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    uint32_t align = config.strideAlignment;
    bool isValidStrideAlignment = true;
    isValidStrideAlignment = isValidStrideAlignment && (SURFACE_MIN_STRIDE_ALIGNMENT <= align);
    isValidStrideAlignment = isValidStrideAlignment && (SURFACE_MAX_STRIDE_ALIGNMENT >= align);
    if (!isValidStrideAlignment) {
        BLOGN_INVALID("config.strideAlignment [%{public}d, %{public}d], now is %{public}d",
                      SURFACE_MIN_STRIDE_ALIGNMENT, SURFACE_MAX_STRIDE_ALIGNMENT, align);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (align & (align - 1)) {
        BLOGN_INVALID("config.strideAlignment is not power of 2 like 4, 8, 16, 32; now is %{public}d", align);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (config.format < 0 || config.format > PIXEL_FMT_BUTT) {
        BLOGN_INVALID("config.format [0, %{public}d], now is %{public}d", PIXEL_FMT_BUTT, config.format);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::CheckFlushConfig(const BufferFlushConfig &config)
{
    if (config.damage.w < 0) {
        BLOGN_INVALID("config.damage.w >= 0, now is %{public}d", config.damage.w);
        return SURFACE_ERROR_INVALID_PARAM;
    }
    if (config.damage.h < 0) {
        BLOGN_INVALID("config.damage.h >= 0, now is %{public}d", config.damage.h);
        return SURFACE_ERROR_INVALID_PARAM;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::RequestBuffer(const BufferRequestConfig &config, BufferExtraData &bedata,
                                        struct IBufferProducer::RequestBufferReturnValue &retval)
{
    ScopedBytrace func(__func__);
    if (listener_ == nullptr && listenerClazz_ == nullptr) {
        BLOGN_FAILURE_RET(SURFACE_ERROR_NO_CONSUMER);
    }

    // check param
    SurfaceError ret = CheckRequestConfig(config);
    if (ret != SURFACE_ERROR_OK) {
        BLOGN_FAILURE_API(CheckRequestConfig, ret);
        return ret;
    }

    // dequeue from free list
    std::lock_guard<std::mutex> lockGuard(mutex_);
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = PopFromFreeList(bufferImpl, config);
    if (ret == SURFACE_ERROR_OK) {
        retval.buffer = bufferImpl;
        return ReuseBuffer(config, bedata, retval);
    }

    // check queue size
    if (GetUsedSize() >= GetQueueSize()) {
        BLOGN_FAILURE("all buffer are using");
        return SURFACE_ERROR_NO_BUFFER;
    }

    ret = AllocBuffer(bufferImpl, config);
    if (ret == SURFACE_ERROR_OK) {
        retval.sequence = bufferImpl->GetSeqNum();
        BLOGN_SUCCESS_ID(retval.sequence, "alloc");
    }
    bufferImpl->GetExtraData(bedata);
    retval.buffer = bufferImpl;
    retval.fence = -1;
    return ret;
}

SurfaceError BufferQueue::ReuseBuffer(const BufferRequestConfig &config, BufferExtraData &bedata,
                                      struct IBufferProducer::RequestBufferReturnValue &retval)
{
    ScopedBytrace func(__func__);
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    retval.sequence = bufferImpl->GetSeqNum();
    bool needRealloc = (config != bufferQueueCache_[retval.sequence].config);
    // config, realloc
    if (needRealloc) {
        if (isShared_) {
            BLOGN_FAILURE_RET(SURFACE_ERROR_INVALID_PARAM);
        }
        DeleteBufferInCache(retval.sequence);

        auto sret = AllocBuffer(bufferImpl, config);
        if (sret != SURFACE_ERROR_OK) {
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
        BLOGN_SUCCESS_ID(retval.sequence, "config change, realloc");
    } else {
        BLOGN_SUCCESS_ID(retval.sequence, "buffer cache");
        retval.buffer = nullptr;
    }

    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::CancelBuffer(int32_t sequence, const BufferExtraData &bedata)
{
    ScopedBytrace func(__func__);
    if (isShared_) {
        BLOGN_FAILURE_RET(SURFACE_ERROR_INVALID_OPERATING);
    }
    std::lock_guard<std::mutex> lockGuard(mutex_);

    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not found in cache");
        return SURFACE_ERROR_NO_ENTRY;
    }

    if (bufferQueueCache_[sequence].state != BUFFER_STATE_REQUESTED) {
        BLOGN_FAILURE_ID(sequence, "state is not BUFFER_STATE_REQUESTED");
        return SURFACE_ERROR_INVALID_OPERATING;
    }
    bufferQueueCache_[sequence].state = BUFFER_STATE_RELEASED;
    freeList_.push_back(sequence);
    bufferQueueCache_[sequence].buffer->SetExtraData(bedata);

    BLOGN_SUCCESS_ID(sequence, "cancel");

    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::FlushBuffer(int32_t sequence, const BufferExtraData &bedata,
                                      int32_t fence, const BufferFlushConfig &config)
{
    ScopedBytrace func(__func__);
    // check param
    auto sret = CheckFlushConfig(config);
    if (sret != SURFACE_ERROR_OK) {
        BLOGN_FAILURE_API(CheckFlushConfig, sret);
        return sret;
    }

    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
            BLOGN_FAILURE_ID(sequence, "not found in cache");
            return SURFACE_ERROR_NO_ENTRY;
        }

        if (isShared_ == false) {
            auto &state = bufferQueueCache_[sequence].state;
            if (state != BUFFER_STATE_REQUESTED && state != BUFFER_STATE_ATTACHED) {
                BLOGN_FAILURE_ID(sequence, "invalid state %{public}d", state);
                return SURFACE_ERROR_NO_ENTRY;
            }
        }
    }

    if (listener_ == nullptr && listenerClazz_ == nullptr) {
        CancelBuffer(sequence, bedata);
        return SURFACE_ERROR_NO_CONSUMER;
    }

    ScopedBytrace bufferIPCSend("BufferIPCSend");
    sret = DoFlushBuffer(sequence, bedata, fence, config);
    if (sret != SURFACE_ERROR_OK) {
        return sret;
    }
    BLOGN_SUCCESS_ID(sequence, "flush");

    if (sret == SURFACE_ERROR_OK) {
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
    ss << "/data/dumpimage-" << getpid() << "-" << name_ << "-" << nowVal << ".raw";

    sptr<SurfaceBufferImpl> &buffer = bufferQueueCache_[sequence].buffer;
    std::ofstream rawDataFile(ss.str(), std::ofstream::binary);
    if (!rawDataFile.good()) {
        BLOGE("open failed: (%{public}d)%{public}s", errno, strerror(errno));
        return;
    }
    rawDataFile.write(static_cast<const char *>(buffer->GetVirAddr()), buffer->GetSize());
    rawDataFile.close();
}

SurfaceError BufferQueue::DoFlushBuffer(int32_t sequence, const BufferExtraData &bedata,
                                        int32_t fence, const BufferFlushConfig &config)
{
    ScopedBytrace func(__func__);
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (bufferQueueCache_[sequence].isDeleting) {
        DeleteBufferInCache(sequence);
        BLOGN_SUCCESS_ID(sequence, "delete");
        return SURFACE_ERROR_OK;
    }

    bufferQueueCache_[sequence].state = BUFFER_STATE_FLUSHED;
    dirtyList_.push_back(sequence);
    bufferQueueCache_[sequence].buffer->SetExtraData(bedata);
    bufferQueueCache_[sequence].fence = fence;
    bufferQueueCache_[sequence].damage = config.damage;

    // api flush
    auto sret = bufferManager_->FlushCache(bufferQueueCache_[sequence].buffer);
    if (sret != SURFACE_ERROR_OK) {
        BLOGN_FAILURE_ID_API(sequence, FlushCache, sret);
        return sret;
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
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::AcquireBuffer(sptr<SurfaceBufferImpl> &buffer,
                                        int32_t &fence, int64_t &timestamp, Rect &damage)
{
    ScopedBytrace func(__func__);
    // dequeue from dirty list
    std::lock_guard<std::mutex> lockGuard(mutex_);
    SurfaceError ret = PopFromDirtyList(buffer);
    if (ret == SURFACE_ERROR_OK) {
        int32_t sequence = buffer->GetSeqNum();
        if (isShared_ == false && bufferQueueCache_[sequence].state != BUFFER_STATE_FLUSHED) {
            BLOGNW("Warning [%{public}d], Reason: state is not BUFFER_STATE_FLUSHED", sequence);
        }
        bufferQueueCache_[sequence].state = BUFFER_STATE_ACQUIRED;

        fence = bufferQueueCache_[sequence].fence;
        timestamp = bufferQueueCache_[sequence].timestamp;
        damage = bufferQueueCache_[sequence].damage;

        BLOGNI("Success [%{public}d]", sequence);
        BLOGN_SUCCESS_ID(sequence, "acquire");
    } else if (ret == SURFACE_ERROR_NO_BUFFER) {
        BLOGN_FAILURE("there is no dirty buffer");
    }

    return ret;
}

SurfaceError BufferQueue::ReleaseBuffer(sptr<SurfaceBufferImpl> &buffer, int32_t fence)
{
    ScopedBytrace func(__func__);
    int32_t sequence = buffer->GetSeqNum();
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
            BLOGN_FAILURE_ID(sequence, "not find in cache");
            return SURFACE_ERROR_NO_ENTRY;
        }

        if (isShared_ == false) {
            auto &state = bufferQueueCache_[sequence].state;
            if (state != BUFFER_STATE_ACQUIRED && state != BUFFER_STATE_ATTACHED) {
                BLOGN_FAILURE_ID(sequence, "invalid state");
                return SURFACE_ERROR_NO_ENTRY;
            }
        }
    }

    if (onBufferRelease != nullptr) {
        ScopedBytrace func("OnBufferRelease");
        sptr<SurfaceBuffer> buf = buffer;
        BLOGNI("onBufferRelease start");
        auto sret = onBufferRelease(buf);
        BLOGNI("onBufferRelease end return %{public}s", SurfaceErrorStr(sret).c_str());

        if (sret == SURFACE_ERROR_OK) {
            return sret;
        }
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    bufferQueueCache_[sequence].state = BUFFER_STATE_RELEASED;
    bufferQueueCache_[sequence].fence = fence;

    if (bufferQueueCache_[sequence].isDeleting) {
        DeleteBufferInCache(sequence);
        BLOGN_SUCCESS_ID(sequence, "delete");
    } else {
        freeList_.push_back(sequence);
        BLOGN_SUCCESS_ID(sequence, "push to free list");
    }

    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::AllocBuffer(sptr<SurfaceBufferImpl> &buffer,
    const BufferRequestConfig &config)
{
    ScopedBytrace func(__func__);
    buffer = new SurfaceBufferImpl();
    int32_t sequence = buffer->GetSeqNum();

    SurfaceError ret = bufferManager_->Alloc(config, buffer);
    if (ret != SURFACE_ERROR_OK) {
        BLOGN_FAILURE_ID_API(sequence, Alloc, ret);
        return ret;
    }

    if (buffer == nullptr) {
        BLOGN_FAILURE_ID_RET(sequence, SURFACE_ERROR_NULLPTR);
    }

    BufferElement ele = {
        .buffer = buffer,
        .state = BUFFER_STATE_REQUESTED,
        .isDeleting = false,
        .config = config,
        .fence = -1
    };

    bufferQueueCache_[sequence] = ele;

    ret = bufferManager_->Map(buffer);
    if (ret == SURFACE_ERROR_OK) {
        BLOGN_SUCCESS_ID(sequence, "Map");
        return SURFACE_ERROR_OK;
    }

    SurfaceError freeRet = bufferManager_->Free(buffer);
    if (freeRet != SURFACE_ERROR_OK) {
        BLOGN_FAILURE_ID(sequence, "Map failed, Free failed");
    } else {
        BLOGN_FAILURE_ID(sequence, "Map failed, Free success");
    }

    return ret;
}

SurfaceError BufferQueue::FreeBuffer(sptr<SurfaceBufferImpl> &buffer)
{
    BLOGND("Free [%{public}d]", buffer->GetSeqNum());
    buffer->SetEglData(nullptr);
    bufferManager_->Unmap(buffer);
    bufferManager_->Free(buffer);
    return SURFACE_ERROR_OK;
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

SurfaceError BufferQueue::AttachBuffer(sptr<SurfaceBufferImpl> &buffer)
{
    ScopedBytrace func(__func__);
    if (isShared_) {
        BLOGN_FAILURE_RET(SURFACE_ERROR_INVALID_OPERATING);
    }

    if (buffer == nullptr) {
        BLOGN_FAILURE_RET(SURFACE_ERROR_NULLPTR);
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
    int32_t usedSize = GetUsedSize();
    int32_t queueSize = GetQueueSize();
    if (usedSize >= queueSize) {
        int32_t freeSize = dirtyList_.size() + freeList_.size();
        if (freeSize >= usedSize - queueSize + 1) {
            DeleteBuffers(usedSize - queueSize + 1);
            bufferQueueCache_[sequence] = ele;
            BLOGN_SUCCESS_ID(sequence, "release");
            return SURFACE_ERROR_OK;
        } else {
            BLOGN_FAILURE_RET(SURFACE_ERROR_OUT_OF_RANGE);
        }
    } else {
        bufferQueueCache_[sequence] = ele;
        BLOGN_SUCCESS_ID(sequence, "no release");
        return SURFACE_ERROR_OK;
    }
}

SurfaceError BufferQueue::DetachBuffer(sptr<SurfaceBufferImpl> &buffer)
{
    ScopedBytrace func(__func__);
    if (isShared_) {
        BLOGN_FAILURE_RET(SURFACE_ERROR_INVALID_OPERATING);
    }

    if (buffer == nullptr) {
        BLOGN_FAILURE_RET(SURFACE_ERROR_NULLPTR);
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    int32_t sequence = buffer->GetSeqNum();
    if (bufferQueueCache_.find(sequence) == bufferQueueCache_.end()) {
        BLOGN_FAILURE_ID(sequence, "not find in cache");
        return SURFACE_ERROR_NO_ENTRY;
    }

    if (bufferQueueCache_[sequence].state == BUFFER_STATE_REQUESTED) {
        BLOGN_SUCCESS_ID(sequence, "requested");
    } else if (bufferQueueCache_[sequence].state == BUFFER_STATE_ACQUIRED) {
        BLOGN_SUCCESS_ID(sequence, "acquired");
    } else {
        BLOGN_FAILURE_ID_RET(sequence, SURFACE_ERROR_NO_ENTRY);
    }

    bufferQueueCache_.erase(sequence);
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::SetQueueSize(uint32_t queueSize)
{
    if (isShared_ == true && queueSize != 1) {
        BLOGN_INVALID("shared queue, size must be 1");
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (queueSize <= 0) {
        BLOGN_INVALID("queue size (%{public}d) <= 0", queueSize);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (queueSize > SURFACE_MAX_QUEUE_SIZE) {
        BLOGN_INVALID("queue size (%{public}d) > %{public}d", queueSize, SURFACE_MAX_QUEUE_SIZE);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    DeleteBuffers(queueSize_ - queueSize);
    queueSize_ = queueSize;

    BLOGN_SUCCESS("queue size: %{public}d", queueSize);
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::GetName(std::string &name)
{
    name = name_;
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::RegisterConsumerListener(sptr<IBufferConsumerListener> &listener)
{
    listener_ = listener;
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::RegisterConsumerListener(IBufferConsumerListenerClazz *listener)
{
    listenerClazz_ = listener;
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::UnregisterConsumerListener()
{
    listener_ = nullptr;
    listenerClazz_ = nullptr;
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::RegisterReleaseListener(OnReleaseFunc func)
{
    onBufferRelease = func;
    return SURFACE_ERROR_OK;
}

SurfaceError BufferQueue::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    if (width <= 0 || width > SURFACE_MAX_WIDTH) {
        BLOGN_INVALID("defaultWidth (0, %{public}d], now is %{public}d", SURFACE_MAX_WIDTH, width);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (height <= 0 || height > SURFACE_MAX_HEIGHT) {
        BLOGN_INVALID("defaultHeight (0, %{public}d], now is %{public}d", SURFACE_MAX_HEIGHT, height);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    defaultWidth = width;
    defaultHeight = height;
    return SURFACE_ERROR_OK;
}

int32_t BufferQueue::GetDefaultWidth()
{
    return defaultWidth;
}

int32_t BufferQueue::GetDefaultHeight()
{
    return defaultHeight;
}

SurfaceError BufferQueue::SetDefaultUsage(uint32_t usage)
{
    defaultUsage = usage;
    return SURFACE_ERROR_OK;
}

uint32_t BufferQueue::GetDefaultUsage()
{
    return defaultUsage;
}

SurfaceError BufferQueue::CleanCache()
{
    DeleteBuffers(queueSize_);
    return SURFACE_ERROR_OK;
}
}; // namespace OHOS
