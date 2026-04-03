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

#ifndef RENDER_SERVICE_BASE_COMMAND_RS_SPATIAL_EFFECT_COMMAND_H
#define RENDER_SERVICE_BASE_COMMAND_RS_SPATIAL_EFFECT_COMMAND_H

#include "command/rs_command_templates.h"
#include "command/rs_macros.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {
enum RSSpatialEffectCommandType : uint16_t {
    SET_IS_DEPTH_BACKGROUND = 0,
    SET_IS_DEPTH_RESOURCE = 1,
};

class RSB_EXPORT RSSpatialEffectCommandHelper {
public:
    static void SetIsDepthBackground(RSContext& context, NodeId id, bool isDepthBackground);
    static void SetIsDepthResource(RSContext& context, NodeId id, bool isDepthResource);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMAND_RS_SPATIAL_EFFECT_COMMAND_H
