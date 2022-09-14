/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "pipeline/rs_occlusion_config.h"

namespace OHOS {
namespace Rosen {
RSOcclusionConfig& RSOcclusionConfig::GetInstance()
{
    static RSOcclusionConfig instance;
    return instance;
}

bool RSOcclusionConfig::IsLeashWindow(std::string win)
{
    return win.find(leashWin_) != std::string::npos;
}

bool RSOcclusionConfig::IsStartingWindow(std::string win)
{
    return win.find(startWin_) != std::string::npos;
}

bool RSOcclusionConfig::IsAlphaWindow(std::string win)
{
    for (auto str : alphaWins_) {
        if (win.find(str) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool RSOcclusionConfig::IsDividerBar(std::string win)
{
    return win.find(dividerBar_) != std::string::npos;
}
} // namespace Rosen
} // namespace OHOS
