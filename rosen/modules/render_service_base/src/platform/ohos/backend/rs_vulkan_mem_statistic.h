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

#include "image/trace_memory_dump.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

inline const char* g_resourceCategory = "Image";
inline const char* g_resourceType = "RS_VULKAN_IMG";

class RsVulkanMemStat {
public:
    RsVulkanMemStat() = default;
    ~RsVulkanMemStat() = default;

    void InsertResource(const std::string& name, const uint64_t size)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mResources[name] = {
            .size = size,
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

    void DumpMemoryStatistics(Drawing::TraceMemoryDump *memoryDump)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (!memoryDump) {
            return;
        }
        for (auto it = mResources.begin(); it != mResources.end(); it++) {
            memoryDump->DumpNumericValue(it->first.c_str(), "size", "bytes", it->second.size);
            memoryDump->DumpStringValue(it->first.c_str(), "type", g_resourceType);
            memoryDump->DumpStringValue(it->first.c_str(), "category", g_resourceCategory);
        }
    }

private:
    struct MemoryInfo {
        uint64_t size;
    };
    std::unordered_map<std::string, MemoryInfo> mResources;
    std::mutex mMutex;
};

}
}

#endif