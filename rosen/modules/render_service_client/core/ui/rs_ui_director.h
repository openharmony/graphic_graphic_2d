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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DIRECTOR_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DIRECTOR_H

#include <mutex>
#include <queue>
#include <vector>

#include "common/rs_common_def.h"
namespace OHOS {
class Surface;
namespace Rosen {
class RSCommand;
class RSSurfaceNode;
using TaskRunner = std::function<void(const std::function<void()>&)>;

class RS_EXPORT RSUIDirector final {
public:
    static std::shared_ptr<RSUIDirector> Create();

    ~RSUIDirector();
    void GoBackground();
    void GoForeground();
    void Init();
    void SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode);
    void SetSurfaceNodeSize(int width, int height);

    void SetRoot(NodeId root);
    void SetUITaskRunner(const TaskRunner& uiTaskRunner);
    void SendMessages(); // post messages to render thread

    void SetTimeStamp(uint64_t timeStamp);

private:
    void Destory();
    static void RecvMessages(); // receive message
    static void ProcessMessages(std::queue<std::shared_ptr<RSCommand>> cmds); // receive message


    RSUIDirector() = default;
    RSUIDirector(const RSUIDirector&) = delete;
    RSUIDirector(const RSUIDirector&&) = delete;
    RSUIDirector& operator=(const RSUIDirector&) = delete;
    RSUIDirector& operator=(const RSUIDirector&&) = delete;

    std::mutex mutex_;
    NodeId root_ = 0;

    uint64_t refreshPeriod_ = 16666667;
    uint64_t timeStamp_ = 0;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
    friend class RSRenderThread;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DIRECTOR_H