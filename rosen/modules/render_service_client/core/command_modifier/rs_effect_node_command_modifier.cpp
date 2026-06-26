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

#include "command_modifier/rs_effect_node_command_modifier.h"

#include "command/rs_node_command.h"
#include "ui/rs_effect_node.h"

namespace OHOS {
namespace Rosen {

void PreFreezeCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSEffectNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(
        node->GetId(), param_.preFreeze_, param_.preMarkedByUI_);
    AddCommand(command, true);
}

void PreFreezeCmdModifier::DumpParam(std::string& out) const
{
    out += "{preFreeze:" + std::string(param_.preFreeze_ ? "true" : "false") +
           ", preMarkedByUI:" + std::string(param_.preMarkedByUI_ ? "true" : "false") + "}";
}

} // namespace Rosen
} // namespace OHOS
