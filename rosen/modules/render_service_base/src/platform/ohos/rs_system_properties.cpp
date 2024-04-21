/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "platform/common/rs_system_properties.h"

#include <cstdlib>
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "scene_board_judgement.h"
#include "pipeline/rs_uni_render_judgement.h"

namespace OHOS {
namespace Rosen {
constexpr int DEFAULT_CACHE_WIDTH = 1250;
constexpr int DEFAULT_CACHE_HEIGHT = 2710;
constexpr int DEFAULT_PARTIAL_RENDER_ENABLED_VALUE = 2;
constexpr int DEFAULT_UNI_PARTIAL_RENDER_ENABLED_VALUE = 4;
constexpr int DEFAULT_CORRECTION_MODE_VALUE = 999;
constexpr int DEFAULT_SCALE_MODE = 2;

#if (defined (ACE_ENABLE_GL) && defined (ACE_ENABLE_VK)) || (defined (RS_ENABLE_GL) && defined (RS_ENABLE_VK))
const GpuApiType RSSystemProperties::systemGpuApiType_ = Drawing::SystemProperties::GetGpuApiType();
#elif defined (ACE_ENABLE_GL) || defined (RS_ENABLE_GL)
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::OPENGL;
#else
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::VULKAN;
#endif

int ConvertToInt(const char *originValue, int defaultValue)
{
    return originValue == nullptr ? defaultValue : std::atoi(originValue);
}
static void ParseDfxSurfaceNamesString(const std::string& paramsStr,
    std::vector<std::string>& splitStrs, const std::string& seperator)
{
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = paramsStr.find(seperator);
    if (std::string::npos == pos2) {
        splitStrs.push_back(paramsStr);
        return;
    }
    while (std::string::npos != pos2) {
        splitStrs.push_back(paramsStr.substr(pos1, pos2 - pos1));
        pos1 = pos2 + seperator.size();
        pos2 = paramsStr.find(seperator, pos1);
    }
    if (pos1 != paramsStr.length()) {
        splitStrs.push_back(paramsStr.substr(pos1));
    }
}

bool RSSystemProperties::IsSceneBoardEnabled()
{
    return SceneBoardJudgement::IsSceneBoardEnabled();
}

// used by clients
int RSSystemProperties::GetDumpFrameNum()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.recording.frameNum", "0");
    int changed = 0;
    const char *num = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(num, 0);
}

int RSSystemProperties::GetRecordingEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("debug.graphic.recording.enabled", "0");
    int changed = 0;
    const char *num = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(num, 0);
}


void RSSystemProperties::SetRecordingDisenabled()
{
    system::SetParameter("debug.graphic.recording.enabled", "0");
    RS_LOGD("RSSystemProperties::SetRecordingDisenabled");
}

bool RSSystemProperties::GetProfilerEnabled()
{
    static CachedHandle handle = CachedParameterCreate("persist.graphic.profiler.enabled", "0");
    int32_t changed = 0;
    return ConvertToInt(CachedParameterGetChanged(handle, &changed), 0) != 0;
}

bool RSSystemProperties::GetInstantRecording()
{
    return (system::GetParameter("debug.graphic.instant.recording.enabled", "0") != "0");
}

void RSSystemProperties::SetInstantRecording(bool flag)
{
    system::SetParameter("debug.graphic.instant.recording.enabled", flag ? "1" : "0");
}

bool RSSystemProperties::GetSaveRDC()
{
    return (system::GetParameter("debug.graphic.rdcenabled", "0") != "0");
}

void RSSystemProperties::SetSaveRDC(bool flag)
{
    system::SetParameter("debug.graphic.rdcenabled", flag ? "1" : "0");
}

std::string RSSystemProperties::GetRecordingFile()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.dumpfile.path", "");
    int changed = 0;
    const char *file = CachedParameterGetChanged(g_Handle, &changed);
    return file == nullptr ? "" : file;
}

