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

#include "command_modifier/rs_depth_node_command_modifier.h"

#include "platform/common/rs_log.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

void DepthSpaceTypeCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    // To be implemented
}

void DepthSpaceTypeCmdModifier::DumpParam(std::string& out) const
{
    out += "{depthSpaceType:" + std::to_string(static_cast<int>(param_.depthSpaceType_)) + "}";
}

} // namespace Rosen
} // namespace OHOS