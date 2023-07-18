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
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {

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

// used by clients
int RSSystemProperties::GetDumpFrameNum()
{
    return std::atoi((system::GetParameter("debug.graphic.recording.frameNum", "0")).c_str());
}

bool RSSystemProperties::GetRecordingEnabled()
{
    return (system::GetParameter("debug.graphic.recording.enabled", "0") != "0");
}

void RSSystemProperties::SetRecordingDisenabled()
{
    system::SetParameter("debug.graphic.recording.enabled", "0");
    RS_LOGD("RSSystemProperties::SetRecordingDisenabled");
}

std::string RSSystemProperties::GetRecordingFile()
{
    return system::GetParameter("debug.graphic.dumpfile.path", "");
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
    ROSEN_LOGI("RSSystemProperties::GetUniRenderEnabled:%d", isUniRenderEnabled_);
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

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    return static_cast<DirtyRegionDebugType>(
        std::atoi((system::GetParameter("rosen.dirtyregiondebug.enabled", "0")).c_str()));
}

PartialRenderType RSSystemProperties::GetPartialRenderEnabled()
{
    return static_cast<PartialRenderType>(
        std::atoi((system::GetParameter("rosen.partialrender.enabled", "2")).c_str()));
}

PartialRenderType RSSystemProperties::GetUniPartialRenderEnabled()
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    return static_cast<PartialRenderType>(
        std::atoi((system::GetParameter("rosen.uni.partialrender.enabled", "0")).c_str()));
#else
    return static_cast<PartialRenderType>(
        std::atoi((system::GetParameter("rosen.uni.partialrender.enabled", "4")).c_str()));
#endif
}

ReleaseGpuResourceType RSSystemProperties::GetReleaseGpuResourceEnabled()
{
    return static_cast<ReleaseGpuResourceType>(
        std::atoi((system::GetParameter("persist.release.gpuresource.enabled", "2")).c_str()));
}

bool RSSystemProperties::GetOcclusionEnabled()
{
    return std::atoi((system::GetParameter("rosen.occlusion.enabled", "1")).c_str()) != 0;
}

bool RSSystemProperties::GetQuickSkipPrepareEnabled()
{
    return system::GetParameter("rosen.quickskipprepare.enabled", "1") != "0";
}

bool RSSystemProperties::GetHardwareComposerEnabled()
{
    return system::GetParameter("rosen.hardwarecomposer.enabled", "1") != "0";
}

bool RSSystemProperties::GetAFBCEnabled()
{
    return system::GetParameter("rosen.afbc.enabled", "1") != "0";
}

std::string RSSystemProperties::GetRSEventProperty(const std::string &paraName)
{
    return system::GetParameter(paraName, "0");
}

bool RSSystemProperties::GetDirectClientCompEnableStatus()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    return std::atoi((system::GetParameter("rosen.directClientComposition.enabled", "1")).c_str()) != 0;
}

bool RSSystemProperties::GetHighContrastStatus()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    return std::atoi((system::GetParameter("rosen.HighContrast.enabled", "0")).c_str()) != 0;
}

bool RSSystemProperties::GetTargetDirtyRegionDfxEnabled(std::vector<std::string>& dfxTargetSurfaceNames_)
{
    std::string targetSurfacesStr = system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
    if (targetSurfacesStr == "0") {
        dfxTargetSurfaceNames_.clear();
        return false;
    }
    dfxTargetSurfaceNames_.clear();
    ParseDfxSurfaceNamesString(targetSurfacesStr, dfxTargetSurfaceNames_, ",");
    return true;
}

bool RSSystemProperties::GetOpaqueRegionDfxEnabled()
{
    return std::atoi((system::GetParameter("rosen.uni.opaqueregiondebug", "0")).c_str()) != 0;
}

uint32_t RSSystemProperties::GetCorrectionMode()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    return std::atoi((system::GetParameter("rosen.CorrectionMode", "999")).c_str());
}

