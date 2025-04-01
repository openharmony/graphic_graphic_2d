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

#ifndef MEM_PARAM_H
#define MEM_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class MEMParam : public FeatureParam {
public:
    MEMParam() = default;
    ~MEMParam() = default;

    static std::string GetRSWatchPoint();
    static bool IsReclaimEnabled();
    static int GetRSCacheLimitsResourceSize();
    static bool IsDeeplyRelGpuResEnable();

protected:
    static void SetRSWatchPoint(std::string rsWatchPoint);
    static void SetReclaimEnabled(bool isEnabled);
    static void SetRSCacheLimitsResourceSize(int rsCacheLimitsResourceSize);
    static void SetDeeplyRelGpuResEnable(bool isDeeplyRelGpuResEnable);

private:
    inline static std::string rsWatchPoint_ = "";
    inline static bool isReclaimEnabled_ = false;
    inline static int rsCacheLimitsResourceSize_ = 0;
    inline static bool isDeeplyRelGpuResEnable_ = false;

    friend class MEMParamParse;
};
} // namespace OHOS::Rosen
#endif // MEM_PARAM_H