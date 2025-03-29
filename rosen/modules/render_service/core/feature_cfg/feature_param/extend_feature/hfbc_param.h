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

#ifndef HFBC_PARAM_H
#define HFBC_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class HFBCParam : public FeatureParam {
public:
    HFBCParam() = default;
    ~HFBCParam() = default;

    static const std::unordered_map<std::string, std::string>& GetHfbcConfigMap();

protected:
    static void SetHfbcConfigForApp(const std::string& appName, const std::string& val);

private:
    // <"pkgName", "1">
    inline static std::unordered_map<std::string, std::string> hfbcConfig_ = {};;

    friend class HFBCParamParse;
};
} // namespace OHOS::Rosen
#endif // HFBC_PARAM_H
