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

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_ROOT_NODE_COMMAND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_ROOT_NODE_COMMAND_MODIFIER_H

#include "command_modifier/rs_command_modifier.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSRootNode;

struct AttachRootNodeCmdParam {
    NodeId attachSurfaceNodeId_;
    uint64_t attachSurfaceNodeToken_;
    bool isSurfaceNodeTree_;
};

class AttachRootNodeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::ATTACH_ROOT_NODE;

    AttachRootNodeCmdModifier(std::weak_ptr<RSNode> node, const AttachRootNodeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const AttachRootNodeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const AttachRootNodeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    AttachRootNodeCmdParam param_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_ROOT_NODE_COMMAND_MODIFIER_H