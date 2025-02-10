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

namespace OHOS::Rosen {
bool RSSurfaceFps::RecordPresentTime(uint64_t timestamp, uint32_t seqNum)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (seqNum == presentTimeRecords_[(count_ - 1 + FRAME_RECORDS_NUM) % FRAME_RECORDS_NUM].seqNum) {
        return false;
    }
    RS_TRACE_NAME_FMT("RSSurfaceFps::RecordPresentTime timestamp:%llu", timestamp);
    presentTimeRecords_[count_].presentTime = timestamp;
    presentTimeRecords_[count_].seqNum = seqNum;
    count_ = (count_ + 1) % FRAME_RECORDS_NUM;
    return true;
}

void RSSurfaceFps::Dump(std::string& result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    const uint32_t offset = count_;
    for (uint32_t i = 0; i < FRAME_RECORDS_NUM; i++) {
        uint32_t order = (offset + FRAME_RECORDS_NUM - i - 1) % FRAME_RECORDS_NUM;
        result += std::to_string(presentTimeRecords_[order].presentTime) + "\n";
    }
}

void RSSurfaceFps::ClearDump()
{
    std::unique_lock<std::mutex> lock(mutex_);
    FPSStat defaultFPSStat = {0, 0};
    presentTimeRecords_.fill(defaultFPSStat);
}
}