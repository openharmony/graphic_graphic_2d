/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_FILTER_CACHE_MEMORY_CONTROLLER_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_FILTER_CACHE_MEMORY_CONTROLLER_H

#include <mutex>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
constexpr float SCREEN_RATIO = 4.5f;
class RSB_EXPORT RSFilterCacheMemoryController {
public:
    static RSFilterCacheMemoryController& Instance();
    void ReduceFilterCacheMem(int64_t cacheMem);
    void ReplaceFilterCacheMem(int64_t oldCacheMem, int64_t newCacheMem);
    /**
     * @brief Check if filter cache memory exceeds threshold
     * @return true if cache memory exceeds threshold, false otherwise
     */
    bool IsFilterCacheMemExceedThreshold();
    void SetScreenRectInfo(Drawing::RectI info);

private:
    RSFilterCacheMemoryController() = default;
    ~RSFilterCacheMemoryController() = default;
    RSFilterCacheMemoryController(const RSFilterCacheMemoryController&) = delete;
    RSFilterCacheMemoryController(RSFilterCacheMemoryController&&) = delete;
    RSFilterCacheMemoryController& operator=(const RSFilterCacheMemoryController&) = delete;
    RSFilterCacheMemoryController& operator=(RSFilterCacheMemoryController&&) = delete;

    std::mutex mutex_;
    int64_t cacheMem_ = 0;
    int32_t cacheCount_ = 0;
    Drawing::RectI screenRectInfo_ = { 0, 0, 0, 0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_FILTER_CACHE_MEMORY_CONTROLLER_H