/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_ROOT_COMMAND_RS_ROOT_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_ROOT_COMMAND_RS_ROOT_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "platform/drawing/rs_surface.h"

namespace OHOS {
namespace Rosen {

enum RSRootNodeCommandType : uint16_t {
    ROOT_NODE_CREATE,
    ROOT_NODE_ATTACH,
};

class RootNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id);
    static void AttachRSSurfaceNode(
        RSContext& context, NodeId id, NodeId surfaceNodeId, int width, int height);
};

ADD_COMMAND(RSRootNodeCreate, ARG(ROOT_NODE, ROOT_NODE_CREATE, RootNodeCommandHelper::Create, NodeId))
ADD_COMMAND(RSRootNodeAttachRSSurfaceNode, ARG(ROOT_NODE, ROOT_NODE_ATTACH, RootNodeCommandHelper::AttachRSSurfaceNode,
                                               NodeId, NodeId, int, int))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_ROOT_COMMAND_RS_ROOT_NODE_COMMAND_H
