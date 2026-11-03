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

#ifndef RS_MULTI_DISPLAY_FEATURE_COMMON_H
#define RS_MULTI_DISPLAY_FEATURE_COMMON_H

#include <cinttypes>
#include <unordered_map>
#include <unordered_set>
#include "screen_manager/screen_types.h"

namespace OHOS::Rosen {
struct SplitScreenParams {
    ScreenId mainDisplayId = INVALID_SCREEN_ID;
    ScreenId subDisplayId = INVALID_SCREEN_ID;
};

struct SplitScreenFeature {
    bool enabled = false;
    std::unordered_map<ScreenId, SplitScreenParams> params;
};

struct InterpolationParams {
    ScreenId displayId = INVALID_SCREEN_ID;
    uint32_t realWidth = 0;
    uint32_t realHeight = 0;
    uint32_t paramA = 0;
    uint32_t paramN = 0;
};

struct InterpolationFeature {
    bool enabled = false;
    std::unordered_map<ScreenId, InterpolationParams> params;
};

struct CrossDomainFeature {
    bool enabled = false;
    std::unordered_set<ScreenId> params;
};
} // OHOS rosen
#endif // RS_MULTI_DISPLAY_FEATURE_COMMON_H