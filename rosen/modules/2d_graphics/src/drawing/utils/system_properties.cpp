/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

#if defined (ACE_ENABLE_GL) && defined (ACE_ENABLE_VK)
static bool VulkanEnabled()
{
    if (!((system::GetParameter("const.gpu.vendor", "0").compare("higpu.v200") == 0) &&
          (system::GetParameter("const.build.product", "0").compare("ALN") == 0))) {
        return false;
    }

    if (std::atoi(system::GetParameter(
        "persist.sys.graphic.GpuApitype", "-1").c_str()) == (-1)) { // -1 is invalid type
        return true;
    }
    if (std::atoi(system::GetParameter("persist.sys.graphic.GpuApitype", "0").c_str()) == 0) {
        return false;
    }
    return true;
}
#endif

#if defined (ACE_ENABLE_GL) && defined (ACE_ENABLE_VK)
const bool SystemProperties::aceVulkanEnabled_ = VulkanEnabled();
#elif defined (ACE_ENABLE_GL)
const bool SystemProperties::aceVulkanEnabled_ = false;
#else
const bool SystemProperties::aceVulkanEnabled_ = true;
#endif

#if defined (RS_ENABLE_GL) && defined (RS_ENABLE_VK)
const bool SystemProperties::rsVulkanEnabled_ = VulkanEnabled();
#elif defined (RS_ENABLE_GL)
const bool SystemProperties::rsVulkanEnabled_ = false;
#else
const bool SystemProperties::rsVulkanEnabled_ = true;
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS