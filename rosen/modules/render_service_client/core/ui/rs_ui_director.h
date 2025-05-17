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

#include "command/rs_animation_command.h"
#include "common/rs_common_def.h"
#include "transaction/rs_irender_client.h"

namespace OHOS {
class Surface;
namespace Rosen {
class RSSurfaceNode;
class RSRootNode;
class RSTransactionData;
class RSUIContext;
using TaskRunner = std::function<void(const std::function<void()>&, uint32_t)>;
using FlushEmptyCallback = std::function<bool(const uint64_t)>;
#ifdef RS_ENABLE_VK
using CommitTransactionCallback =
    std::function<void(std::shared_ptr<RSIRenderClient>&, std::unique_ptr<RSTransactionData>&&, uint32_t&)>;
#endif

class RSC_EXPORT RSUIDirector final {
public:
    static std::shared_ptr<RSUIDirector> Create();

    ~RSUIDirector();
    void GoBackground(bool isTextureExport = false);
    void GoForeground(bool isTextureExport = false);
    void Init(bool shouldCreateRenderThread = true, bool isMultiInstance = false);
    void StartTextureExport();
    void Destroy(bool isTextureExport = false);
    void SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode);
    std::shared_ptr<RSSurfaceNode> GetRSSurfaceNode() const;
    void SetAbilityBGAlpha(uint8_t alpha);
    /**
     * @brief Set rt render status and keep it till set again
     *
     * @param isRenderForced if true, rt will reject partial render and be forced to render all frames
     */
    void SetRTRenderForced(bool isRenderForced);
    void SetContainerWindow(bool hasContainerWindow, RRect rrect);
    void SetFlushEmptyCallback(FlushEmptyCallback flushEmptyCallback);

    void SetRoot(NodeId root);
    void SetUITaskRunner(const TaskRunner& uiTaskRunner, int32_t instanceId = INSTANCE_ID_UNDEFINED,
        bool useMultiInstance = false); // plan to del
    void SendMessages(); // post messages to render thread

    void SetTimeStamp(uint64_t timeStamp, const std::string& abilityName);
    void SetCacheDir(const std::string& cacheFilePath);

    bool FlushAnimation(uint64_t timeStamp, int64_t vsyncPeriod = 0);
    bool HasFirstFrameAnimation();
    void FlushModifier();
    bool HasUIRunningAnimation();
    void FlushAnimationStartTime(uint64_t timeStamp);

    void SetAppFreeze(bool isAppFreeze);

    void SetRequestVsyncCallback(const std::function<void()>& callback);

    static void PostFrameRateTask(const std::function<void()>& task, bool useMultiInstance = false);

    int32_t GetCurrentRefreshRateMode();
    int32_t GetAnimateExpectedRate() const;

    uint32_t GetIndex() const;

    std::shared_ptr<RSUIContext> GetRSUIContext() const;
    void SetRSRootNode(std::shared_ptr<RSRootNode> rootNode);
private:
    void AttachSurface();
    static void RecvMessages();
    static void RecvMessages(std::shared_ptr<RSTransactionData> cmds, bool useMultiInstance = false);
    static void ProcessMessages(std::shared_ptr<RSTransactionData> cmds); // receive message
    static void ProcessMessages(std::shared_ptr<RSTransactionData> cmds, bool useMultiInstance);
    static void AnimationCallbackProcessor(NodeId nodeId, AnimationId animId, uint64_t token,
        AnimationCallbackEvent event);
    static void DumpNodeTreeProcessor(NodeId nodeId, pid_t pid, uint32_t taskId); // DFX to do
    static void PostTask(const std::function<void()>& task, int32_t instanceId = INSTANCE_ID_UNDEFINED); // planing
    static void PostDelayTask(
        const std::function<void()>& task, uint32_t delay = 0, int32_t instanceId = INSTANCE_ID_UNDEFINED); // planing

#ifdef RS_ENABLE_VK
    void InitHybridRender();
    void SetCommitTransactionCallback(CommitTransactionCallback commitTransactionCallback);
#endif

    RSUIDirector() = default;
    RSUIDirector(const RSUIDirector&) = delete;
    RSUIDirector(const RSUIDirector&&) = delete;
    RSUIDirector& operator=(const RSUIDirector&) = delete;
    RSUIDirector& operator=(const RSUIDirector&&) = delete;

    inline static std::unordered_map<RSUIDirector*, TaskRunner> uiTaskRunners_;
    inline static std::mutex uiTaskRunnersVisitorMutex_;

    std::mutex mutex_;
    NodeId root_ = 0;
    int32_t instanceId_ = INSTANCE_ID_UNDEFINED;

    bool isActive_ = false;
    bool isUniRenderEnabled_ = false;
    uint64_t refreshPeriod_ = 16666667;
    uint64_t timeStamp_ = 0;
    uint32_t index_ = 0;
    std::string abilityName_;
    std::weak_ptr<RSSurfaceNode> surfaceNode_;
    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
    std::string cacheDir_;
    static std::function<void()> requestVsyncCallback_;
    bool isHgmConfigChangeCallbackReg_ = false;
    std::shared_ptr<RSUIContext> rsUIContext_ = nullptr;
    std::weak_ptr<RSRootNode> rootNode_;

    friend class RSApplicationAgentImpl;
    friend class RSRenderThread;
    friend class RSImplicitAnimator;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DIRECTOR_H
