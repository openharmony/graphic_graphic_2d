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

#include "command/rs_ui_director_command.h"

#include "common/rs_common_def.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_ui_render_director.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

void RSUIDirectorCommandHelper::GoCreate(RSContext& context, NodeId nodeId, uint64_t token)
{
    pid_t pid = ExtractPid(nodeId);
    auto director = context.GetUIRenderDirector(pid, token);
    if (director != nullptr) {
        RS_LOGD("RSUIDirectorCommandHelper::GoCreate found director for token: %{public}" PRIu64, token);
        return;
    }
    context.CreateUIRenderDirector(pid, token);
    director = context.GetUIRenderDirector(pid, token);
    if (director == nullptr) {
        ROSEN_LOGE("RSUIDirectorCommandHelper::GoCreate failed to create director for token: %{public}" PRIu64, token);
        return;
    }
    director->OnStateSync(static_cast<RSUIDirectorLifecycleState>(RSUIDirectorLifecycleState::CREATE));
}

void RSUIDirectorCommandHelper::GoResume(RSContext& context, NodeId nodeId, uint64_t token)
{
    auto director = context.GetUIRenderDirector(ExtractPid(nodeId), token);
    if (director == nullptr) {
        RS_LOGE("RSUIDirectorCommandHelper::GoResume failed to find director for token: %{public}" PRIu64, token);
        return;
    }
    director->OnStateSync(static_cast<RSUIDirectorLifecycleState>(RSUIDirectorLifecycleState::RESUME));
}

void RSUIDirectorCommandHelper::GoForeground(RSContext& context, NodeId nodeId, uint64_t token)
{
    auto director = context.GetUIRenderDirector(ExtractPid(nodeId), token);
    if (director == nullptr) {
        RS_LOGE("RSUIDirectorCommandHelper::GoForeground failed to find director for token: %{public}" PRIu64, token);
        return;
    }
    director->OnStateSync(static_cast<RSUIDirectorLifecycleState>(RSUIDirectorLifecycleState::FOREGROUND));
}

void RSUIDirectorCommandHelper::GoBackground(RSContext& context, NodeId nodeId, uint64_t token)
{
    auto director = context.GetUIRenderDirector(ExtractPid(nodeId), token);
    if (director == nullptr) {
        RS_LOGE("RSUIDirectorCommandHelper::GoBackground failed to find director for token: %{public}" PRIu64, token);
        return;
    }
    director->OnStateSync(static_cast<RSUIDirectorLifecycleState>(RSUIDirectorLifecycleState::BACKGROUND));
}

void RSUIDirectorCommandHelper::GoStop(RSContext& context, NodeId nodeId, uint64_t token)
{
    pid_t pid = ExtractPid(nodeId);
    context.GetMutableNodeMap().DestroyTokenNode(pid, token);
    auto director = context.GetUIRenderDirector(pid, token);
    if (director == nullptr) {
        RS_LOGE("RSUIDirectorCommandHelper::GoStop failed to find director for token: %{public}" PRIu64, token);
        return;
    }
    director->OnStateSync(static_cast<RSUIDirectorLifecycleState>(RSUIDirectorLifecycleState::STOP));
}

void RSUIDirectorCommandHelper::GoDestroy(RSContext& context, NodeId nodeId, uint64_t token)
{
    pid_t pid = ExtractPid(nodeId);
    auto director = context.GetUIRenderDirector(pid, token);
    if (director == nullptr) {
        ROSEN_LOGW("RSUIDirectorCommandHelper::GoDestroy failed to find director for token: %{public}" PRIu64, token);
        return;
    }

    director->OnStateSync(static_cast<RSUIDirectorLifecycleState>(RSUIDirectorLifecycleState::DESTROYED));
    context.DestroyUIRenderDirector(pid, token);
}

} // namespace Rosen
} // namespace OHOS