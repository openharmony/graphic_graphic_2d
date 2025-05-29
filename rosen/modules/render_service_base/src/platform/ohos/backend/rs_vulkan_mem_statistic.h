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

#ifndef RS_VULKAN_MEM_STAT_H
#define RS_VULKAN_MEM_STAT_H

#include <mutex>
#include <string>

#include "memory/rs_dfx_string.h"
#include "memory/rs_tag_tracker.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

class RsVulkanMemStat {
public:
    RsVulkanMemStat() = default;
    ~RsVulkanMemStat() = default;

    void InsertResource(const std::string& name, pid_t pid, const uint64_t size)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mResources[name] = {
            .size = size,
            .pid = pid,
        };
    }

    void DeleteResource(const std::string& name)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mResources.find(name);
        if (it != mResources.end()) {
            mResources.erase(name);
        } else {
            ROSEN_LOGE("DeleteResource Error, name is invalid");
        }
    }

    void DumpMemoryStatistics(DfxString& log)
    {
        uint64_t totalSize = 0;
        uint64_t totalCount = 0;
        struct DumpInfo {
            uint64_t size = 0;
            uint64_t count = 0;
        };
        std::unordered_map<pid_t, DumpInfo> pidDumpInfoMap;
        {
            std::lock_guard<std::mutex> lock(mMutex);
            totalCount = mResources.size();
            for (const auto& it : mResources) {
                auto& dumpInfo = pidDumpInfoMap[it.second.pid];
                dumpInfo.size += it.second.size;
                ++dumpInfo.count;
                totalSize += it.second.size;
            }
        }
        log.AppendFormat("\n------------\nVulkan Memory Statistics: Count: %lu, Size: %lu\n", totalCount, totalSize);
        for (const auto& pidIt : pidDumpInfoMap) {
            log.AppendFormat("  pid: %d, count: %lu, size: %lu\n", pidIt.first, pidIt.second.count, pidIt.second.size);
        }
    }

private:
    struct MemoryInfo {
        uint64_t size = 0;
        pid_t pid = 0;
    };
    std::unordered_map<std::string, MemoryInfo> mResources;
    std::mutex mMutex;
};

}
}

#endif