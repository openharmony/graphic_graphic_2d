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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_NODE_COMMAND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_NODE_COMMAND_MODIFIER_H

#include <memory>
#include <string>

#include "command/rs_base_node_command.h"
#include "command/rs_node_command.h"
#include "command_modifier/rs_command_modifier.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSNode;

struct OcclusionCullingStatusCmdParam {
    bool enablekeyOcclusion_;
    NodeId keyOcclusionNodeId_;
};

class OcclusionCullingStatusCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::OCCLUSION_CULLING_STATUS;

    OcclusionCullingStatusCmdModifier(std::weak_ptr<RSNode> node, const OcclusionCullingStatusCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const OcclusionCullingStatusCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const OcclusionCullingStatusCmdParam& GetParam() const
    {
        return param_;
    }

private:
    OcclusionCullingStatusCmdParam param_;
};

struct NodeNameCmdParam {
    std::string nodeName_;
};

class NodeNameCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::NODE_NAME;

    NodeNameCmdModifier(std::weak_ptr<RSNode> node, const NodeNameCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const NodeNameCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const NodeNameCmdParam& GetParam() const
    {
        return param_;
    }

private:
    NodeNameCmdParam param_;
};

struct IsP3ColorCmdParam {
    int8_t collectColorSpace_;
};

class IsP3ColorCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_P3COLOR;

    IsP3ColorCmdModifier(std::weak_ptr<RSNode> node, const IsP3ColorCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const IsP3ColorCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const IsP3ColorCmdParam& GetParam() const
    {
        return param_;
    }

private:
    IsP3ColorCmdParam param_;
};

struct DrawRegionCmdParam {
    std::shared_ptr<RectF> drawRegion_;
};

class DrawRegionCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::DRAW_REGION;

    DrawRegionCmdModifier(std::weak_ptr<RSNode> node, const DrawRegionCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const DrawRegionCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const DrawRegionCmdParam& GetParam() const
    {
        return param_;
    }

private:
    DrawRegionCmdParam param_;
};

struct UseCmdlistDrawRegionCmdParam {
    bool needUseCmdlistDrawRegion_;
};

class UseCmdlistDrawRegionCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::USE_CMDLIST_DRAWREGION;

    UseCmdlistDrawRegionCmdModifier(std::weak_ptr<RSNode> node, const UseCmdlistDrawRegionCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const UseCmdlistDrawRegionCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const UseCmdlistDrawRegionCmdParam& GetParam() const
    {
        return param_;
    }

private:
    UseCmdlistDrawRegionCmdParam param_;
};

struct ExcludeNodeGroupCmdParam {
    bool isExcludedFromNodeGroup_;
};

class ExcludeNodeGroupCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::EXCLUDE_NODE_GROUP;

    ExcludeNodeGroupCmdModifier(std::weak_ptr<RSNode> node, const ExcludeNodeGroupCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ExcludeNodeGroupCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ExcludeNodeGroupCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ExcludeNodeGroupCmdParam param_;
};

struct MarkNodeSingleFrameComposerCmdParam {
    bool isNodeSingleFrameComposer_;
    pid_t realPid_;
};

class MarkNodeSingleFrameComposerCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::MARK_NODE_SINGLE_FRAME_COMPOSER;

    MarkNodeSingleFrameComposerCmdModifier(std::weak_ptr<RSNode> node, const MarkNodeSingleFrameComposerCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const MarkNodeSingleFrameComposerCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const MarkNodeSingleFrameComposerCmdParam& GetParam() const
    {
        return param_;
    }

private:
    MarkNodeSingleFrameComposerCmdParam param_;
};

struct IsRepaintBoundaryCmdParam {
    bool isRepaintBoundary_;
};

class IsRepaintBoundaryCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_REPAINT_BOUNDARY;

    IsRepaintBoundaryCmdModifier(std::weak_ptr<RSNode> node, const IsRepaintBoundaryCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const IsRepaintBoundaryCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const IsRepaintBoundaryCmdParam& GetParam() const
    {
        return param_;
    }

private:
    IsRepaintBoundaryCmdParam param_;
};