DumpSurfaceType RSSystemProperties::GetDumpSurfaceType()
{
    return static_cast<DumpSurfaceType>(
        std::atoi((system::GetParameter("rosen.dumpsurfacetype.enabled", "0")).c_str()));
}

long long int RSSystemProperties::GetDumpSurfaceId()
{
    return std::atoll((system::GetParameter("rosen.dumpsurfaceid", "0")).c_str());
}

bool RSSystemProperties::GetDumpLayersEnabled()
{
    return std::atoi((system::GetParameter("rosen.dumplayer.enabled", "0")).c_str()) != 0;
}

void RSSystemProperties::SetDrawTextAsBitmap(bool flag)
{
    isDrawTextAsBitmap_ = flag;
}
bool RSSystemProperties::GetDrawTextAsBitmap()
{
    return isDrawTextAsBitmap_;
}

ParallelRenderingType RSSystemProperties::GetPrepareParallelRenderingEnabled()
{
    return static_cast<ParallelRenderingType>(
        std::atoi((system::GetParameter("rosen.prepareparallelrender.enabled", "1")).c_str()));
}

ParallelRenderingType RSSystemProperties::GetParallelRenderingEnabled()
{
    return static_cast<ParallelRenderingType>(
        std::atoi((system::GetParameter("rosen.parallelrender.enabled", "0")).c_str()));
}

HgmRefreshRates RSSystemProperties::GetHgmRefreshRatesEnabled()
{
    return static_cast<HgmRefreshRates>(
        std::atoi((system::GetParameter("rosen.sethgmrefreshrate.enabled", "0")).c_str()));
}

HgmRefreshRateModes RSSystemProperties::GetHgmRefreshRateModesEnabled()
{
    return static_cast<HgmRefreshRateModes>(
        std::atoi((system::GetParameter("rosen.sethgmrefreshratemode.enabled", "0")).c_str()));
}

bool RSSystemProperties::GetColdStartThreadEnabled()
{
    return std::atoi((system::GetParameter("rosen.coldstartthread.enabled", "0")).c_str()) != 0;
}

bool RSSystemProperties::GetSkipForAlphaZeroEnabled()
{
    return std::atoi((system::GetParameter("persist.skipForAlphaZero.enabled", "1")).c_str()) != 0;
}

float RSSystemProperties::GetAnimationScale()
{
    return std::atof((system::GetParameter("persist.sys.graphic.animationscale", "1.0")).c_str());
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

bool RSSystemProperties::GetKawaseEnabled()
{
    static bool kawaseBlurEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.kawaseEnable", "1")).c_str()) != 0;
    return kawaseBlurEnabled;
}

bool RSSystemProperties::GetProxyNodeDebugEnabled()
{
    static bool proxyNodeDebugEnabled = system::GetParameter("persist.sys.graphic.proxyNodeDebugEnabled", "0") != "0";
    return proxyNodeDebugEnabled;
}

bool RSSystemProperties::GetUIFirstEnabled()
{
    static bool isPhone = system::GetParameter("const.product.devicetype", "pc") == "phone";
    return (std::atoi((system::GetParameter("rosen.ui.first.enabled", "1")).c_str()) != 0) && isPhone;
}

bool RSSystemProperties::GetCacheCmdEnabled()
{
    return std::atoi((system::GetParameter("rosen.cacheCmd.enabled", "1")).c_str()) != 0;
}

bool RSSystemProperties::GetASTCEnabled()
{
    static bool isASTCEnabled = std::atoi((system::GetParameter("persist.rosen.astc.enabled", "0")).c_str()) != 0;
    return isASTCEnabled;
}

bool RSSystemProperties::GetBoolSystemProperty(const char* name, bool defaultValue)
{
    return std::atoi((system::GetParameter(name, defaultValue ? "1" : "0")).c_str()) != 0;
}

int RSSystemProperties::WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context)
{
    return WatchParameter(name, func, context);
}
} // namespace Rosen
} // namespace OHOS
