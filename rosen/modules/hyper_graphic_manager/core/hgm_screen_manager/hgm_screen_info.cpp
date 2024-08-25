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

#include "hgm_screen_info.h"

namespace OHOS {
namespace Rosen {

HgmScreenInfo& HgmScreenInfo::GetInstance()
{
    static HgmScreenInfo instance;
    return instance;
}

HgmScreenInfo::~HgmScreenInfo() { }

HgmScreenType HgmScreenInfo::GetScreenType(ScreenId screenId)
{
    // hgm warning: use GetDisplaySupportedModes instead
    return (screenId == 0) ? HgmScreenType::LTPO : HgmScreenType::LTPS;
}

bool HgmScreenInfo::IsLtpoType(HgmScreenType screenType)
{
    if (screenType >= HgmScreenType::LTPO) {
        return true;
    }
    return false;
}

void HgmScreenInfo::Init() { }

int32_t HgmScreenInfo::GetPanelInfo(std::string& panelInfo)
{
    return HGM_ERROR;
}

int32_t HgmScreenInfo::InitPanelTypeMap(std::string& panelInfo)
{
    return HGM_ERROR;
}

} // namespace Rosen
} // namespace OHOS
