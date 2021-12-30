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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H

#include "rs_command_templates.h"

namespace OHOS {
namespace Rosen {

enum RSNodeCommandType : uint16_t {
    RSNODE_CREATE,
    RSNODE_UPDATE_RECORDING,
};

class DrawCmdList;

class RSNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id);
    static void UpdateRecording(
        RSContext& context, NodeId id, std::shared_ptr<DrawCmdList> drawCmds, bool drawContentLast);
};

ADD_COMMAND(RSNodeCreate, ARG(NODE, RSNODE_CREATE, RSNodeCommandHelper::Create, NodeId))
ADD_COMMAND(RSNodeUpdateRecording, ARG(NODE, RSNODE_UPDATE_RECORDING, RSNodeCommandHelper::UpdateRecording, NodeId,
                                       std::shared_ptr<DrawCmdList>, bool))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
