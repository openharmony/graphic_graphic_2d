/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_display_render_node.h"

#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSDisplayRenderNode::RSDisplayRenderNode(NodeId id, const RSDisplayNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSBaseRenderNode(id, context), screenId_(config.screenId), isMirroredDisplay_(config.isMirrored)
{}

RSDisplayRenderNode::~RSDisplayRenderNode() {}

void RSDisplayRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareDisplayRenderNode(*this);
}

void RSDisplayRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessDisplayRenderNode(*this);
}

RSDisplayRenderNode::CompositeType RSDisplayRenderNode::GetCompositeType() const
{
    return compositeType_;
}

void RSDisplayRenderNode::SetCompositeType(RSDisplayRenderNode::CompositeType type)
{
    compositeType_ = type;
}

void RSDisplayRenderNode::SetForceSoftComposite(bool flag)
{
    forceSoftComposite_ = flag;
}

bool RSDisplayRenderNode::IsForceSoftComposite() const
{
    return forceSoftComposite_;
}

void RSDisplayRenderNode::SetMirrorSource(SharedPtr node)
{
    if (!isMirroredDisplay_ || node == nullptr) {
        return;
    }
    mirrorSource_ = node;
}

bool RSDisplayRenderNode::IsMirrorDisplay() const
{
    return isMirroredDisplay_;
}

} // namespace Rosen
} // namespace OHOS
