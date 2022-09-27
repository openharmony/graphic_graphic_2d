/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <parameters.h>
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
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
} // namespace Rosen
} // namespace OHOS
