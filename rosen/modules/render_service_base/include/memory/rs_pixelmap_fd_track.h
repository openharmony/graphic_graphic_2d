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
#include <set>

#include "image_type.h"

#include "common/rs_common_def.h"
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPixelMapFdTrack {
public:
    RSPixelMapFdTrack() = default;
    ~RSPixelMapFdTrack() = default;

    void AddFdRecord(int32_t pid, const void* addr, Media::AllocatorType allocType, uint64_t uniqueId = 0);
    void RemoveFdRecord(int32_t pid, const void* addr, Media::AllocatorType allocType);
    bool CheckFdRecordAndKillProcess(int32_t pid);

private:
    DISALLOW_COPY_AND_MOVE(RSPixelMapFdTrack);

    void ClearFdRecordByPid(int32_t pid);
    void CheckFdRecordAndGetPidsToKill(std::unordered_map<int32_t, int32_t>& pidsToKill, int32_t pid) const;
    bool CheckFdCountByPid(int32_t pid, int32_t& fdCount) const;
    bool CheckFdTotal(int32_t& topFdCountPid, int32_t& topFdCount) const;
    void GetTopFdCountPid(int32_t& topFdCountPid, int32_t& topFdCount) const;
    void CheckFdWarningAndReport();
    void GetTopPixelMapsInfo(std::string& topPixelMapsInfo);
    void UpdateUniqueIdStat(uint64_t uniqueId, int32_t pid, int32_t delta);
    void RemoveUniqueIdStat(uint64_t uniqueId, int32_t pid);

    static bool KillProcessByPid(int32_t pid, const std::string& reason);
    void ReportFdOverLimit(int32_t pid, const std::string& type, int32_t fdThreshold, int32_t fdNum);
    static void WriteFdMemInfoToFile(const std::string& fdMemInfoPath, const std::string& fdMemInfo);
    static void ReadAshmemInfoFromFile(const std::string& ashmemInfoPath, std::string& ashmemInfo, int32_t pid);
    static void ReadDmaBufInfoFromFile(const std::string& dmaBufInfoPath, std::string& dmaBufInfo, int32_t pid);
    static void FilterAshmemInfoByPid(std::string& out, const std::string& info, int32_t pid);
    static void FilterDmaBufInfoByPid(std::string& out, const std::string& info, int32_t pid);

    static constexpr int32_t MAX_APP_FD = 29000; // the maximum fd count held by a single application
    static constexpr int32_t MAX_TOTAL_FD = 29000; // the maximum fd total held by all applications
    static constexpr int32_t WARNING_THRESHOLD = 20000; // warning threshold for fd total
    static constexpr int64_t WARNING_RATE_LIMIT_SECONDS = 600; // 10 minutes rate limit per pid
    static constexpr int32_t TOP_DUMP_COUNT = 10; // max uniqueId entries in the top pixelmaps section

    // entry of topUniqueIdTracker_: a uniqueId always belongs to one pid, so the pid of an entry
    // never changes and doubles as the second ordering priority of TopUniqueIdOrder
    struct TopEntry {
        int32_t fdCount = 0;
        uint64_t uniqueId = 0;
        int32_t pid = 0;
    };
    // orders TopEntry the way GetTopPixelMapsInfo prints them: larger fdCount first, then larger
    // pid first (same tie rule as GetTopFdCountPid), then smaller uniqueId first
    struct TopUniqueIdOrder {
        bool operator()(const TopEntry& lhs, const TopEntry& rhs) const
        {
            if (lhs.fdCount != rhs.fdCount) {
                return lhs.fdCount > rhs.fdCount;
            }
            if (lhs.pid != rhs.pid) {
                return lhs.pid > rhs.pid;
            }
            return lhs.uniqueId < rhs.uniqueId;
        }
    };

    int32_t fdTotal_ = 0;
    std::unordered_map<int32_t, int32_t> fdCountTracker_; // fd count of each pid
    // pixelmap addr to RSImage uniqueId tag of each pid, addr is fd number and may repeat across pids.
    // keyed by addr because RemoveFdRecord only knows addr, so a uniqueId ordered container would
    // downgrade removal to a full scan and lose duplicate addr detection
    std::unordered_map<int32_t, std::unordered_map<const void*, uint64_t>> addrTracker_;
    // fd count of each tagged uniqueId, maintained incrementally in Add/Remove/Clear paths
    std::unordered_map<uint64_t, int32_t> pixelMapFdCountTracker_;
    // updatable priority queue over pixelMapFdCountTracker_ entries ordered by TopUniqueIdOrder, so
    // GetTopPixelMapsInfo walks self contained top entries without scan and sort
    std::set<TopEntry, TopUniqueIdOrder> topUniqueIdTracker_;
    std::unordered_map<int32_t, int64_t> lastWarningTime_; // pid -> steady_clock nanoseconds of last warning report
    mutable std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_PIXELMAP_FD_TRACK
