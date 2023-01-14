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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_OCCLUSION_CONFIG_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_OCCLUSION_CONFIG_H

#include <string>
#include <vector>
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSOcclusionConfig {
public:
    static RSOcclusionConfig& GetInstance();
    bool IsLeashWindow(std::string win);
    bool IsStartingWindow(std::string win);
    bool IsAlphaWindow(std::string win);
    bool IsDividerBar(std::string win);
private:
    std::vector<std::string> alphaWins_ = {
        "StatusBar",
        "NavigationBar",
        "DropdownPanel",
        "SUBWINDOW",
        "freeze0"};
    std::string leashWin_ = "leashWindow";
    std::string startWin_ = "startingWindow";
    std::string dividerBar_ = "divider_bar";
};
} // namespace Rosen
} // namespace OHOS
#endif