struct MarkOpincNodeCmdParam {
    bool isSuggestOpincNode_;
    bool isOpincNeedCalculate_;
};

class MarkOpincNodeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::MARK_OPINC_NODE;

    MarkOpincNodeCmdModifier(std::weak_ptr<RSNode> node, const MarkOpincNodeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const MarkOpincNodeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const MarkOpincNodeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    MarkOpincNodeCmdParam param_;
};

struct IsUifirstNodeCmdParam {
    bool isUifirstNode_;
};

class IsUifirstNodeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_UIFIRST_NODE;

    IsUifirstNodeCmdModifier(std::weak_ptr<RSNode> node, const IsUifirstNodeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const IsUifirstNodeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const IsUifirstNodeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    IsUifirstNodeCmdParam param_;
};

struct IsForceUifirstNodeCmdParam {
    bool isForceFlag_;
    bool isUifirstEnable_;
};

class IsForceUifirstNodeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_FORCE_UIFIRST_NODE;

    IsForceUifirstNodeCmdModifier(std::weak_ptr<RSNode> node, const IsForceUifirstNodeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const IsForceUifirstNodeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const IsForceUifirstNodeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    IsForceUifirstNodeCmdParam param_;
};

struct SyncDrawNodeTypeCmdParam {
    DrawNodeType nodeType_;
};

class SyncDrawNodeTypeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::SYNC_DRAW_NODE_TYPE;

    SyncDrawNodeTypeCmdModifier(std::weak_ptr<RSNode> node, const SyncDrawNodeTypeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const SyncDrawNodeTypeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const SyncDrawNodeTypeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    SyncDrawNodeTypeCmdParam param_;
};

struct UIFirstSwitchCmdParam {
    RSUIFirstSwitch uifirstSwitch_;
};

class UIFirstSwitchCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::UI_FIRST_SWITCH;

    UIFirstSwitchCmdModifier(std::weak_ptr<RSNode> node, const UIFirstSwitchCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const UIFirstSwitchCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const UIFirstSwitchCmdParam& GetParam() const
    {
        return param_;
    }

private:
    UIFirstSwitchCmdParam param_;
};

struct OutOfParentCmdParam {
    OutOfParentType outOfParent_;
};

class OutOfParentCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::OUT_OF_PARENT;

    OutOfParentCmdModifier(std::weak_ptr<RSNode> node, const OutOfParentCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const OutOfParentCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const OutOfParentCmdParam& GetParam() const
    {
        return param_;
    }

private:
    OutOfParentCmdParam param_;
};

struct IsCrossNodeCmdParam {
    bool isCrossNode_;
};

class IsCrossNodeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_CROSS_NODE;

    IsCrossNodeCmdModifier(std::weak_ptr<RSNode> node, const IsCrossNodeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const IsCrossNodeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const IsCrossNodeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    IsCrossNodeCmdParam param_;
};

struct NodeGroupCmdParam {
    bool isNodeGroup_;
    bool nodeGroupIsForced_;
    bool nodeGroupIncludeProperty_;
};

class NodeGroupCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::MARK_NODE_GROUP;

    NodeGroupCmdModifier(std::weak_ptr<RSNode> node, const NodeGroupCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const NodeGroupCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const NodeGroupCmdParam& GetParam() const
    {
        return param_;
    }

private:
    NodeGroupCmdParam param_;
};

struct LayerPartRenderCmdParam {
    bool isLayerPartRender_;
};

class LayerPartRenderCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::MARK_LAYER_PART_RENDER;

    LayerPartRenderCmdModifier(std::weak_ptr<RSNode> node, const LayerPartRenderCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const LayerPartRenderCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const LayerPartRenderCmdParam& GetParam() const
    {
        return param_;
    }

private:
    LayerPartRenderCmdParam param_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_NODE_COMMAND_MODIFIER_H
