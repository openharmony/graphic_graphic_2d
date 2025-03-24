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

#ifndef FILTER_CACHE_PARAM_H
#define FILTER_CACHE_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class FilterParam : public FeatureParam {
public:
    FilterParam() = default;
    ~FilterParam() = default;

    bool IsFilterCacheEnable() const;
    bool IsEffectMergeEnable() const;
    bool IsBlurAdaptiveAdjust() const;

protected:
    void SetFilterCacheEnable(bool isEnable);
    void SetEffectMergeEnable(bool isEnable);
    void SetBlurAdaptiveAdjust(bool isEnable);

private:
    bool isFilterCacheEnable_ = true;
    bool isEffectMergeEnable_ = true;
    bool isBlurAdaptiveAdjust = false;
    friend class FilterParamParse;
};
} // namespace OHOS::Rosen
#endif // FILTER_CACHE_PARAM_H