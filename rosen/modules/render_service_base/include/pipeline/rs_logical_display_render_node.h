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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_LOGICAL_DISPLAY_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_LOGICAL_DISPLAY_RENDER_NODE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_special_layer_manager.h"

#include "params/rs_logical_display_render_params.h"
#include "pipeline/rs_render_node.h"


namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSLogicalDisplayRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSLogicalDisplayRenderNode>;
    using SharedPtr = std::shared_ptr<RSLogicalDisplayRenderNode>;
    static inline constexpr auto Type = RSRenderNodeType::LOGICAL_DISPLAY_NODE;

    ~RSLogicalDisplayRenderNode() override;

    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void UpdateRenderParams() override;

    RSRenderNode::ChildrenListSharedPtr GetSortedChildren() const override;

    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    void SetScreenId(ScreenId screenId)
    {
        screenId_ = screenId;
    }

    ScreenId GetScreenId() const
    {
        return screenId_;
    }

    bool IsValidScreenId() const
    {
        return screenId_ != INVALID_SCREEN_ID;
    }

    void SetScbNodePid(const std::vector<int32_t>& oldScbPids, int32_t currentScbPid)
    {
        oldScbPids_ = oldScbPids;
        lastScbPid_ = currentScbPid_;
        currentScbPid_ = currentScbPid;
        isNeedWaitNewScbPid_ = true;
        isFullChildrenListValid_ = false;
    }

    std::vector<int32_t> GetOldScbPids() const
    {
        return oldScbPids_;
    }

    int32_t GetCurrentScbPid() const
    {
        return currentScbPid_;
    }

    void SetScreenRotation(const ScreenRotation& screenRotation)
    {
        screenRotation_ = screenRotation;
    }

    ScreenRotation GetScreenRotation()
    {
        return screenRotation_;
    }

    void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID,
        NodeId uifirstRootNodeId = INVALID_NODEID, NodeId screenNodeId = INVALID_NODEID,
        NodeId logicalDisplayNodeId = INVALID_NODEID) override;
    
    // Window Container
    void SetWindowContainer(std::shared_ptr<RSBaseRenderNode> container);
    std::shared_ptr<RSBaseRenderNode> GetWindowContainer() const;

    using ScreenStatusNotifyTask = std::function<void(bool)>;

    static void SetScreenStatusNotifyTask(ScreenStatusNotifyTask task);

    void NotifyScreenNotSwitching();

    ScreenRotation GetRotation() const;
    void UpdateRotation();
    bool IsRotationChanged() const;
    bool IsLastRotationChanged() const;
    bool GetPreRotationStatus() const;
    bool GetCurRotationStatus() const;

    void UpdateOffscreenRenderParams(bool needOffscreen);

    void SetSecurityDisplay(bool isSecurityDisplay);
    bool GetSecurityDisplay() const;
    void SetMirrorSource(SharedPtr node);
    void ResetMirrorSource();
    WeakPtr GetMirrorSource() const;
    void SetIsMirrorDisplay(bool isMirror);
    bool IsMirrorDisplay() const;
    bool IsMirrorDisplayChanged() const;
    void ResetMirrorDisplayChangedFlag();
    void SetVirtualScreenMuteStatus(bool virtualScreenMuteStatus);
    bool GetVirtualScreenMuteStatus() const;
    void SetDisplaySpecialSurfaceChanged(bool displaySpecialSurfaceChanged);

    RSSpecialLayerManager& GetMultableSpecialLayerMgr();
    const RSSpecialLayerManager& GetSpecialLayerMgr() const;

    void AddSecurityLayer(NodeId id);
    void ClearSecurityLayerList();
    const std::vector<NodeId>& GetSecurityLayerList();

    void AddSecurityVisibleLayer(NodeId id);
    void ClearSecurityVisibleLayerList();
    const std::vector<NodeId>& GetSecurityVisibleLayerList();

    void SetSecurityExemption(bool isSecurityExemption);
    bool GetSecurityExemption() const;
    void SetHasSecLayerInVisibleRect(bool hasSecLayer);

    void SetCompositeType(CompositeType type);
    CompositeType GetCompositeType() const;

    void SetHasCaptureWindow(bool hasCaptureWindow)
    {
        hasCaptureWindow_ = hasCaptureWindow;
    }

    void SetAncestorScreenNode(const RSBaseRenderNode::WeakPtr& ancestorScreenNode)
    {
        ancestorScreenNode_ = ancestorScreenNode;
    }

    RSBaseRenderNode::WeakPtr GetAncestorScreenNode()
    {
        return ancestorScreenNode_;
    }

    void NotifySetOnTreeFlag()
    {
        waitToSetOnTree_ = true;
    }

    void ResetSetOnTreeFlag()
    {
        waitToSetOnTree_ = false;
    }

    bool IsWaitToSetOnTree()
    {
        return waitToSetOnTree_;
    }

protected:
    void OnSync() override;
private:
    explicit RSLogicalDisplayRenderNode(NodeId id, const RSDisplayNodeConfig& config,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);
    void InitRenderParams() override;

    ScreenId screenId_ = INVALID_SCREEN_ID;

    // bounds rotation
    bool preRotationStatus_ = false;
    bool curRotationStatus_ = false;
    bool lastRotationChanged_ = false;
    float lastRotation_ = 0.f;

    int32_t currentScbPid_ = -1;
    int32_t lastScbPid_ = -1;
    std::vector<int32_t> oldScbPids_ {};
    mutable bool isNeedWaitNewScbPid_ = false;
    mutable std::shared_ptr<std::vector<std::shared_ptr<RSRenderNode>>> currentChildrenList_ =
        std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    
    ScreenRotation screenRotation_ = ScreenRotation::ROTATION_0;

    bool isSecurityDisplay_ = false;
    bool isMirrorDisplay_ = false;
    WeakPtr mirrorSource_;
    bool isMirrorDisplayChanged_ = false;
    bool virtualScreenMuteStatus_ = false;
    bool isSecurityExemption_ = false;
    bool hasSecLayerInVisibleRect_ = false;
    bool hasSecLayerInVisibleRectChanged_ = false;
    CompositeType compositeType_ = CompositeType::HARDWARE_COMPOSITE;
    bool hasCaptureWindow_ = false;

    bool waitToSetOnTree_ = false;

    // Use in virtual screen security exemption
    std::vector<NodeId> securityLayerList_;  // leashPersistentId and surface node id
    RSSpecialLayerManager specialLayerManager_;
    bool displaySpecialSurfaceChanged_ = false;
    // Use in mirror screen visible rect projection
    std::vector<NodeId> securityVisibleLayerList_;  // surface node id

    RSBaseRenderNode::WeakPtr ancestorScreenNode_;
    // window Container
    std::shared_ptr<RSBaseRenderNode> windowContainer_;

    static inline ScreenStatusNotifyTask screenStatusNotifyTask_ = nullptr;

    friend class DisplayNodeCommandHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_LOGICAL_DISPLAY_RENDER_NODE_H
