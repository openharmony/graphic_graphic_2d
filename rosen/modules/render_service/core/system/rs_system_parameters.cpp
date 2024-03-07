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
constexpr int DEFAULT_QUICK_SKIP_PREPARE_TYPE_VALUE = 3;
int ConvertToInt(const char *originValue, int defaultValue)
{
    return originValue == nullptr ? defaultValue : std::atoi(originValue);
}
bool RSSystemParameters::GetCalcCostEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.calcCost.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

bool RSSystemParameters::GetDrawingCacheEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.drawingCache.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemParameters::GetDrawingCacheEnabledDfx()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.drawingCache.enabledDfx", "0");
    int changed = 0;
    const char *enabledDfx = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enabledDfx, 0) != 0;
}

bool RSSystemParameters::GetShowRefreshRateEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.showRefreshRate.enabled", "0");
    int changed = 0;
    const char *enabled = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enabled, 0) != 0;
}

QuickSkipPrepareType RSSystemParameters::GetQuickSkipPrepareType()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.quickskipprepare.enabled", "5");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<QuickSkipPrepareType>(ConvertToInt(type, DEFAULT_QUICK_SKIP_PREPARE_TYPE_VALUE));
}

bool RSSystemParameters::GetVSyncControlEnabled()
{
    static bool vsyncControlEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.vsyncControlEnabled", "1")).c_str()) != 0;
    return vsyncControlEnabled;
}

bool RSSystemParameters::GetSystemAnimatedScenesEnabled()
{
    static bool systemAnimatedScenesEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.systemAnimatedScenesEnabled", "1")).c_str()) != 0;
    return systemAnimatedScenesEnabled;
}

bool RSSystemParameters::GetFilterCacheOcculusionEnabled()
{
    static bool filterCacheOcclusionEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheOcclusionEnabled", "1")).c_str()) != 0;
    return filterCacheOcclusionEnabled;
}

bool RSSystemParameters::GetSkipCanvasNodeOutofScreenEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.skipCanvasNodeOutofScreen.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemParameters::GetDrawingEffectRegionEnabledDfx()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.graphic.drawingEffectRegionEnabledDfx", "0");
    int changed = 0;
    const char *enableDfx = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enableDfx, 0) != 0;
}

bool RSSystemParameters::GetRenderStop()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.render.stop", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}
} // namespace Rosen
} // namespace OHOS