bool RSSystemProperties::GetUniRenderEnabled()
{
    static bool inited = false;
    if (inited) {
        return isUniRenderEnabled_;
    }

    isUniRenderEnabled_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
        ->GetUniRenderEnabled();
    inited = true;
    ROSEN_LOGD("RSSystemProperties::GetUniRenderEnabled:%{public}d", isUniRenderEnabled_);
    return isUniRenderEnabled_;
}

bool RSSystemProperties::GetDrawOpTraceEnabled()
{
    static bool code = system::GetParameter("persist.rosen.drawoptrace.enabled", "0") != "0";
    return code;
}

bool RSSystemProperties::GetRenderNodeTraceEnabled()
{
    static bool isNeedTrace = system::GetParameter("persist.rosen.rendernodetrace.enabled", "0") != "0";
    return isNeedTrace;
}

bool RSSystemProperties::GetRSScreenRoundCornerEnable()
{
    static bool isNeedScreenRCD = system::GetParameter("persist.rosen.screenroundcornerrcd.enabled", "0") != "0";
    return isNeedScreenRCD;
}

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.dirtyregiondebug.enabled", "0");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<DirtyRegionDebugType>(ConvertToInt(type, 0));
}

PartialRenderType RSSystemProperties::GetPartialRenderEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.partialrender.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<PartialRenderType>(ConvertToInt(enable, DEFAULT_PARTIAL_RENDER_ENABLED_VALUE));
}

PartialRenderType RSSystemProperties::GetUniPartialRenderEnabled()
{
    int changed = 0;
    static CachedHandle g_Handle = CachedParameterCreate("rosen.uni.partialrender.enabled", "4");
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<PartialRenderType>(ConvertToInt(enable, DEFAULT_UNI_PARTIAL_RENDER_ENABLED_VALUE));
}

bool RSSystemProperties::GetReleaseResourceEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("persist.release.gpuresource.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemProperties::GetOcclusionEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.occlusion.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemProperties::GetHardwareComposerEnabled()
{
    static bool hardwareComposerEnabled = system::GetParameter(
        "persist.rosen.hardwarecomposer.enabled", "1") != "0";
    return hardwareComposerEnabled;
}

bool RSSystemProperties::GetHwcRegionDfxEnabled()
{
    static bool hwcRegionDfxEnabled = system::GetParameter(
        "persist.rosen.hwcRegionDfx.enabled", "0") != "0";
    return hwcRegionDfxEnabled;
}

bool RSSystemProperties::GetAFBCEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.afbc.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

std::string RSSystemProperties::GetRSEventProperty(const std::string &paraName)
{
    return system::GetParameter(paraName, "0");
}

bool RSSystemProperties::GetDirectClientCompEnableStatus()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    static CachedHandle g_Handle = CachedParameterCreate("rosen.directClientComposition.enabled", "1");
    int changed = 0;
    const char *status = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(status, 1) != 0;
}

bool RSSystemProperties::GetHighContrastStatus()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    static CachedHandle g_Handle = CachedParameterCreate("rosen.HighContrast.enabled", "0");
    int changed = 0;
    const char *status = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(status, 0) != 0;
}

bool RSSystemProperties::GetTargetDirtyRegionDfxEnabled(std::vector<std::string>& dfxTargetSurfaceNames_)
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.dirtyregiondebug.surfacenames", "0");
    int changed = 0;
    const char *targetSurfacesStr = CachedParameterGetChanged(g_Handle, &changed);
    if (strcmp(targetSurfacesStr, "0") == 0) {
        dfxTargetSurfaceNames_.clear();
        return false;
    }
    dfxTargetSurfaceNames_.clear();
    ParseDfxSurfaceNamesString(targetSurfacesStr, dfxTargetSurfaceNames_, ",");
    return true;
}

bool RSSystemProperties::GetOpaqueRegionDfxEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.uni.opaqueregiondebug", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

bool RSSystemProperties::GetVisibleRegionDfxEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.uni.visibleregiondebug", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

