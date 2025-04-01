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

    std::string GetRSWatchPoint() const;
    static bool IsReclaimEnabled();
    static int GetRSCacheLimitsResourceSize();

protected:
    void SetRSWatchPoint(std::string rsWatchPoint);
    static void SetReclaimEnabled(bool isEnabled);
    static void SetRSCacheLimitsResourceSize(int rsCacheLimitsResourceSize);

private:
    std::string rsWatchPoint_ = "";
    inline static bool isReclaimEnabled_ = false;
    inline static int rsCacheLimitsResourceSize_ = 0;

    friend class MEMParamParse;
};
} // namespace OHOS::Rosen
#endif // MEM_PARAM_H