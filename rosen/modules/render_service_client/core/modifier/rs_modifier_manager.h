/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
 * @file rs_modifier_manager.h
 *
 * @brief Defines the properties and methods for RSModifierManager class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_H

#include <memory>
#include <set>
#include <unordered_map>

#include "animation/rs_animation_rate_decider.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "render_service_base/include/pipeline/rs_render_display_sync.h"

namespace OHOS {
namespace Rosen {
class RSModifier;
class RSRenderAnimation;

/**
 * @class RSModifierManager
 *
 * @brief The class for managing RSModifier instances.
 */
class RSC_EXPORT RSModifierManager {
public:
    /**
     * @brief Constructor for RSModifierManager.
     */
    RSModifierManager() = default;

    /**
     * @brief Destructor for RSModifierManager.
     */
    virtual ~RSModifierManager() = default;

    /**
     * @brief Adds a modifier.
     *
     * @param modifier A shared pointer to the RSModifier to be added.
     */
    void AddModifier(const std::shared_ptr<RSModifier>& modifier);

    /**
     * @brief Adds a animation.
     *
     * @param animation A shared pointer to the RSRenderAnimation to be added.
     */
    void AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation);

    /**
     * @brief Removes a animation.
     *
     * @param keyId The unique identifier of the animation to be removed.
     */
    void RemoveAnimation(const AnimationId keyId);

    /**
     * @brief Checks if there are any UI animations currently running.
     *
     * @return true if at least one UI animation is running; false otherwise.
     */
    bool HasUIRunningAnimation();

    /**
     * @brief Animates the modifier manager based on the given time and optional vsync period.
     *
     * @param time The current time used to update the animation state.
     * @param vsyncPeriod (Optional) The vertical synchronization period. Default is 0.
     * @return true if the animation state was updated successfully; false otherwise.
     */
    bool Animate(int64_t time, int64_t vsyncPeriod = 0);

    /**
     * @brief Draws the modifiers.
     */
    void Draw();

    /**
     * @brief Gets the first frame animation state and resets it.
     *
     * UI animation need this info to get expected frame rate, each window will call it once per frame
     *
     * @return true if the first frame animation was active before the reset; false otherwise.
     */
    bool GetAndResetFirstFrameAnimationState();

    /**
     * @brief Sets the function to get frame rate.
     *
     * @param func The function to be set.
     */
    void SetFrameRateGetFunc(const FrameRateGetFunc& func);

    /**
     * @brief Gets the range of frame rate.
     *
     * @return The range of frame rate.
     */
    const FrameRateRange GetFrameRateRange() const;

    /**
     * @brief Registers a spring animation.
     *
     * @param propertyId The ID of the property associated with the spring animation.
     * @param animId The ID of the animation to be registered.
     */
    void RegisterSpringAnimation(PropertyId propertyId, AnimationId animId);

    /**
     * @brief Unregisters a spring animation.
     *
     * @param propertyId The ID of the property associated with the spring animation.
     * @param animId The ID of the animation to be unregistered.
     */
    void UnregisterSpringAnimation(PropertyId propertyId, AnimationId animId);

    /**
     * @brief Queries a spring animation.
     *
     * @param propertyId The ID of the property associated with the spring animation.
     * @return A shared pointer to the RSRenderAnimation if found; otherwise, nullptr.
     */
    std::shared_ptr<RSRenderAnimation> QuerySpringAnimation(PropertyId propertyId);

    /**
     * @brief Judges whether the animation should be skipped.
     *
     * @param animId The unique identifier of the animation.
     * @param time The current timestamp.
     * @param vsyncPeriod The duration of a single vsync period.
     * @return true if the animation should be skipped for this frame; false otherwise.
     */
    bool JudgeAnimateWhetherSkip(AnimationId animId, int64_t time, int64_t vsyncPeriod);

    /**
     * @brief Sets the display sync enable state.
     *
     * @param isDisplaySyncEnabled true if display sync is enabled; false otherwise.
     */
    void SetDisplaySyncEnable(bool isDisplaySyncEnabled);

    /**
     * @brief Gets the display sync enable state.
     *
     * @return true if display sync is enabled; false otherwise.
     */
    bool IsDisplaySyncEnabled() const;

    /**
     * @brief Refresh the start time of the animation
     *
     * If the animation needs to update its start time, its start time will be set to time.
     *
     * @param time The new start time for the animation.
     */
    void FlushStartAnimation(int64_t time);

private:
    /**
     * @brief Callback invoked when a animation has finished.
     *
     * @param animation A shared pointer to the RSRenderAnimation that has finished.
     */
    void OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation);

    /**
     * @brief Gets the animation associated with the specified ID.
     *
     * @param id The ID of the animation.
     * @return A shared pointer to the RSRenderAnimation if found; otherwise, nullptr.
     */
    const std::shared_ptr<RSRenderAnimation> GetAnimation(AnimationId id) const;

    std::set<std::shared_ptr<RSModifier>> modifiers_;
    std::unordered_map<AnimationId, std::weak_ptr<RSRenderAnimation>> animations_;
    std::unordered_map<PropertyId, AnimationId> springAnimations_;

    RSAnimationRateDecider rateDecider_;
    FrameRateGetFunc frameRateGetFunc_;
    bool hasFirstFrameAnimation_ = false;

    std::unordered_map<AnimationId, std::shared_ptr<RSRenderDisplaySync>> displaySyncs_;
    bool isDisplaySyncEnabled_ = false;

    template <typename T>
    friend class RSAnimatableProperty;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_H