SurfaceRegionDebugType RSSystemProperties::GetSurfaceRegionDfxType()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.uni.surfaceregiondebug", "0");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<SurfaceRegionDebugType>(ConvertToInt(type, 0));
}

uint32_t RSSystemProperties::GetCorrectionMode()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    static CachedHandle g_Handle = CachedParameterCreate("rosen.CorrectionMode", "999");
    int changed = 0;
    const char *mode = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(mode, DEFAULT_CORRECTION_MODE_VALUE);
}

DumpSurfaceType RSSystemProperties::GetDumpSurfaceType()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.dumpsurfacetype.enabled", "0");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<DumpSurfaceType>(ConvertToInt(type, 0));
}

long long int RSSystemProperties::GetDumpSurfaceId()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.dumpsurfaceid", "0");
    int changed = 0;
    const char *surfaceId = CachedParameterGetChanged(g_Handle, &changed);
    return surfaceId == nullptr ? std::atoll("0") : std::atoll(surfaceId);
}

bool RSSystemProperties::GetDumpLayersEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.dumplayer.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 0) != 0;
}

void RSSystemProperties::SetDrawTextAsBitmap(bool flag)
{
    isDrawTextAsBitmap_ = flag;
}
bool RSSystemProperties::GetDrawTextAsBitmap()
{
    return isDrawTextAsBitmap_;
}

void RSSystemProperties::SetCacheEnabledForRotation(bool flag)
{
    cacheEnabledForRotation_ = flag;
}

bool RSSystemProperties::GetCacheEnabledForRotation()
{
    return cacheEnabledForRotation_;
}

ParallelRenderingType RSSystemProperties::GetPrepareParallelRenderingEnabled()
{
    static ParallelRenderingType systemPropertiePrepareType = static_cast<ParallelRenderingType>(
        std::atoi((system::GetParameter("persist.rosen.prepareparallelrender.enabled", "1")).c_str()));
    return systemPropertiePrepareType;
}

ParallelRenderingType RSSystemProperties::GetParallelRenderingEnabled()
{
    static ParallelRenderingType systemPropertieType = static_cast<ParallelRenderingType>(
        std::atoi((system::GetParameter("persist.rosen.parallelrender.enabled", "0")).c_str()));
    return systemPropertieType;
}

HgmRefreshRates RSSystemProperties::GetHgmRefreshRatesEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.sethgmrefreshrate.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<HgmRefreshRates>(ConvertToInt(enable, 0));
}

void RSSystemProperties::SetHgmRefreshRateModesEnabled(std::string param)
{
    system::SetParameter("persist.rosen.sethgmrefreshratemode.enabled", param);
    RS_LOGI("RSSystemProperties::SetHgmRefreshRateModesEnabled set to %{public}s", param.c_str());
}

HgmRefreshRateModes RSSystemProperties::GetHgmRefreshRateModesEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("persist.rosen.sethgmrefreshratemode.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<HgmRefreshRateModes>(ConvertToInt(enable, 0));
}

