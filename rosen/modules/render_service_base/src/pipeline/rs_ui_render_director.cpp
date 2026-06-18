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

#include "pipeline/rs_ui_render_director.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSUIRenderDirector::RSUIRenderDirector(uint64_t token)
    : token_(token), currentState_(RSUIDirectorLifecycleState::CREATE)
{}

void RSUIRenderDirector::OnStateSync(RSUIDirectorLifecycleState state)
{
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (currentState_ == state) {
        return;
    }
    ROSEN_LOGD("RSUIRenderDirector::OnStateSync token:%{public}" PRIu64 " %{public}d -> %{public}d", token_,
        static_cast<int>(currentState_), static_cast<int>(state));
    currentState_ = state;
}

RSUIDirectorLifecycleState RSUIRenderDirector::GetCurrentState() const
{
    std::lock_guard<std::mutex> lock(stateMutex_);
    return currentState_;
}

uint64_t RSUIRenderDirector::GetToken() const
{
    return token_;
}

void RSUIRenderDirector::RegisterRenderNode(NodeId id, const std::shared_ptr<RSBaseRenderNode>& node)
{
    std::lock_guard<std::mutex> lock(nodeMutex_);
    renderNodes_[id] = node;
}

void RSUIRenderDirector::UnregisterRenderNode(NodeId id)
{
    std::lock_guard<std::mutex> lock(nodeMutex_);
    renderNodes_.erase(id);
}

std::shared_ptr<RSBaseRenderNode> RSUIRenderDirector::GetRenderNode(NodeId id) const
{
    std::lock_guard<std::mutex> lock(nodeMutex_);
    auto it = renderNodes_.find(id);
    if (it != renderNodes_.end()) {
        return it->second;
    }
    return nullptr;
}

void RSUIRenderDirector::ClearRenderNodes()
{
    std::lock_guard<std::mutex> lock(nodeMutex_);
    renderNodes_.clear();
}

} // namespace Rosen
} // namespace OHOS
