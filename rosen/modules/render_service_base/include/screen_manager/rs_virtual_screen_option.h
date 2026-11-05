/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_VIRTUAL_SCREEN_OPTION_H
#define RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_VIRTUAL_SCREEN_OPTION_H

#ifndef ROSEN_CROSS_PLATFORM

#include <cstdint>
#include <string>
#include <vector>

#include <surface.h>

#include "common/rs_common_def.h"
#include "screen_manager/screen_types.h"

namespace OHOS::Rosen {

// Options for creating a virtual screen. It wraps the params of the legacy
// CreateVirtualScreen interface and adds the HDR mode. The mode is applied when
// the virtual screen is created, so the first frame can always read the expected
// HDR mode in RSHdrUtil::HandleVirtualScreenHDRStatus, which cannot be guaranteed
// by the separate RSInterfaces::SetVirtualScreenHDRMode call.
struct RSVirtualScreenOption {
    std::string name;
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<Surface> surface = nullptr;
    ScreenId associatedScreenId = 0;
    int32_t flags = 0;
    std::vector<NodeId> whiteList = {};
    HDRMode hdrMode = HDRMode::NONE;
};

} // namespace OHOS::Rosen

#endif // ROSEN_CROSS_PLATFORM
#endif // RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_VIRTUAL_SCREEN_OPTION_H
