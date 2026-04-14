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

#ifndef RS_PIXELMAP_FD_TRACK
#define RS_PIXELMAP_FD_TRACK

#include <mutex>

#include "image_type.h"

#include "common/rs_common_def.h"
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPixelMapFdTrack {
public:
    RSPixelMapFdTrack() = default;
    ~RSPixelMapFdTrack() = default;

    void AddFdRecord(int32_t pid, const void* addr, Media::AllocatorType allocType);
    void RemoveFdRecord(int32_t pid, const void* addr, Media::AllocatorType allocType);
    bool CheckFdRecordAndKillProcess(int32_t pid);

private:
    DISALLOW_COPY_AND_MOVE(RSPixelMapFdTrack);

    void ClearFdRecordByPid(int32_t pid);
    void CheckFdRecordAndGetPidsToKill(std::unordered_map<int32_t, int32_t>& pidsToKill, int32_t pid) const;
    bool CheckFdCountByPid(int32_t pid, int32_t& fdCount) const;
    bool CheckFdTotal(int32_t& topFdCountPid, int32_t& topFdCount) const;

    static bool KillProcessByPid(int32_t pid, const std::string& reason);
    static void ReportFdOverLimit(int32_t pid);
    static void WriteFdMemInfoToFile(const std::string& fdMemInfoPath, const std::string& fdMemInfo);
    static void ReadAshmemInfoFromFile(const std::string& ashmemInfoPath, std::string& ashmemInfo, int32_t pid);
    static void ReadDmaBufInfoFromFile(const std::string& dmaBufInfoPath, std::string& dmaBufInfo, int32_t pid);
    static void FilterAshmemInfoByPid(std::string& out, const std::string& info, int32_t pid);
    static void FilterDmaBufInfoByPid(std::string& out, const std::string& info, int32_t pid);

    static constexpr int32_t MAX_APP_FD = 25000; // the maximum fd count held by a single application
    static constexpr int32_t MAX_TOTAL_FD = 29000; // the maximum fd total held by all applications

    int32_t fdTotal_ = 0;
    std::unordered_map<int32_t, int32_t> fdCountTracker_; // fd count of each pid
    std::unordered_map<int32_t, std::unordered_set<const void*>> addrTracker_; // pixelmap address set of each pid
    mutable std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_PIXELMAP_FD_TRACK
