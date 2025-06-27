/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef HYPER_GRAPHIC_MANAGER_CORE_HGM_SCREEN_INFO_H
#define HYPER_GRAPHIC_MANAGER_CORE_HGM_SCREEN_INFO_H

#include <string>

#include "hgm_command.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

enum class HgmScreenType {
    LTPS = 0,
    LTPS1,
    LTPS2,
    LTPO = 10,
    LTPO1, // First-generation LTPO screen
    LTPO2, // Second-generation LTPO screen
    LTPO3,
};

struct HgmPanelInfo {
    int32_t panelId;
    HgmScreenType panelType;
    std::vector<int32_t> supportFps;
};

using ConvertHandle = void*;
using ConvertFuncGet = int (*)(std::string&);

class HgmScreenInfo {
public:
    static HgmScreenInfo& GetInstance();

    HgmScreenType GetScreenType(ScreenId screenId);
    bool IsLtpoType(HgmScreenType screenType);
private:
    HgmScreenInfo() = default;
    virtual ~HgmScreenInfo();
    
    void Init();
    int32_t GetPanelInfo(std::string& panelInfo);
    int32_t InitPanelTypeMap(std::string& panelInfo);

    // key: panel id
    std::unordered_map<int32_t, HgmPanelInfo> panelInfo_;

    static const std::string HGM_EXT_SO_PATH;
    static const std::string GET_PANEL_INFO_SYMBOL;
    ConvertHandle handle_ { nullptr };
    ConvertFuncGet func_ { nullptr };
};

} // namespace Rosen
} // namespace OHOS
#endif // HYPER_GRAPHIC_MANAGER_CORE_HGM_SCREEN_INFO_H