bool RSSystemProperties::GetSkipForAlphaZeroEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("persist.skipForAlphaZero.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemProperties::GetSkipGeometryNotChangeEnabled()
{
    static bool skipGeoNotChangeEnabled =
        std::atoi((system::GetParameter("persist.skipGeometryNotChange.enabled", "1")).c_str()) != 0;
    return skipGeoNotChangeEnabled;
}

bool RSSystemProperties::GetAnimationCacheEnabled()
{
    static bool animationCacheEnabled =
        std::atoi((system::GetParameter("persist.animation.cache.enabled", "0")).c_str()) != 0;
    return animationCacheEnabled;
}

float RSSystemProperties::GetAnimationScale()
{
    static CachedHandle g_Handle = CachedParameterCreate("persist.sys.graphic.animationscale", "1.0");
    int changed = 0;
    const char *scale = CachedParameterGetChanged(g_Handle, &changed);
    return scale == nullptr ? std::atof("1.0") : std::atof(scale);
}

bool RSSystemProperties::GetFilterCacheEnabled()
{
    // Determine whether the filter cache should be enabled. The default value is 1, which means that it is enabled.
    // If dirty-region is not properly implemented, the filter cache will act as a skip-frame strategy for filters.
    static bool filterCacheEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheEnabled", "1")).c_str()) != 0;
    return filterCacheEnabled;
}

int RSSystemProperties::GetFilterCacheUpdateInterval()
{
    // Configure whether to enable skip-frame for the filter cache. The default value is 1, which means that the cached
    // image is updated with a delay of 1 frame.
    static int filterCacheUpdateInterval =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheUpdateInterval", "1")).c_str());
    return filterCacheUpdateInterval;
}

int RSSystemProperties::GetFilterCacheSizeThreshold()
{
    // Set the minimum size for enabling skip-frame in the filter cache. By default, this value is 400, which means that
    // skip-frame is only enabled for regions where both the width and height are greater than 400.
    static int filterCacheSizeThreshold =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheSizeThreshold", "400")).c_str());
    return filterCacheSizeThreshold;
}

bool RSSystemProperties::GetColorPickerPartialEnabled()
{
    // Determine whether the color picker partial render should be enabled. The default value is 0,
    // which means that it is unenabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.colorPickerPartialEnabled", "1")).c_str()) != 0;
    return enabled;
}

bool RSSystemProperties::GetMaskLinearBlurEnabled()
{
    // Determine whether the mask LinearBlur render should be enabled. The default value is 0,
    // which means that it is unenabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.maskLinearBlurEnabled", "1")).c_str()) != 0;
    return enabled;
}

bool RSSystemProperties::GetMotionBlurEnabled()
{
    // Determine whether the motionBlur render should be enabled. The default value is 0,
    // which means that it is unenabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.motionBlurEnabled", "1")).c_str()) != 0;
    return enabled;
}

bool RSSystemProperties::GetKawaseEnabled()
{
    static bool kawaseBlurEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.kawaseEnable", "1")).c_str()) != 0;
    return kawaseBlurEnabled;
}

bool RSSystemProperties::GetHpsBlurEnabled()
{
    static bool hpsBlurEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.HpsBlurEnable", "1")).c_str()) != 0;
    return hpsBlurEnabled;
}

float RSSystemProperties::GetKawaseRandomColorFactor()
{
    static float randomFactor =
        std::atof((system::GetParameter("persist.sys.graphic.kawaseFactor", "1.75")).c_str());
    return randomFactor;
}

bool RSSystemProperties::GetRandomColorEnabled()
{
    static bool randomColorEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.randomColorEnable", "1")).c_str()) != 0;
    return randomColorEnabled;
}

bool RSSystemProperties::GetKawaseOriginalEnabled()
{
    static bool kawaseOriginalEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.kawaseOriginalEnable", "0")).c_str()) != 0;
    return kawaseOriginalEnabled;
}

// this will migrate to rs_system_parameters.cpp
bool RSSystemProperties::GetQuickPrepareEnabled()
{
    static bool quickPrepareEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.quickPrepareEnabled", "1")).c_str()) != 0;
    return quickPrepareEnabled;
}

bool RSSystemProperties::GetRenderParallelEnabled()
{
    static bool enable =
        std::atoi((system::GetParameter("persist.sys.graphic.renderParallel", "1")).c_str()) != 0;
    return enable;
}

bool RSSystemProperties::GetBlurEnabled()
{
    static bool blurEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.blurEnabled", "1")).c_str()) != 0;
    return blurEnabled;
}

const std::vector<float>& RSSystemProperties::GetAiInvertCoef()
{
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    static std::vector<float> aiInvertCoef = {0.0, 1.0, 0.55, 0.4, 1.6, 45.0};
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        // Configure AiInvertCoef0: Low
        aiInvertCoef[0] =
            std::atof((system::GetParameter("persist.sys.graphic.aiInvertLow", "0")).c_str());
        // Configure AiInvertCoef1: High.
        aiInvertCoef[1] =
            std::atof((system::GetParameter("persist.sys.graphic.aiInvertHigh", "1")).c_str());
        // Configure AiInvertCoef2: Threshold.
        aiInvertCoef[2] =
            std::atof((system::GetParameter("persist.sys.graphic.aiInvertThreshold", "0.55")).c_str());
        // Configure AiInvertCoef3: Opacity.
        aiInvertCoef[3] =
            std::atof((system::GetParameter("persist.sys.graphic.aiInvertOpacity", "0.4")).c_str());
        // Configure AiInvertCoef4: Saturation.
        aiInvertCoef[4] =
            std::atof((system::GetParameter("persist.sys.graphic.aiInvertSaturation", "1.6")).c_str());
        // Configure AiInvertCoef5: Filter Radius.
        aiInvertCoef[5] =
            std::atof((system::GetParameter("persist.sys.graphic.aiInvertFilterRadius", "45")).c_str());
    }
    return aiInvertCoef;
}

