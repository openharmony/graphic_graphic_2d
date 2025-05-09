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

#ifndef OPINC_PARAM_H
#define OPINC_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class OPIncParam : public FeatureParam {
public:
    OPIncParam() = default;
    ~OPIncParam() = default;

    static bool IsOPIncEnable();
    static int GetCacheWidthThresholdPercentValue();

protected:
    static void SetOPIncEnable(bool isEnable);
    static void SetCacheWidthThresholdPercentValue(int cacheWidthThresholdPercentValue);

private:
    inline static int isOPIncEnable_ = true;
    inline static int cacheWidthThresholdPercentValue_ = 150;

    friend class OPIncParamParse;
};
} // namespace OHOS::Rosen
#endif // OPINC_PARAM_H