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

#ifndef RENDER_SERVICE_BASE_INCLUDE_TRANSACTION_RS_TRANSACTION_HANDLER_MANAGER_H
#define RENDER_SERVICE_BASE_INCLUDE_TRANSACTION_RS_TRANSACTION_HANDLER_MANAGER_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "transaction/rs_transaction_handler.h"

namespace OHOS {
namespace Rosen {
/**
 * @class RSTransactionHandlerManager
 *
 * @brief Manages the lifecycle and access to RSTransactionHandler instances.
 *        This manager allows RSRenderNode to access the correct RSTransactionHandler
 *        based on the uiContextToken, without directly depending on RSUIContext.
 */
class RSB_EXPORT RSTransactionHandlerManager final {
public:
    /**
     * @brief Provides a singleton instance of the RSTransactionHandlerManager.
     *
     * @return A reference to the singleton instance of RSTransactionHandlerManager.
     */
    static RSTransactionHandlerManager& Instance();

    /**
     * @brief Registers a RSTransactionHandler with the given token.
     *
     * @param token The unique identifier for the RSTransactionHandler.
     * @param handler The shared pointer to the RSTransactionHandler to register.
     */
    void Register(uint64_t token, std::shared_ptr<RSTransactionHandler> handler);

    /**
     * @brief Retrieves the RSTransactionHandler associated with the given token.
     *
     * @param token The unique identifier for the RSTransactionHandler.
     * @return A shared pointer to the RSTransactionHandler if found; otherwise, nullptr.
     */
    std::shared_ptr<RSTransactionHandler> Get(uint64_t token);

    /**
     * @brief Retrieves any available RSTransactionHandler as a fallback.
     *        Used when the specific token is not found.
     *
     * @return A shared pointer to any available RSTransactionHandler if any exists; otherwise, nullptr.
     */
    std::shared_ptr<RSTransactionHandler> GetAny();

    /**
     * @brief Unregisters the RSTransactionHandler associated with the given token.
     *
     * @param token The unique identifier for the RSTransactionHandler to unregister.
     */
    void Unregister(uint64_t token);

private:
    RSTransactionHandlerManager() = default;
    ~RSTransactionHandlerManager() = default;
    RSTransactionHandlerManager(const RSTransactionHandlerManager&) = delete;
    RSTransactionHandlerManager(RSTransactionHandlerManager&&) = delete;
    RSTransactionHandlerManager& operator=(const RSTransactionHandlerManager&) = delete;
    RSTransactionHandlerManager& operator=(RSTransactionHandlerManager&&) = delete;

    std::unordered_map<uint64_t, std::shared_ptr<RSTransactionHandler>> handlers_;
    std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_INCLUDE_TRANSACTION_RS_TRANSACTION_HANDLER_MANAGER_H
