/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <atomic>
#include <functional>
#include <mutex>

#include "common/rs_common_def.h"

namespace OHOS {
class Surface;
namespace Rosen {
class RSSurfaceNode;
class RSTransactionData;
using TaskRunner = std::function<void(const std::function<void()>&)>;

class RSC_EXPORT RSUIDirector final {
public:
    static std::shared_ptr<RSUIDirector> Create();

    ~RSUIDirector();
    void GoBackground();
    void GoForeground();
    void Init(bool shouldCreateRenderThread = true);
    void Destroy();
    void SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode);
    void SetAbilityBGAlpha(uint8_t alpha);
    /**
     * @brief Set rt render status and keep it till set again
     *
     * @param isRenderForced if true, rt will reject partial render and be forced to render all frames
     */
    void SetRTRenderForced(bool isRenderForced);
    void SetContainerWindow(bool hasContainerWindow, float density);

    void SetRoot(NodeId root);
    void SetUITaskRunner(const TaskRunner& uiTaskRunner);
    void SendMessages(); // post messages to render thread

    void SetTimeStamp(uint64_t timeStamp, const std::string& abilityName);
    void SetCacheDir(const std::string& cacheFilePath);

    bool RunningCustomAnimation(uint64_t timeStamp);

    void SetAppFreeze(bool isAppFreeze);

private:
    void AttachSurface();
    static void RecvMessages(bool needProcess = true);
    static void RecvMessages(std::shared_ptr<RSTransactionData> cmds);
    static void ProcessMessages(std::shared_ptr<RSTransactionData> cmds); // receive message
    static void AnimationCallbackProcessor(NodeId nodeId, AnimationId animId);

    RSUIDirector() = default;
    RSUIDirector(const RSUIDirector&) = delete;
    RSUIDirector(const RSUIDirector&&) = delete;
    RSUIDirector& operator=(const RSUIDirector&) = delete;
    RSUIDirector& operator=(const RSUIDirector&&) = delete;

    std::mutex mutex_;
    NodeId root_ = 0;

    bool isActive_ = false;
    uint64_t refreshPeriod_ = 16666667;
    uint64_t timeStamp_ = 0;
    std::string abilityName_;
    std::weak_ptr<RSSurfaceNode> surfaceNode_;
    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
    std::string cacheDir_;

    friend class RSRenderThread;
    friend class RSApplicationAgentImpl;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DIRECTOR_H
