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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_FRAME_RATE_LINKER_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_FRAME_RATE_LINKER_COMMAND_H

#include "animation/rs_frame_rate_range.h"
#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum RSFrameRateLinkerCommandType : uint16_t {
    FRAME_RATE_LINKER_DESTROY = 0,
    FRAME_RATE_LINKER_UPDATE_RANGE = 1,
};

class RSB_EXPORT RSFrameRateLinkerCommandHelper {
public:
    static void Destroy(RSContext& context, FrameRateLinkerId id);
    static void UpdateRange(RSContext& context, FrameRateLinkerId id, FrameRateRange range,
        int32_t animatorExpectedFrameRate);
};

ADD_COMMAND(RSFrameRateLinkerDestroy,
    ARG(PERMISSION_APP, FRAME_RATE_LINKER, FRAME_RATE_LINKER_DESTROY,
        RSFrameRateLinkerCommandHelper::Destroy, FrameRateLinkerId))
ADD_COMMAND(RSFrameRateLinkerUpdateRange,
    ARG(PERMISSION_APP, FRAME_RATE_LINKER, FRAME_RATE_LINKER_UPDATE_RANGE,
        RSFrameRateLinkerCommandHelper::UpdateRange, FrameRateLinkerId, FrameRateRange, int32_t))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_FRAME_RATE_LINKER_COMMAND_H
