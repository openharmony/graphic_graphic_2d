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

#ifndef PERFEVENT_H
#define PERFEVENT_H

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum COUNTER_TYPE {
    COUNTER_INVALID_TYPE = -1,
    COUNTER_FIRST_TYPE,
    COUNTER_SECOND_TYPE,
    COUNTER_THIRD_TYPE,
    COUNTER_FORTH_TYPE,
};

enum COUNTER_TIME_LIMIT {
    COUNTER_MS_FIRST_TYPE = 1000000,
    COUNTER_MS_SECOND_TYPE = 3000000,
    COUNTER_MS_THIRD_TYPE = 5000000,
    COUNTER_MS_FORTH_TYPE = 8000000,
};

struct RsTextureEvent {
    void initEvent (int32_t pid, int32_t maxBytes, int32_t budgetedBytes, int64_t allocTime,
        bool clearCache)
    {
        fPid = pid;
        fMaxBytes = maxBytes;
        fBudgetedBytes = budgetedBytes;
        fAllocTime = allocTime;
        fClearCache = clearCache;
    }
    int32_t fPid {0};
    int32_t fMaxBytes {0};
    int32_t fBudgetedBytes {0};
    int64_t fAllocTime {0};
    bool fClearCache {false};
};

struct RsBlurEvent {
    void initEvent (int32_t pid, uint16_t filterType, float blurRadius, int32_t width,
        int32_t height, int64_t blurTime)
    {
        fPid = pid;
        fFilterType = filterType;
        fBlurRadius = blurRadius;
        fWidth = width;
        fHeight = height;
        fBlurTime = blurTime;
    }
    int32_t fPid {0};
    uint16_t fFilterType {0};
    float fBlurRadius {0.0};
    int32_t fWidth {0};
    int32_t fHeight {0};
    int64_t fBlurTime {0};
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
