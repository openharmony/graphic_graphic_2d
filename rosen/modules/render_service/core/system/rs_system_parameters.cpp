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

#include "rs_system_parameters.h"

#include <cstdlib>
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RSSystemParameters::GetCalcCostEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.calcCost.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

bool RSSystemParameters::GetDrawingCacheEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.drawingCache.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

bool RSSystemParameters::GetDrawingCacheEnabledDfx()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.drawingCache.enabledDfx", "0");
    int changed = 0;
    const char *enabledDfx = CachedParameterGetChanged(g_Handle, &changed);
    return std::atoi(enabledDfx) != 0;
}

QuickSkipPrepareType RSSystemParameters::GetQuickSkipPrepareType()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.quickskipprepare.enabled", "3");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<QuickSkipPrepareType>(std::atoi(type));
}

bool RSSystemParameters::GetFilterCacheOcculusionEnabled()
{
    static bool filterCacheOcclusionEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheOcclusionEnabled", "1")).c_str()) != 0;
    return filterCacheOcclusionEnabled;
}
} // namespace Rosen
} // namespace OHOS
