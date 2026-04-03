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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_DEPTH_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_DEPTH_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {
enum RSDepthNodeCommandType : uint16_t {
    DEPTH_NODE_CREATE = 0,
};

class RSB_EXPORT RSDepthNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, bool isTextureExportNode = false);
};

ADD_COMMAND(RSDepthNodeCreate,
    ARG(PERMISSION_APP, DEPTH_NODE, DEPTH_NODE_CREATE,
        RSDepthNodeCommandHelper::Create, NodeId, bool))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_DEPTH_NODE_COMMAND_H
