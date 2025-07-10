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

} // namespace Rosen
} // namespace OHOS
