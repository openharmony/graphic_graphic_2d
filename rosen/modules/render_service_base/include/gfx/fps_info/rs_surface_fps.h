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

#ifndef RENDER_SERVICE_BASE_GFX_FPS_INFO_RS_SURFACE_FPS_H
#define RENDER_SERVICE_BASE_GFX_FPS_INFO_RS_SURFACE_FPS_H

#include <string>
#include <mutex>
#include <array>
#include <sstream>
#include "common/rs_common_def.h"

namespace OHOS::Rosen {

struct FPSStat {
    uint64_t flushTime;
    uint64_t presentTime;
    uint32_t seqNum;
};

class RSB_EXPORT RSSurfaceFps {
public:
    RSSurfaceFps(std::string name) : name_(name) {};
    bool RecordFlushTime(uint64_t vsyncId, uint64_t timestamp);
    bool RecordPresentFd(uint64_t vsyncId, int32_t presentFd);
    bool RecordPresentTime(int32_t presentFd, uint64_t timestamp);
    void Dump(std::string& result);
    void ClearDump();
    const std::string& GetName() const {
        return name_;
    }

private:
    static constexpr int FRAME_RECORDS_NUM = 384;
    // record self drawing layer timestamp
    std::array<FPSStat, FRAME_RECORDS_NUM> presentTimeRecords_ {};
    uint32_t count_ = 0;
    std::mutex mutex_;
    std::string name_;

    bool GetFlushTimeStampWithPresentFd(int32_t presentFd, uint64_t &flushTime);
    bool GetFlushTimeStampWithVsyncId(uint64_t vsyncId, uint64_t &flushTime);
    static const uint32_t MAX_VSYNC_ID_MAP_SIZE = 8; // the number of data in the map should not exceed 8 as RS runs
    static const uint32_t MAX_PRESENT_FD_MAP_SIZE = 8; // the number of data in the map should not exceed 8 as RS runs
    std::mutex vsyncIdMapMtx_;
    std::unordered_map<uint32_t, int64_t> vsyncIdWithFlushTimeMap_ {};
    std::mutex presentFdMapMtx_;
    std::unordered_map<int32_t, uint64_t> presentFdWithFlushTimeMap_ {};
};
}
#endif