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

int RSSystemParameters::GetDumpRSTreeCount()
{
    static CachedHandle g_Handle = CachedParameterCreate("debug.graphic.dumpRSTreeCount", "0");
    int changed = 0;
    const char *num = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(num, 0);
}

void RSSystemParameters::SetDumpRSTreeCount(int count)
{
    count = (count > 0) ? count : 0;
    system::SetParameter("debug.graphic.dumpRSTreeCount", std::to_string(count));
    RS_LOGD("RSSystemParameters::SetDumpRSTreeCount %{public}d", count);
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

bool RSSystemParameters::GetShowRefreshRateEnabled(int *changed)
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.showRefreshRate.enabled", "0");
    const char *enabled = CachedParameterGetChanged(g_Handle, changed);
    return ConvertToInt(enabled, 0) != 0;
}

QuickSkipPrepareType RSSystemParameters::GetQuickSkipPrepareType()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.quickskipprepare.enabled", "2");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<QuickSkipPrepareType>(ConvertToInt(type, DEFAULT_QUICK_SKIP_PREPARE_TYPE_VALUE));
}

RsParallelType RSSystemParameters::GetRsParallelType()
{
    static CachedHandle g_Handle = CachedParameterCreate("persist.sys.graphic.parallel.type", "0");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<RsParallelType>(ConvertToInt(type, 0));
}

RsSurfaceCaptureType RSSystemParameters::GetRsSurfaceCaptureType()
{
    if (GetRsParallelType() == RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD) {
        return RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD;
    }
    static CachedHandle g_Handle =
        CachedParameterCreate("persist.sys.graphic.surface_capture.type", "0");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<RsSurfaceCaptureType>(ConvertToInt(type, 0));
}

bool RSSystemParameters::GetVRateControlEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.vRateControl.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
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
    static CachedHandle g_Handle = CachedParameterCreate("rosen.skipCanvasNodeOutofScreen.enabled", "0");
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

bool RSSystemParameters::GetOcclusionCallBackToWMSDebugType()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.occlusion.callbacktowms.debug.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

bool RSSystemParameters::GetPrevalidateHwcNodeEnabled()
{
    static bool prevalidateHwcNodeEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.prevalidateHwcNode.Enabled", "1")).c_str()) != 0;
    return prevalidateHwcNodeEnabled;
}

bool RSSystemParameters::GetSolidLayerHwcEnabled()
{
    static bool solidLayerHwcEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.solidLayer.Enabled", "1")).c_str()) != 0;
    return solidLayerHwcEnabled;
}

bool RSSystemParameters::GetControlBufferConsumeEnabled()
{
    static bool controlBufferConsume =
        std::atoi((system::GetParameter("persist.sys.graphic.controlBufferConsume.Enabled", "1")).c_str()) != 0;
    return controlBufferConsume;
}

bool RSSystemParameters::GetHideNotchStatus()
{
    static CachedHandle g_Handle = CachedParameterCreate("persist.sys.graphic.hideNotch.status", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    if (!enable) {
        return false;
    }
    return (strcmp(enable, "2") == 0);
}

bool RSSystemParameters::GetHpaeBlurEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("debug.graphic.hpae.blur.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemParameters::GetTcacheEnabled()
{
    static bool flag = system::GetBoolParameter("persist.sys.graphic.tcache.enable", true);
    return flag;
}

bool RSSystemParameters::GetDumpCanvasDrawingNodeEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("debug.graphic.canvasDrawingEnabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

bool RSSystemParameters::IsNeedScRGBForP3(const GraphicColorGamut& currentGamut)
{
    static bool isSupportScRGBForP3_ = system::GetBoolParameter("persist.sys.graphic.scrgb.enabled", false);
    return isSupportScRGBForP3_ && (currentGamut != GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

bool RSSystemParameters::GetWiredScreenOndrawEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.wiredScreenOndraw.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

bool RSSystemParameters::GetDebugMirrorOndrawEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.debugMirrorOndraw.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemParameters::GetArsrPreEnabled()
{
    static bool flag = system::GetBoolParameter("const.display.enable_arsr_pre", true);
    return flag;
}

bool RSSystemParameters::GetIsCopybitSupported()
{
    static bool flag = system::GetBoolParameter("const.display.support_copybit", false);
    return flag;
}

bool RSSystemParameters::GetMultimediaEnableCameraRotationCompensation()
{
    static bool flag = system::GetBoolParameter("const.multimedia.enable_camera_rotation_compensation", 0);
    return flag;
}

bool RSSystemParameters::GetCanvasDrawingNodeRegionEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.canvas_drawing_node.region.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

int32_t RSSystemParameters::GetWindowScreenScanType()
{
    static int32_t screenScanType = system::GetIntParameter<int32_t>("const.window.screen.scan_type", 0);
    return screenScanType;
}

int32_t RSSystemParameters::GetPurgeableResourceLimit()
{
    static int32_t purgeableResourceLimit =
        system::GetIntParameter<int32_t>("persist.sys.graphic.purgeableResourceLimit", 40000); // purge limit: 40000
    return purgeableResourceLimit;
}

bool RSSystemParameters::GetAnimationOcclusionEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.ani.occlusion.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

bool RSSystemParameters::GetUIFirstPurgeEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.uifirst.purge.enable", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

bool RSSystemParameters::GetUIFirstOcclusionEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.uni.uifirst.occlusion.enable", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0);
}

bool RSSystemParameters::GetUIFirstCaptrueReuseEnabled()
{
    static bool enable =
        std::atoi((system::GetParameter("persist.sys.graphic.uifirst.captrue.reuse.enable", "1")).c_str()) != 0;
    return enable;
}
bool RSSystemParameters::GetUIFirstStartingWindowCacheEnabled()
{
    static bool enable =
        std::atoi((system::GetParameter("persist.sys.graphic.uifirst.startingWindow.cache.enable", "1")).c_str()) != 0;
    return enable;
}
} // namespace Rosen
} // namespace OHOS
