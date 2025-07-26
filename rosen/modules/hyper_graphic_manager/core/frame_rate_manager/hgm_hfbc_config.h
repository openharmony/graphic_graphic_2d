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

#ifndef HYPER_GRAPHIC_MANAGER_CORE_HGM_HFBC_CONFIG_H
#define HYPER_GRAPHIC_MANAGER_CORE_HGM_HFBC_CONFIG_H

#include <string>
#include <vector>

namespace OHOS {
namespace Rosen {

class HgmHfbcConfig {
public:
    void SetHfbcConfigMap(const std::unordered_map<std::string, std::string>& hfbcConfig)
    {
        hfbcConfig_ = hfbcConfig;
    }

    void SetHfbcControlMode(bool mode)
    {
        isHfbcDisableListMode_ = mode;
    }

    void HandleHfbcConfig(const std::vector<std::string>& packageList);
private:
    // <"pkgName", "1">
    std::unordered_map<std::string, std::string> hfbcConfig_ = {};
    // true: disable list mode, false: enable list mode
    bool isHfbcDisableListMode_ = true;
    inline static int32_t curHfbcStatus_ = 0;
};

} // namespace Rosen
} // namespace OHOS
#endif // HYPER_GRAPHIC_MANAGER_CORE_HGM_HFBC_CONFIG_H
