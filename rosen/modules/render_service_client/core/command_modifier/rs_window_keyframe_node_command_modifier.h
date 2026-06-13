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

#ifndef RENDER_SERVICE_CLIENT_CORE_FEATURE_WINDOW_KEYFRAME_RS_WINDOW_KEYFRAME_NODE_COMMAND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_FEATURE_WINDOW_KEYFRAME_RS_WINDOW_KEYFRAME_NODE_COMMAND_MODIFIER_H

#include "command_modifier/rs_command_modifier.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSWindowKeyFrameNode;

struct WkfIsFreezeCmdParam {
    bool isFreeze_;
    bool isMarkedByUI_;
};

class WkfIsFreezeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::WKF_IS_FREEZE;

    WkfIsFreezeCmdModifier(std::weak_ptr<RSNode> node, const WkfIsFreezeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const WkfIsFreezeCmdParam& param)
    {
        if (param_.isFreeze_ == param.isFreeze_ &&
            param_.isMarkedByUI_ == param.isMarkedByUI_) {
            return true;
        }
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const WkfIsFreezeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    WkfIsFreezeCmdParam param_;
};

struct WkfLinkedNodeIdCmdParam {
    NodeId linkedNodeId_;
};

class WkfLinkedNodeIdCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::WKF_LINKED_NODE_ID;

    WkfLinkedNodeIdCmdModifier(std::weak_ptr<RSNode> node, const WkfLinkedNodeIdCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const WkfLinkedNodeIdCmdParam& param)
    {
        if (param_.linkedNodeId_ == param.linkedNodeId_) {
            return true;
        }
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const WkfLinkedNodeIdCmdParam& GetParam() const
    {
        return param_;
    }

private:
    WkfLinkedNodeIdCmdParam param_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_FEATURE_WINDOW_KEYFRAME_RS_WINDOW_KEYFRAME_NODE_COMMAND_MODIFIER_H