bool RSSystemProperties::GetProxyNodeDebugEnabled()
{
    static bool proxyNodeDebugEnabled = system::GetParameter("persist.sys.graphic.proxyNodeDebugEnabled", "0") != "0";
    return proxyNodeDebugEnabled;
}

bool RSSystemProperties::GetUIFirstEnabled()
{
#ifdef ROSEN_EMULATOR
    return false;
#else
    static CachedHandle g_Handle = CachedParameterCreate("rosen.ui.first.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
#endif
}

bool RSSystemProperties::GetUIFirstDebugEnabled()
{
    static bool debugEnable = system::GetParameter("persist.sys.graphic.uifirstDebugEnabled", "0") != "0";
    return debugEnable;
}

bool RSSystemProperties::GetUIFirstForceEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.ui.first.force.enabled", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemProperties::GetDebugTraceEnabled()
{
    static bool openDebugTrace =
        std::atoi((system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str()) != 0;
    return openDebugTrace;
}

int RSSystemProperties::GetDebugTraceLevel()
{
    static int openDebugTraceLevel =
        std::atoi((system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str());
    return openDebugTraceLevel;
}

bool RSSystemProperties::GetDumpImgEnabled()
{
    static bool dumpImgEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.dumpImgEnabled", "0")).c_str()) != 0;
    return dumpImgEnabled;
}

bool RSSystemProperties::FindNodeInTargetList(std::string node)
{
    static std::string targetStr = system::GetParameter("persist.sys.graphic.traceTargetList", "");
    static auto strSize = targetStr.size();
    if (strSize == 0) {
        return false;
    }
    static std::vector<std::string> targetVec;
    static bool loaded = false;
    if (!loaded) {
        const std::string pattern = ";";
        targetStr += pattern;
        strSize = targetStr.size();
        std::string::size_type pos;
        for (std::string::size_type i = 0; i < strSize; i++) {
            pos = targetStr.find(pattern, i);
            if (pos >= strSize) {
                break;
            }
            auto str = targetStr.substr(i, pos - i);
            if (str.size() > 0) {
                targetVec.emplace_back(str);
            }
            i = pos;
        }
        loaded = true;
    }
    bool res = std::find(targetVec.begin(), targetVec.end(), node) != targetVec.end();
    return res;
}

bool RSSystemProperties::IsFoldScreenFlag()
{
    static bool isFoldScreenFlag = system::GetParameter("const.window.foldscreen.type", "") != "";
    return isFoldScreenFlag;
}

bool RSSystemProperties::GetCacheCmdEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.cacheCmd.enabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemProperties::GetASTCEnabled()
{
    static bool isASTCEnabled = std::atoi((system::GetParameter("persist.rosen.astc.enabled", "0")).c_str()) != 0;
    return isASTCEnabled;
}

// GetCachedBlurPartialRenderEnabled Option On: no need to expand blur dirtyregion if blur has background cache
bool RSSystemProperties::GetCachedBlurPartialRenderEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.cachedblurpartialrender.enabled", "0");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(type, 1) != 0;
}

bool RSSystemProperties::GetParallelUploadTexture()
{
    static bool enable = std::atoi((system::GetParameter("rosen.parallelUpload,enabled", "1")).c_str()) != 0;
    return enable;
}

bool RSSystemProperties::GetImageGpuResourceCacheEnable(int width, int height)
{
    static bool cacheEnable =
        std::atoi((system::GetParameter("persist.sys.graphic.gpuResourceCacheEnable", "1")).c_str()) != 0;
    if (!cacheEnable) {
        return false;
    }

    // default cache full screen image gpu resource.
    static int widthConfig =
        std::atoi((system::GetParameter("persist.sys.graphic.gpuResourceCacheWidth", "0")).c_str());
    static int heightConfig =
        std::atoi((system::GetParameter("persist.sys.graphic.gpuResourceCacheHeight", "0")).c_str());
    int cacheWidth = widthConfig == 0 ? DEFAULT_CACHE_WIDTH : widthConfig;
    int cacheHeight = heightConfig == 0 ? DEFAULT_CACHE_HEIGHT : heightConfig;
    if (width >= cacheWidth && height >= cacheHeight) {
        return true;
    }
    return false;
}

bool RSSystemProperties::GetBoolSystemProperty(const char* name, bool defaultValue)
{
    static CachedHandle g_Handle = CachedParameterCreate(name, defaultValue ? "1" : "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, defaultValue ? 1 : 0) != 0;
}

int RSSystemProperties::WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context)
{
    return WatchParameter(name, func, context);
}

bool RSSystemProperties::GetSnapshotWithDMAEnabled()
{
    static bool isSupportDma = system::GetParameter("const.product.devicetype", "pc") == "phone" ||
        system::GetParameter("const.product.devicetype", "pc") == "tablet" ||
        system::GetParameter("const.product.devicetype", "pc") == "pc" ||
        system::GetParameter("const.product.devicetype", "pc") == "2in1";
    return isSupportDma && system::GetBoolParameter("rosen.snapshotDma.enabled", true);
}

bool RSSystemProperties::IsPhoneType()
{
    static bool isPhone = system::GetParameter("const.product.devicetype", "pc") == "phone";
    return isPhone;
}

bool RSSystemProperties::IsPcType()
{
    static bool isPc = (system::GetParameter("const.product.devicetype", "pc") == "pc") ||
                       (system::GetParameter("const.product.devicetype", "pc") == "2in1");
    return isPc;
}

bool RSSystemProperties::GetSyncTransactionEnabled()
{
    static bool syncTransactionEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.syncTransaction.enabled", "1")).c_str()) != 0;
    return syncTransactionEnabled;
}

int RSSystemProperties::GetSyncTransactionWaitDelay()
{
    static int syncTransactionWaitDelay =
        std::atoi((system::GetParameter("persist.sys.graphic.syncTransactionWaitDelay", "1500")).c_str());
    return syncTransactionWaitDelay;
}

bool RSSystemProperties::GetSingleFrameComposerEnabled()
{
    static bool singleFrameComposerEnabled =
        (std::atoi((system::GetParameter("persist.sys.graphic.singleFrameComposer", "0")).c_str()) != 0);
    return singleFrameComposerEnabled;
}

bool RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled()
{
    static bool singleFrameComposerCanvasNodeEnabled =
        (std::atoi((system::GetParameter("persist.sys.graphic.singleFrameComposerCanvasNode", "0")).c_str()) != 0);
    return singleFrameComposerCanvasNodeEnabled;
}

#ifdef DDGR_ENABLE_FEATURE_OPINC
const DdgrOpincType RSSystemProperties::ddgrOpincType_ =
    static_cast<DdgrOpincType>(std::atoi((system::GetParameter("persist.ddgr.opinctype", "2")).c_str()));
const DdgrOpincDfxType RSSystemProperties::ddgrOpincDfxType_ =
    static_cast<DdgrOpincDfxType>(std::atoi((
        system::GetParameter("persist.rosen.ddgr.opinctype.debugtype", "0")).c_str()));

DdgrOpincType RSSystemProperties::GetDdgrOpincType()
{
    return RSSystemProperties::ddgrOpincType_;
}

bool RSSystemProperties::IsDdgrOpincEnable()
{
    return ((GetDdgrOpincType() == DdgrOpincType::DDGR_AUTOCACHE ||
        GetDdgrOpincType() == DdgrOpincType::DDGR_AUTOCACHE_REALDRAW ||
        GetDdgrOpincType() == DdgrOpincType::DDGR_RENDERCACHE ||
        GetDdgrOpincType() == DdgrOpincType::DDGR_OPINCUPDATE) &&
        (RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR)) ||
        (GetDdgrOpincType() == DdgrOpincType::DDGR_UNRESTRICTED_MODE) ||
        (GetDdgrOpincType() == DdgrOpincType::DDGR_AUTOCACHE_REALDRAW);
}

bool RSSystemProperties::IsOpincRealDrawCacheEnable()
{
    return  GetDdgrOpincType() == DdgrOpincType::DDGR_AUTOCACHE_REALDRAW;
}

DdgrOpincDfxType RSSystemProperties::GetDdgrOpincDfxType()
{
    return ddgrOpincDfxType_;
}

bool RSSystemProperties::GetAutoCacheDebugEnabled()
{
    return GetDdgrOpincDfxType() == DdgrOpincDfxType::DDGR_OPINC_DFX_AUTO;
}
#endif

#ifdef RS_ENABLE_STACK_CULLING
bool RSSystemProperties::GetViewOcclusionCullingEnabled()
{
    static bool stackViewCullingEnabled =
        system::GetBoolParameter("persist.sys.graphic.stack.culling.enabled", true);
    return stackViewCullingEnabled;
}
#endif

bool RSSystemProperties::GetSubSurfaceEnabled()
{
    static bool subSurfaceEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.subSurface", "1")).c_str());
    return subSurfaceEnabled;
}

bool RSSystemProperties::GetSecurityPermissionCheckEnabled()
{
    static bool openSecurityPermissionCheck =
        std::atoi((system::GetParameter("persist.sys.graphic.openSecurityPermissionCheck", "0")).c_str()) != 0;
    return openSecurityPermissionCheck;
}

bool RSSystemProperties::GetEffectMergeEnabled()
{
    static CachedHandle g_Handle = CachedParameterCreate("rosen.graphic.effectMergeEnabled", "1");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, 1) != 0;
}

bool RSSystemProperties::GetDumpUICaptureEnabled()
{
    bool dumpUICaptureEnabled =
        std::atoi((system::GetParameter("rosen.dumpUICaptureEnabled.enabled", "0")).c_str()) != 0;
    return dumpUICaptureEnabled;
}

bool RSSystemProperties::GetDumpUIPixelmapEnabled()
{
    bool dumpUIPixelmapEnabled =
        std::atoi((system::GetParameter("rosen.dumpUIPixelmapEnabled.enabled", "0")).c_str()) != 0;
    return dumpUIPixelmapEnabled;
}

uint32_t RSSystemProperties::GetVirtualScreenScaleModeDFX()
{
    static uint32_t scaleModeDFX =
        std::atoi((system::GetParameter("persist.rosen.virtualScreenScaleMode.debugType", "2")).c_str());
    return (scaleModeDFX > DEFAULT_SCALE_MODE) ? DEFAULT_SCALE_MODE : scaleModeDFX;
}

SubTreePrepareCheckType RSSystemProperties::GetSubTreePrepareCheckType()
{
    static CachedHandle g_Handle = CachedParameterCreate("persist.sys.graphic.SubTreePrepareCheckType.type", "2");
    int changed = 0;
    const char *type = CachedParameterGetChanged(g_Handle, &changed);
    return static_cast<SubTreePrepareCheckType>(ConvertToInt(type, 2)); // Default value 2
}
} // namespace Rosen
} // namespace OHOS
