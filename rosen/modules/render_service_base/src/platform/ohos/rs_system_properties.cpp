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
bool RSSystemProperties::GetUniRenderEnabled()
{
    static bool inited = false;
    if (inited) {
        return isUniRenderEnabled_;
    }

    isUniRenderEnabled_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
        ->GetUniRenderEnabled();
    isUniRenderMode_ = isUniRenderEnabled_;
    inited = true;
    ROSEN_LOGI("RSSystemProperties::GetUniRenderEnabled:%d", isUniRenderEnabled_);
    return isUniRenderEnabled_;
}

bool RSSystemProperties::GetRenderNodeTraceEnabled()
{
    static bool isNeedTrace = system::GetParameter("persist.rosen.rendernodetrace.enabled", "0") != "0";
    return isNeedTrace;
}

bool RSSystemProperties::IsUniRenderMode()
{
    return isUniRenderMode_;
}

void RSSystemProperties::SetRenderMode(bool isUni)
{
    isUniRenderMode_ = isUni;
}

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    return static_cast<DirtyRegionDebugType>(
        std::atoi((system::GetParameter("rosen.dirtyregiondebug.enabled", "0")).c_str()));
}

PartialRenderType RSSystemProperties::GetPartialRenderEnabled()
{
    return static_cast<PartialRenderType>(
        std::atoi((system::GetParameter("rosen.partialrender.enabled", "1")).c_str()));
}

PartialRenderType RSSystemProperties::GetUniPartialRenderEnabled()
{
    return static_cast<PartialRenderType>(
        std::atoi((system::GetParameter("rosen.uni.partialrender.enabled", "4")).c_str()));
}

ContainerWindowConfigType RSSystemProperties::GetContainerWindowConfig()
{
    return static_cast<ContainerWindowConfigType>(
        std::atoi((system::GetParameter("rosen.uni.containerwindowconfig", "2")).c_str()));
}

bool RSSystemProperties::GetOcclusionEnabled()
{
    return std::atoi((system::GetParameter("rosen.occlusion.enabled", "1")).c_str()) != 0;
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

uint64_t RSSystemProperties::GetDumpSurfaceId()
{
    return std::atoi((system::GetParameter("rosen.dumpsurfaceid", "0")).c_str());
}

void RSSystemProperties::SetDrawTextAsBitmap(bool flag)
{
    isDrawTextAsBitmap_ = flag;
}
bool RSSystemProperties::GetDrawTextAsBitmap()
{
    return isDrawTextAsBitmap_;
}

bool RSSystemProperties::GetColdStartThreadEnabled()
{
    return std::atoi((system::GetParameter("rosen.coldstartthread.enabled", "0")).c_str()) != 0;
}

float RSSystemProperties::GetAnimationScale()
{
    return std::atof((system::GetParameter("persist.sys.graphic.animationscale", "1.0")).c_str());
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
