/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_UNION_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_UNION_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum RSUnionNodeCommandType : uint16_t {
    UNION_NODE_CREATE = 0,
};

class RSB_EXPORT UnionNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, bool isTextureExportNode = false);
};

ADD_COMMAND(RSUnionNodeCreate,
    ARG(PERMISSION_APP, UNION_NODE, UNION_NODE_CREATE,
        UnionNodeCommandHelper::Create, NodeId, bool))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_UNION_NODE_COMMAND_H
