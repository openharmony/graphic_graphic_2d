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
    return isUniRenderEnabled_;
}

void RSSystemProperties::InitUniRenderEnabled(const std::string &bundleName)
{
    static bool inited = false;
    if (inited) {
        return;
    }

    // init
    inited = true;
    isUniRenderEnabled_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
                ->InitUniRenderEnabled(bundleName);
    ROSEN_LOGI("Init UniRender Enabled:%d, package name:%s", isUniRenderEnabled_, bundleName.c_str());
}

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    return static_cast<DirtyRegionDebugType>(
        std::atoi((system::GetParameter("rosen.dirtyregiondebug.enabled", "0")).c_str()));
}

bool RSSystemProperties::GetOcclusionEnabled()
{
    return std::atoi((system::GetParameter("rosen.occlusion.enabled", "0")).c_str()) != 0;
}
} // namespace Rosen
} // namespace OHOS
