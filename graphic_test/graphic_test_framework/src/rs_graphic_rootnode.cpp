/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_graphic_rootnode.h"
#include "transaction/rs_interfaces.h"
#include "rs_graphic_errors.h"
#include "rs_graphic_log.h"

namespace OHOS {
namespace Rosen {
RSGraphicRootNode::RSGraphicRootNode()
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestScreenSurface";
    screenSurfaceNode_ = RSSurfaceNode::Create(config, RSSurfaceNodeType::LEASH_WINDOW_NODE);
}

void RSGraphicRootNode::SetTestSurface(
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> node)
{
    testSurfaceNodes_.push_back(node);
    screenSurfaceNode_->AddChild(testSurfaceNodes_.back(), -1);
}

void RSGraphicRootNode::ResetTestSurface()
{
    screenSurfaceNode_->ClearChildren();
    testSurfaceNodes_.clear();
    nodes_.clear();
}

void RSGraphicRootNode::AddChild(std::shared_ptr<RSNode> child, int index)
{
    if (testSurfaceNodes_.back()) {
        testSurfaceNodes_.back()->AddChild(child, index);
    }
}

void RSGraphicRootNode::RemoveChild(std::shared_ptr<RSNode> child)
{
    if (testSurfaceNode_) {
        testSurfaceNode_->RemoveChild(child);
    }
}

void RSGraphicRootNode::ClearChildren()
{
    if (testSurfaceNode_) {
        testSurfaceNode_->ClearChildren();
    }
}

void RSGraphicRootNode::RegisterNode(std::shared_ptr<RSNode> node)
{
    nodes_.push_back(node);
}

void RSGraphicRootNode::SetScreenId(ScreenId id)
{
    screenId_ = id;
}

void RSGraphicRootNode::SetMainWindowZ(float mainZ)
{
    mainWindowZ_ = mainZ;
}

float RSGraphicRootNode::GetMainWindowZ() const
{
    return mainWindowZ_;
}

std::shared_ptr<RSSurfaceNode> RSGraphicRootNode::CreateContentNode(
    SubWindowId id, const SubWindowOptions& options)
{
    RSSurfaceNodeConfig contentConfig;
    contentConfig.SurfaceNodeName =
        "SubWindowSurface_" + std::to_string(id);
    auto contentNode = RSSurfaceNode::Create(
        contentConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
    if (!contentNode) {
        LOGE("RSGraphicRootNode::CreateContentNode create failed");
        return nullptr;
    }

    constexpr int BOUNDS_WIDTH_INDEX = 2;
    constexpr int BOUNDS_HEIGHT_INDEX = 3;
    float width = options.bounds[BOUNDS_WIDTH_INDEX];
    float height = options.bounds[BOUNDS_HEIGHT_INDEX];
    contentNode->SetBounds({0, 0, width, height});
    contentNode->SetFrame({0, 0, width, height});
    contentNode->SetBackgroundColor(options.contentBgColor);
    return contentNode;
}

SubWindowId RSGraphicRootNode::CreateSubWindow(const SubWindowOptions& options)
{
    if (!screenSurfaceNode_) {
        LOGE("RSGraphicRootNode::CreateSubWindow screenSurfaceNode_ is null");
        return INVALID_SUB_WINDOW_ID;
    }

    SubWindowId id = nextSubWindowId_++;

    RSSurfaceNodeConfig leashConfig;
    leashConfig.SurfaceNodeName = "SubWindow_" + std::to_string(id);
    auto leashNode = RSSurfaceNode::Create(
        leashConfig, RSSurfaceNodeType::LEASH_WINDOW_NODE);
    if (!leashNode) {
        LOGE("RSGraphicRootNode::CreateSubWindow create leash node failed");
        return INVALID_SUB_WINDOW_ID;
    }

    leashNode->SetBounds(options.bounds);
    leashNode->SetFrame(options.bounds);
    leashNode->SetBackgroundColor(0);
    leashNode->AttachToDisplay(screenId_);

    float zValue = 0.0f;
    switch (options.order) {
        case SubWindowOrder::ABOVE_MAIN:
            zValue = mainWindowZ_ + 1.0f;
            break;
        case SubWindowOrder::BELOW_MAIN:
            zValue = mainWindowZ_ - 1.0f;
            break;
        case SubWindowOrder::ABSOLUTE:
            zValue = options.absoluteZ;
            break;
    }
    leashNode->SetPositionZ(zValue);

    auto contentNode = CreateContentNode(id, options);
    if (!contentNode) {
        leashNode->RemoveFromTree();
        return INVALID_SUB_WINDOW_ID;
    }

    leashNode->AddChild(contentNode, -1);
    subWindows_[id] = {leashNode, contentNode};
    return id;
}

std::shared_ptr<RSSurfaceNode> RSGraphicRootNode::GetSubWindow(
    SubWindowId id) const
{
    auto it = subWindows_.find(id);
    if (it == subWindows_.end()) {
        LOGE("RSGraphicRootNode::GetSubWindow invalid id %{public}u", id);
        return nullptr;
    }
    return it->second.leashNode;
}

std::shared_ptr<RSSurfaceNode> RSGraphicRootNode::GetSubWindowTestSurface(
    SubWindowId id) const
{
    auto it = subWindows_.find(id);
    if (it == subWindows_.end()) {
        LOGE("RSGraphicRootNode::GetSubWindowTestSurface invalid id "
            "%{public}u", id);
        return nullptr;
    }
    return it->second.contentNode;
}

bool RSGraphicRootNode::AddChildToSubWindow(
    SubWindowId id, std::shared_ptr<RSNode> child, int childIndex)
{
    auto it = subWindows_.find(id);
    if (it == subWindows_.end()) {
        LOGE("RSGraphicRootNode::AddChildToSubWindow invalid id "
            "%{public}u", id);
        return false;
    }
    if (!it->second.contentNode) {
        LOGE("RSGraphicRootNode::AddChildToSubWindow content node is null");
        return false;
    }
    it->second.contentNode->AddChild(child, childIndex);
    return true;
}

bool RSGraphicRootNode::RemoveSubWindow(SubWindowId id)
{
    auto it = subWindows_.find(id);
    if (it == subWindows_.end()) {
        LOGE("RSGraphicRootNode::RemoveSubWindow invalid id %{public}u", id);
        return false;
    }
    it->second.leashNode->ClearChildren();
    it->second.leashNode->RemoveFromTree();
    subWindows_.erase(it);
    return true;
}

void RSGraphicRootNode::ResetSubWindows()
{
    for (auto& [id, entry] : subWindows_) {
        if (entry.leashNode) {
            entry.leashNode->ClearChildren();
            entry.leashNode->RemoveFromTree();
        }
    }
    subWindows_.clear();
}

bool RSGraphicRootNode::HasSubWindows() const
{
    return !subWindows_.empty();
}

} // namespace Rosen
} // namespace OHOS
