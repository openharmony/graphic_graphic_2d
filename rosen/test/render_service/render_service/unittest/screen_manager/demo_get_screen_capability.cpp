/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <iostream>
#include <vector>
#include "transaction/rs_interfaces.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/screen_types.h"

using namespace OHOS::Rosen;

// ScreenInterfaceType to string
const char* ScreenInterfaceTypeToString(ScreenInterfaceType type)
{
    switch (type) {
        case DISP_INTF_UNKNOW:
            return "UNKNOW";
        case DISP_INTF_HDMI:
            return "HDMI";
        case DISP_INTF_LCD:
            return "LCD";
        case DISP_INTF_BT1120:
            return "BT1120";
        case DISP_INTF_BT656:
            return "BT656";
        case DISP_INTF_YPBPR:
            return "YPBPR";
        case DISP_INTF_RGB:
            return "RGB";
        case DISP_INTF_CVBS:
            return "CVBS";
        case DISP_INTF_SVIDEO:
            return "SVIDEO";
        case DISP_INTF_VGA:
            return "VGA";
        case DISP_INTF_MIPI:
            return "MIPI";
        case DISP_INTF_PANEL:
            return "PANEL";
        case DISP_INTF_DP:
            return "DP";
        case DISP_INTF_EDP:
            return "EDP";
        case DISP_INTF_GPMI:
            return "GPMI";
        case DISP_INVALID:
            return "INVALID";
        default:
            return "UNKNOWN";
    }
}

int main()
{
    RSInterfaces& rsIf = RSInterfaces::GetInstance();

    // 1. get all screen ID
    std::vector<ScreenId> screenIds = rsIf.GetAllScreenIds();
    std::cout << "GetAllScreenIds count: " << screenIds.size() << std::endl;

    // 2. add INVALID_SCREEN_ID
    screenIds.push_back(INVALID_SCREEN_ID);
    std::cout << "After appending INVALID_SCREEN_ID, count: " << screenIds.size() << std::endl;

    // 3. tet screenId GetScreenCapability and print result
    for (ScreenId id : screenIds) {
        std::cout << "\n===== Screen ID: " << id << " =====" << std::endl;

        RSScreenCapability capability = rsIf.GetScreenCapability(id);

        std::cout << "Name: " << capability.GetName() << std::endl;
        std::cout << "Type: " << ScreenInterfaceTypeToString(capability.GetType())
                  << " (" << static_cast<int>(capability.GetType()) << ")" << std::endl;
        std::cout << "PhyWidth: " << capability.GetPhyWidth() << std::endl;
        std::cout << "PhyHeight: " << capability.GetPhyHeight() << std::endl;
    }

    std::cout << "\nDemo finished." << std::endl;
    return 0;
}
