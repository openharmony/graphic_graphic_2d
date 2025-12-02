/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gfx/fps_info/rs_surface_fps.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
bool RSSurfaceFps::RecordFlushTime(uint64_t vsyncId, uint64_t timestamp)
{
    std::unique_lock<std::mutex> lock(vsyncIdMapMtx_);
    // too much redundant data, clean up all data and re record
    if (vsyncIdWithFlushTimeMap_.size() >= MAX_VSYNC_ID_MAP_SIZE) {
        vsyncIdWithFlushTimeMap_.clear();
        RS_LOGW("RecordFlushTime: vsyncId exceeds the maximum number");
    }
    vsyncIdWithFlushTimeMap_[vsyncId] = timestamp;
    RS_TRACE_NAME_FMT("RecordFlushTime vsyncid:%llu, timestamp:%llu", vsyncId, timestamp);
    return true;
}
bool RSSurfaceFps::GetFlushTimeStampWithVsyncId(uint64_t vsyncId, uint64_t &flushTime)
{
    std::unique_lock<std::mutex> lock(vsyncIdMapMtx_);
    auto iter = vsyncIdWithFlushTimeMap_.find(vsyncId);
    if (iter == vsyncIdWithFlushTimeMap_.end()) {
        return false;
    }
    flushTime = iter->second;
    vsyncIdWithFlushTimeMap_.erase(iter);
    return true;
}
bool RSSurfaceFps::RecordPresentFd(uint64_t vsyncId, int32_t presentFd)
{
    RS_TRACE_NAME_FMT("RecordPresentFd vsyncid:%llu, presentFd:%d", vsyncId, presentFd);
    uint64_t flushTimeStamp;
    if (!GetFlushTimeStampWithVsyncId(vsyncId, flushTimeStamp)) {
        return false;
    }
    std::unique_lock<std::mutex> lock(presentFdMapMtx_);
    // too much redundant data, clean up all data and re record
    if (presentFdWithFlushTimeMap_.size() >= MAX_PRESENT_FD_MAP_SIZE) {
        presentFdWithFlushTimeMap_.clear();
        RS_LOGW("RecordPresentFd: presentFd exceeds the maximum number");
    }
    presentFdWithFlushTimeMap_[presentFd] = flushTimeStamp;
    return true;
}
bool RSSurfaceFps::GetFlushTimeStampWithPresentFd(int32_t presentFd, uint64_t &flushTime)
{
    std::unique_lock<std::mutex> lock(presentFdMapMtx_);
    auto iter = presentFdWithFlushTimeMap_.find(presentFd);
    if (iter == presentFdWithFlushTimeMap_.end()) {
        return false;
    }
    flushTime = iter->second;
    presentFdWithFlushTimeMap_.erase(iter);
    return true;
}
bool RSSurfaceFps::RecordPresentTime(int32_t presentFd, uint64_t timestamp)
{
    uint64_t flushTimeStamp;
    if (!GetFlushTimeStampWithPresentFd(presentFd, flushTimeStamp)) {
        return false;
    }
    RS_TRACE_NAME_FMT("RSSurfaceFps::RecordPresentTime flushTimeStamp:%llu, timestamp:%llu", flushTimeStamp, timestamp);
    std::unique_lock<std::mutex> lock(mutex_);
    presentTimeRecords_[count_].presentTime = timestamp;
    presentTimeRecords_[count_].flushTime = flushTimeStamp;
    presentTimeRecords_[count_].seqNum = 0;
    count_ = (count_ + 1) % FRAME_RECORDS_NUM;
    return true;
}

void RSSurfaceFps::Dump(std::string& result)
{
    std::ostringstream oss;
    std::unique_lock<std::mutex> lock(mutex_);
    const uint32_t offset = count_;
    for (uint32_t i = 0; i < FRAME_RECORDS_NUM; i++) {
        uint32_t order = (offset + FRAME_RECORDS_NUM - i - 1) % FRAME_RECORDS_NUM;
        oss << presentTimeRecords_[order].flushTime << ":" << presentTimeRecords_[order].presentTime << "\n";
    }
    result += oss.str();
}

void RSSurfaceFps::ClearDump()
{
    std::unique_lock<std::mutex> lock(mutex_);
    FPSStat defaultFPSStat = {0, 0};
    presentTimeRecords_.fill(defaultFPSStat);
}
}