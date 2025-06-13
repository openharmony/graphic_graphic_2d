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

#ifndef RENDER_SERVICE_BASE_SELF_DRAWING_NODE_MONITOR_H
#define RENDER_SERVICE_BASE_SELF_DRAWING_NODE_MONITOR_H

#include <map>
#include "common/rs_macros.h"
#include "common/rs_common_def.h"
#include "ipc_callbacks/rs_iself_drawing_node_rect_change_callback.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT SelfDrawingNodeMonitor {
public:
    static SelfDrawingNodeMonitor &GetInstance();

    SelfDrawingNodeMonitor(const SelfDrawingNodeMonitor&) = delete;
    SelfDrawingNodeMonitor(const SelfDrawingNodeMonitor&&) = delete;
    SelfDrawingNodeMonitor& operator=(const SelfDrawingNodeMonitor&) = delete;
    SelfDrawingNodeMonitor& operator=(const SelfDrawingNodeMonitor&&) = delete;

    bool IsListeningEnabled() const
    {
        return isListeningEnabled_;
    }

    void InsertCurRectMap(NodeId nodeId, std::string nodeName, RectI rect);
    void EraseCurRectMap(NodeId nodeId);
    void ClearRectMap();

    pid_t GetCallingPid() const;

    void RegisterRectChangeCallback(pid_t pid, sptr<RSISelfDrawingNodeRectChangeCallback> callback);
    void UnRegisterRectChangeCallback(pid_t pid);
    void TriggerRectChangeCallback();

private:
    SelfDrawingNodeMonitor() = default;
    ~SelfDrawingNodeMonitor() noexcept {};
    std::pair<pid_t, sptr<RSISelfDrawingNodeRectChangeCallback>> rectChangeCallbackListenner_;
    SelfDrawingNodeRectCallbackData curRect_;
    SelfDrawingNodeRectCallbackData lastRect_;
    bool isListeningEnabled_ = false;
    pid_t callingPid_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_self_drawing_node_monitor_H