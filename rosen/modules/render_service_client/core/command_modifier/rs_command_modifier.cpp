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

#include "command_modifier/rs_command_modifier.h"

#include "platform/common/rs_log.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

bool RSCmdModifier::AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand) const
{
    auto node = node_.lock();
    if (!node) {
        RS_LOGE("RSCmdModifier::AddCommand: node is nullptr");
        return false;
    }
    return node->AddCommand(command, isRenderServiceCommand);
}

bool RSCmdModifier::AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand,
                               FollowType followType, NodeId nodeId) const
{
    auto node = node_.lock();
    if (!node) {
        RS_LOGE("RSCmdModifier::AddCommand: node is nullptr");
        return false;
    }
    return node->AddCommand(command, isRenderServiceCommand, followType, nodeId);
}

} // namespace Rosen
} // namespace OHOS
