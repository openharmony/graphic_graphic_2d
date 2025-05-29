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
 * @file rs_ui_context_manager.h
 *
 * @brief Defines the properties and methods for RSUIContextManager class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_MANAGER_H

#include "rs_ui_context.h"

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
/**
 * @class RSUIContextManager
 *
 * @brief Manages the lifecycle and access to RSUIContext instances.
 */
class RSC_EXPORT RSUIContextManager final {
public:

    /**
     * @brief Provides a singleton instance of the RSUIContextManager.
     *
     * @return A constant reference to the singleton instance of RSUIContextManager.
     */
    static const RSUIContextManager& Instance();

    /**
     * @brief Provides a mutable singleton instance of the RSUIContextManager.
     *
     * This function will be called when there are multiple instances of arkui.
     *
     * @return A reference to the singleton instance of RSUIContextManager.
     */
    static RSUIContextManager& MutableInstance();

    /**
     * @brief Retrieves the RSUIContext associated with the given token.
     * 
     * @param token The unique identifier for the RSUIContext.
     * @return A shared pointer to the RSUIContext if found; otherwise, nullptr.
     */
    const std::shared_ptr<RSUIContext> GetRSUIContext(uint64_t token) const;

    /**
     * @brief Creates a new RSUIContext.
     * 
     * @return A shared pointer to the newly created RSUIContext.
     */
    std::shared_ptr<RSUIContext> CreateRSUIContext();

    /**
     * @brief Destroys the RSUIContext associated with the given token.
     * 
     * @param token The unique identifier for the RSUIContext to be destroyed.
     */
    void DestroyContext(uint64_t token);

private:
    RSUIContextManager();
    ~RSUIContextManager() noexcept;
    RSUIContextManager(const RSUIContextManager&) = delete;
    RSUIContextManager(const RSUIContextManager&&) = delete;
    RSUIContextManager& operator=(const RSUIContextManager&) = delete;
    RSUIContextManager& operator=(const RSUIContextManager&&) = delete;

    /**
     * @brief Generates a unique token based on the provided thread ID (tid).
     * 
     * @param tid The thread ID for which the token is to be generated.
     * @return A 64-bit unsigned integer representing the generated token.
     */
    uint64_t GenerateToken(int32_t tid);

    /**
     * @brief Get a random UI task runner context.
     *
     * @return A shared pointer to an RSUIContext.
     */
    std::shared_ptr<RSUIContext> GetRandomUITaskRunnerCtx() const;

    mutable std::mutex mutex_;
    std::unordered_map<uint64_t, std::shared_ptr<RSUIContext>> rsUIContextMap_;
    uint32_t instanceIdCounter_ = 0;
    bool isMultiInstanceOpen_ = false;

    friend class RSUIDirector;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_MANAGER_H