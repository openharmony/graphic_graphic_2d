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

#include "command/rs_ui_director_command.h"

#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_ui_render_director.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

void RSUIDirectorCommandHelper::GoCreate(RSContext& context, NodeId nodeId, pid_t pid, uint64_t token)
{
}

void RSUIDirectorCommandHelper::GoResume(RSContext& context, NodeId nodeId, pid_t pid, uint64_t token)
{
}

void RSUIDirectorCommandHelper::GoForeground(RSContext& context, NodeId nodeId, pid_t pid, uint64_t token)
{
}

void RSUIDirectorCommandHelper::GoBackground(RSContext& context, NodeId nodeId, pid_t pid, uint64_t token)
{
}

void RSUIDirectorCommandHelper::GoStop(RSContext& context, NodeId nodeId, pid_t pid, uint64_t token)
{
    context.GetMutableNodeMap().DestroyTokenNode(pid, token);
}

void RSUIDirectorCommandHelper::GoDestroy(RSContext& context, NodeId nodeId, pid_t pid, uint64_t token)
{
}

} // namespace Rosen
} // namespace OHOS