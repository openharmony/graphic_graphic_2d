/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include "transaction/rs_interfaces.h"

using namespace OHOS::Rosen;
using namespace std;

int main()
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();
    if (screenId == INVALID_SCREEN_ID) {
        cout << "Default Screen Id is INVALID_SCREEN_ID !" << std::endl;
    }
    std::cout << "ScreenID : " << screenId << endl;

    auto screenData = rsInterfaces.GetScreenData(screenId);

    auto screenCapability = screenData.GetCapability();

    cout << "Name : " << screenCapability.GetName() << endl;
    cout << "PhiWidth : " << screenCapability.GetPhyWidth() << endl;
    cout << "PhiHeight : " << screenCapability.GetPhyHeight() << endl;

    auto modeInfo = screenData.GetActivityModeInfo();
    cout << "RefreshRate : " << modeInfo.GetScreenRefreshRate() << endl;
    cout << "ScreenHeight : " << modeInfo.GetScreenHeight() << endl;
    cout << "ScreenWidth : " << modeInfo.GetScreenWidth() << endl;
}