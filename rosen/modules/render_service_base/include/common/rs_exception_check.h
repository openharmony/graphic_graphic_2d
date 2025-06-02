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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_EXCEPTION_CHECK_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_EXCEPTION_CHECK_H

#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSTimer {
public:
    static inline int64_t GetNanoSeconds()
    {
        struct timespec ts;
        (void)clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * NANO + ts.tv_nsec;
    }

    static inline int64_t GetSeconds()
    {
        struct timespec ts;
        (void)clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec;
    }

    explicit RSTimer(const char* tag, const int64_t threshold): tag_(tag),
        timestamp_(GetNanoSeconds()), threshold_(threshold) {}

    ~RSTimer()
    {
        int64_t durationMs = GetDuration();
        if (durationMs > threshold_) {
            RS_LOGE("%{public}s task too long: %{public}" PRIu64 " ms", tag_, durationMs);
        }
    }

    int64_t GetDuration();

private:
    const char* tag_;
    const int64_t timestamp_;
    const int64_t threshold_;
    static constexpr int64_t NANO = 1000000000LL;
    static constexpr int64_t MILLI = 1000000LL;
};

struct RSB_EXPORT ExceptionCheck {
    void UploadRenderExceptionData();
    int32_t pid_ = 0;
    int32_t uid_ = 0;
    std::string processName_;
    int32_t exceptionCnt_ = 0;
    int64_t exceptionMoment_ = 0;
    std::string exceptionPoint_;
    bool isUpload_ = true;
};

} // namespace Rosen
} // namespace OHOS

#endif