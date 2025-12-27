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

#ifndef SMART_CACHE_PARAM_H
#define SMART_CACHE_PARAM_H

#include <string>
#include "feature_param.h"

namespace OHOS::Rosen {
class SmartCacheParam : public FeatureParam {
public:
    SmartCacheParam() = default;
    ~SmartCacheParam() = default;

    static std::string IsEnabled();
    static std::string GetUMDPoolSize();
    static std::string GetTimeInterval();

protected:
    static void SetEnabled(std::string isEnabled);
    static void SetUMDPoolSize(std::string size);
    static void SetTimeInterval(std::string time);

private:
    inline static std::string isSmartCacheEnabled_ = "true";
    inline static std::string smartCacheUMDPoolSize_ = "50"; // MB
    inline static std::string timeInterval_ = "100"; // ms

    friend class SmartCacheParamParse;
};
} // namespace OHOS::Rosen
#endif // SMART_CACHE_PARAM_H