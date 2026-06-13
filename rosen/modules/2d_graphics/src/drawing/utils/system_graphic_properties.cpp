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

#include <parameter.h>
#include <parameters.h>
#include <set>
#include "utils/system_properties.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
GpuApiType SystemProperties::GetSystemGraphicGpuType()
{
    int systemGraphicGpuApiType = std::atoi(system::GetParameter("persist.sys.graphic.GpuApitype", "0").c_str());
    if (systemGraphicGpuApiType == 1) {
        return GpuApiType::VULKAN;
    }

    if (systemGraphicGpuApiType == 2) { // 2 is ddgr type
        // restartParameter： "1" means has updated parameters but not restarted, it will revert to "0" once restarted.
        // cloudParameter： "1" means has found the switch to disable DDGR, while "0" means it was not.
        int restartParameter = std::atoi(system::GetParameter("debug.graphic.cloudpushrestart", "0").c_str());
        int cloudParameter = std::atoi(system::GetParameter("persist.rosen.disableddgr.enabled", "0").c_str());
        if (cloudParameter == 1 && restartParameter == 0) {
            LOGD("SystemProperties::GetSystemGraphicGpuType: disable DDGR successfully.");
            return GpuApiType::VULKAN;
        }
#ifdef ENABLE_DDGR_OPTIMIZE
        LOGD("Enable DDGR");
        return GpuApiType::DDGR;
#else
        return GpuApiType::VULKAN;
#endif
    }
    return GpuApiType::OPENGL;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS