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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_ui_director.h
 *
 * @brief Defines the properties and methods for RSUIDirector class.
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

/**
 * @class RSUIDirector
 *
 * @brief The class for managing the UI director in the rendering service.
 */
class RSC_EXPORT RSUIDirector final {
public:
    /**
     * @brief Creates and returns a shared pointer to an instance of RSUIDirector.
     *
     * @return A std::shared_ptr pointing to the newly created RSUIDirector instance.
     */
    static std::shared_ptr<RSUIDirector> Create();

    /**
     * @brief Destructor for RSUIDirector.
     */
    ~RSUIDirector();

    /**
     * @brief Moves the UI director to the background.
     * 
     * Update the window status, node status, and clear cache and redundant resources
     * 
     * @param isTextureExport Indicates whether texture export should be performed 
     *                        during the transition to the background. Defaults to false.
     */
    void GoBackground(bool isTextureExport = false);

    /**
     * @brief Moves the UI director to the foreground.
     *
     * @param isTextureExport Indicates whether texture export should be performed 
     *                        during the transition to the foreground. Defaults to false.
     */
    void GoForeground(bool isTextureExport = false);

    /**
     * @brief Initializes the RSUIDirector instance.
     *
     * @param shouldCreateRenderThread Indicates whether a render thread should be created. Defaults to true.
     * @param isMultiInstance Indicates whether the instance supports multiple instances. Defaults to false.
     */
    void Init(bool shouldCreateRenderThread = true, bool isMultiInstance = false);

    /**
     * @brief Initiates the process of exporting texture data.
     */
    void StartTextureExport();

    /**
     * @brief Destroy RSUIDirector instance.
     */
    void Destroy(bool isTextureExport = false);

