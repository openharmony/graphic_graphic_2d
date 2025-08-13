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

#ifndef DRM_PARAM_H
#define DRM_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class DRMParam : public FeatureParam {
public:
    DRMParam() = default;
    ~DRMParam() = default;

    static bool IsDrmEnable();
    static bool IsDrmMarkAllParentBlurEnable();
    static const std::vector<std::string>& GetWhiteList();
    static const std::vector<std::string>& GetBlackList();

protected:
    static void SetDrmEnable(bool isEnable);
    static void SetDrmMarkAllParentBlurEnable(bool isEnable);
    static void AddWhiteList(const std::string& name);
    static void AddBlackList(const std::string& name);

private:
    inline static bool isDrmEnable_ = true;
    inline static bool isDrmMarkAllParentBlurEnable_ = false;
    inline static std::vector<std::string> whiteMarkBlurList_;
    inline static std::vector<std::string> blackMarkBlurList_;

    friend class DRMParamParse;
};
} // namespace OHOS::Rosen
#endif // DRM_PARAM_H