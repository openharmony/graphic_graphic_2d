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
void RSSurfaceFps::RecordBufferTime(uint64_t flushTimestamp, uint64_t presentTimestamp)
{
    RS_TRACE_NAME_FMT("RecordBufferTime flushTimeStamp:%llu, presentTimestamp:%llu", flushTimestamp, presentTimestamp);
    std::unique_lock<std::mutex> lock(mutex_);
    presentTimeRecords_[count_].presentTime = presentTimestamp;
    presentTimeRecords_[count_].flushTime = flushTimestamp;
    presentTimeRecords_[count_].seqNum = 0;
    count_ = (count_ + 1) % FRAME_RECORDS_NUM;
    return;
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