    /**
     * @brief Sets the surfaceNode for the RSUIDirector.
     *
     * @param surfaceNode A shared pointer to the Surface object to be set.
     */
    void SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode);
    std::shared_ptr<RSSurfaceNode> GetRSSurfaceNode() const;

    /**
     * @brief Sets the background alpha value for the ability.
     *
     * @param alpha The alpha value to set.
     */
    void SetAbilityBGAlpha(uint8_t alpha);

    /**
     * @brief Set rt render status and keep it till set again
     *
     * @param isRenderForced if true, rt will reject partial render and be forced to render all frames
     */
    void SetRTRenderForced(bool isRenderForced);

    /**
     * @brief Sets the container window properties.
     *
     * @param hasContainerWindow A boolean indicating whether the container window is present.
     * @param rrect The rounded rectangle (RRect) defining the bounds of the container window.
     */
    void SetContainerWindow(bool hasContainerWindow, RRect rrect);

    /**
     * @brief Sets the callback function to be invoked when a flush operation results in no changes.
     *
     * @param flushEmptyCallback The callback function to be executed on an empty flush.
     */
    void SetFlushEmptyCallback(FlushEmptyCallback flushEmptyCallback);

    /**
     * @brief Sets the root node for the UI director.
     *
     * It is recommended to use the SetRSRootNode interface, as the SetRoot interface is planned to be deprecated.
     *
     * @param root The ID of the node to be set as the root.
     */
    void SetRoot(NodeId root);

    /**
     * @brief Sets the UI task runner for the specified instance.
     *
     * plan to del.
     *
     * @param uiTaskRunner The task runner to be used for UI tasks.
     * @param instanceId The ID of the instance for which the task runner is being set. 
     *                   Defaults to INSTANCE_ID_UNDEFINED if not specified.
     * @param useMultiInstance A flag indicating whether to use multiple instances. Defaults to false.
     */
    void SetUITaskRunner(const TaskRunner& uiTaskRunner, int32_t instanceId = INSTANCE_ID_UNDEFINED,
        bool useMultiInstance = false);

    /**
     * @brief Post messages to render thread.
     */
    void SendMessages();
    
    /**
     * @brief Sets the timestamp and associates it with a specific ability name.
     *
     * @param timeStamp The timestamp value to be set.
     * @param abilityName The name of the ability to associate with the timestamp.
     */

      /**
     * @brief Post messages to render thread.
     */
    void SendMessages(std::function<void()> callback);
    
    /**
     * @brief Sets the timestamp and associates it with a specific ability name.
     *
     * @param timeStamp The timestamp value to be set.
     * @param abilityName The name of the ability to associate with the timestamp.
     */
    void SetTimeStamp(uint64_t timeStamp, const std::string& abilityName);

    /**
     * @brief Sets the directory path for caching files.
     *
     * @param cacheFilePath The file path to the cache directory.
     */
    void SetCacheDir(const std::string& cacheFilePath);

    /**
     * @brief Flushes the animation updates to the rendering pipeline.
     *
     * @param timeStamp The current timestamp.
     * @param vsyncPeriod The vertical synchronization period, in nanoseconds. defaults to 0 if not provided.
     * @return true if the animation flush was successful, false otherwise.
     */
    bool FlushAnimation(uint64_t timeStamp, int64_t vsyncPeriod = 0);

    /**
     * @brief Checks if the first frame animation is present.
     *
     * @return true if the first frame animation exists, false otherwise.
     */
    bool HasFirstFrameAnimation();

    /**
     * @brief Flushes the modifier operations to ensure that all pending changes are applied.
     *
     * Sends the modifier commands to the render service for drawing.
     */
    void FlushModifier();

    /**
     * @brief Checks if there are any UI animations currently running.
     * 
     * @return true if there are running UI animations, false otherwise.
     */
    bool HasUIRunningAnimation();

    /**
     * @brief Updates the start time of animations with the specified timestamp.
     *
     * @param timeStamp The timestamp (in microseconds) to set as the start time for animations.
     */
    void FlushAnimationStartTime(uint64_t timeStamp);

    /**
     * @brief Sets the application freeze state.
     *
     * @param isAppFreeze Indicates the freeze state of the application.
     *                    True if the application is frozen; false otherwise.
     */
    void SetAppFreeze(bool isAppFreeze);

    /**
     * @brief Sets the callback function to be invoked when a vsync request is made.
     *
     * @param callback A std::function object representing the callback to be executed.
     */
    void SetRequestVsyncCallback(const std::function<void()>& callback);

    /**
     * @brief Posts a task to handle frame rate operations.
     *
     * @param task The task to be executed, represented as a std::function<void()>.
     * @param useMultiInstance Optional parameter indicating whether to use multiple
     *                         instances for the task execution. Defaults to false.
     */
    static void PostFrameRateTask(const std::function<void()>& task, bool useMultiInstance = false);

    /**
     * @brief Gets the current refresh rate mode of the UI director.
     * 
     * @return int32_t The current refresh rate mode.
     */
    int32_t GetCurrentRefreshRateMode();

    /**
     * @brief Gets the expected animation frame rate.
     * 
     * @return The expected frame rate for animations as an integer.
     */
    int32_t GetAnimateExpectedRate() const;

    /**
     * @brief Gets the index associated with the current object.
     * 
     * @return The index as an unsigned 32-bit integer.
     */
    uint32_t GetIndex() const;

    /**
     * @brief Gets the RSUIContext associated with this RSUIDirector.
     * 
     * @return A shared pointer to the RSUIContext instance.
     */
    std::shared_ptr<RSUIContext> GetRSUIContext() const;

    /**
     * @brief Sets the root node for the UI director.
     *
     * @param rootNode A std::shared_ptr pointing to the RSRootNode object to be set.
     */
    void SetRSRootNode(std::shared_ptr<RSRootNode> rootNode);

    /**
     * @brief Identify typical resident processes of the system, such as FSR, SCB, inputMethod.
     *
     * @param isTypicalResidentProcess means whether the relevant services are disabled.
     */
    static void SetTypicalResidentProcess(bool isTypicalResidentProcess = false);

private:
    void ReportUiSkipEvent(const std::string& abilityName);
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
    static void SetTypicalResidentProcessOnce(bool isResidentProcess);

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
    int64_t lastUiSkipTimestamp_ = 0; // ms
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

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DIRECTOR_H
