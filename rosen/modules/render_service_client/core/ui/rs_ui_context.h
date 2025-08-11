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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_ui_context.h
 *
 * @brief Defines the properties and methods for RSUIContext class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_H

#include <functional>
#include <memory>
#include <mutex>

#include "animation/rs_animation.h"
#include "animation/rs_implicit_animator.h"
#include "common/rs_common_def.h"
#include "modifier/rs_modifier_manager.h"
#include "pipeline/rs_node_map_v2.h"
#include "transaction/rs_transaction_handler.h"

namespace OHOS {
namespace Rosen {
using TaskRunner = std::function<void(const std::function<void()>&, uint32_t)>;

/**
 * @class RSUIContext
 *
 * @brief Represents the context for UI operations in the rendering service.
 */
class RSC_EXPORT RSUIContext : public std::enable_shared_from_this<RSUIContext> {
public:
    /**
     * @brief Default constructor for RSUIContext.
     */
    virtual ~RSUIContext();

    /**
     * @brief Gets node map.
     * 
     * This function will be called when there are multiple instances of arkui.
     * 
     * @return A reference to the mutable RSNodeMapV2 instance.
     */
    inline RSNodeMapV2& GetMutableNodeMap()
    {
        return nodeMap_;
    }

    /**
     * @brief Gets node map.
     * 
     * This function will be called when there is a single instance of arkui.
     * 
     * @return A reference to the RSNodeMapV2 instance.
     */
    inline const RSNodeMapV2& GetNodeMap() const
    {
        return nodeMap_;
    }

    /**
     * @brief Gets the RSTransactionHandler associated with this context.
     *
     * @return A shared pointer to the RSTransactionHandler instance.
     */
    inline std::shared_ptr<RSTransactionHandler> GetRSTransaction() const
    {
        return rsTransactionHandler_;
    }

    /**
     * @brief Gets the RSSyncTransactionHandler associated with this context.
     *
     * @return A shared pointer to the RSSyncTransactionHandler instance.
     */
    inline std::shared_ptr<RSSyncTransactionHandler> GetSyncTransactionHandler() const
    {
        return rsSyncTransactionHandler_;
    }

    /**
     * @brief Gets the RSImplicitAnimator associated with this context.
     *
     * @return A shared pointer to the RSImplicitAnimator instance.
     */
    const std::shared_ptr<RSImplicitAnimator> GetRSImplicitAnimator();

    /**
     * @brief Gets the RSModifierManager associated with this context.
     *
     * @return A shared pointer to the RSModifierManager instance.
     */
    const std::shared_ptr<RSModifierManager> GetRSModifierManager();

    /**
     * @brief Gets the RSImplicitAnimParam associated with this context.
     *
     * @return A shared pointer to the RSImplicitAnimParam instance.
     */
    bool AnimationCallback(AnimationId animationId, AnimationCallbackEvent event);

    /**
     * @brief Adds an animation to the context.
     *
     * @param animation The shared pointer to the RSAnimation to be added.
     */
    void AddAnimationInner(const std::shared_ptr<RSAnimation>& animation);

    /**
     * @brief Removes animations from the context.
     */
    void RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation);

    /**
     * @brief Gets the token associated with the current context.
     * 
     * @return The token as a 64-bit unsigned integer.
     */
    inline uint64_t GetToken() const
    {
        return token_;
    }

    /**
     * @brief Sets the task runner for the UI thread.
     *
     * @param uiTaskRunner The task runner to be set.
     */
    void SetUITaskRunner(const TaskRunner& uiTaskRunner);

    /**
     * @brief Posts a task to the UI thread.
     *
     * @param task The task to be posted.
     */
    void PostTask(const std::function<void()>& task);

    /**
     * @brief Posts a delayed task to the UI thread.
     *
     * @param task The task to be posted.
     * @param delay The delay before executing the task.
     */
    void PostDelayTask(const std::function<void()>& task, uint32_t delay);

    /**
     * @brief Checks if the task runner is set.
     *
     * @return true if the task runner is set; false otherwise.
     */
    inline bool HasTaskRunner()
    {
        return bool(taskRunner_);
    }

private:
    RSUIContext();
    RSUIContext(uint64_t token);
    RSUIContext(const RSUIContext&) = delete;
    RSUIContext(const RSUIContext&&) = delete;
    RSUIContext& operator=(const RSUIContext&) = delete;
    RSUIContext& operator=(const RSUIContext&&) = delete;

    void DumpNodeTreeProcessor(std::string& out, NodeId nodeId, pid_t pid, uint32_t taskId);

    uint64_t token_;

    RSNodeMapV2 nodeMap_;
    std::shared_ptr<RSTransactionHandler> rsTransactionHandler_;
    std::shared_ptr<RSSyncTransactionHandler> rsSyncTransactionHandler_;
    std::unordered_map<pid_t, std::shared_ptr<RSImplicitAnimator>> rsImplicitAnimators_;
    std::shared_ptr<RSModifierManager> rsModifierManager_;

    std::unordered_map<AnimationId, std::shared_ptr<RSAnimation>> animations_;
    std::unordered_map<PropertyId, uint32_t> animatingPropertyNum_;
    std::recursive_mutex animationMutex_;

    TaskRunner taskRunner_ = TaskRunner();
    std::mutex implicitAnimatorMutex_;

    friend class RSUIContextManager;
    friend class RSUIDirector;
};

} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_H