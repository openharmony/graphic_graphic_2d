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

#include "render/rs_filter_cache_memory_controller.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

RSFilterCacheMemoryController& RSFilterCacheMemoryController::Instance()
{
    static RSFilterCacheMemoryController instance;
    return instance;
}

void RSFilterCacheMemoryController::ReplaceFilterCacheMem(int64_t oldCacheMem, int64_t newCacheMem)
{
    if (oldCacheMem < 0 || newCacheMem < 0) {
        RS_LOGW("RSFilterCacheMemoryController::ReplaceFilterCacheMem: invalid oldCacheMem:%{public}" PRId64
            ",newCacheMem:%" PRId64 "", oldCacheMem, newCacheMem);
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (oldCacheMem == 0 && newCacheMem > 0) {
        cacheMem_ += newCacheMem;
        cacheCount_++;
        RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheMemoryController::ReplaceFilterCacheMem, added CacheMem:%" PRId64
            ", current CacheMem:%" PRId64 ", current count:%d", newCacheMem, cacheMem_, cacheCount_);
        return;
    }
    cacheMem_ = cacheMem_ - oldCacheMem + newCacheMem;
    if (cacheMem_ < 0) {
        RS_LOGW("RSFilterCacheMemoryController::ReplaceFilterCacheMem:"
            "current CacheMem:%{public}" PRId64 ", current count:%{public}d", cacheMem_, cacheCount_);
        cacheMem_ = 0;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheMemoryController::ReplaceFilterCacheMem, "
        "oldCacheMem:%" PRId64 ", newCacheMem:%" PRId64 ", current CacheMem:%" PRId64 ", current count:%d",
        oldCacheMem, newCacheMem, cacheMem_, cacheCount_);
}

void RSFilterCacheMemoryController::ReduceFilterCacheMem(int64_t cacheMem)
{
    if (cacheMem < 0) {
        RS_LOGW("RSFilterCacheMemoryController::ReduceFilterCacheMem: invalid cacheMem:%{public}" PRId64 "", cacheMem);
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    cacheMem_ -= cacheMem;
    cacheCount_--;

    if (cacheMem_ < 0 || cacheCount_ < 0) {
        RS_LOGW("RSFilterCacheMemoryController::ReduceFilterCacheMem: reduced CacheMem:%{public}" PRId64 ", "
            "current CacheMem:%{public}" PRId64", current count:%{public}d", cacheMem, cacheMem_, cacheCount_);
        cacheMem_ = 0;
        cacheCount_ = 0;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheMemoryController::ReduceFilterCacheMem, reduced CacheMem:%" PRId64 ", "
        "current CacheMem:%" PRId64 ", current count:%d", cacheMem, cacheMem_, cacheCount_);
}

bool RSFilterCacheMemoryController::IsFilterCacheMemExceedThreshold()
{
    if (!RSSystemProperties::GetFilterCacheMemThresholdEnabled()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t threshold = static_cast<int64_t>(screenRectInfo_.GetWidth()) *
        static_cast<int64_t>(screenRectInfo_.GetHeight()) * SCREEN_RATIO;
    if (cacheMem_ >= threshold) {
        RS_TRACE_NAME_FMT("RSFilterCacheMemExceedThreshold,"
            " CacheMem:%" PRId64", count:%d, threshold:%" PRId64"", cacheMem_, cacheCount_, threshold);
        return true;
    }
    return false;
}

void RSFilterCacheMemoryController::SetScreenRectInfo(Drawing::RectI info)
{
    screenRectInfo_ = info;
}

} // namespace Rosen
} // namespace OHOS