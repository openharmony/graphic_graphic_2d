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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_MANAGER_H

#include "rs_ui_context.h"

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSUIContextManager final {
public:
    static const RSUIContextManager& Instance();
    static RSUIContextManager& MutableInstance();

    const std::shared_ptr<RSUIContext> GetRSUIContext(uint64_t token) const;

    std::shared_ptr<RSUIContext> CreateRSUIContext();
    void DestroyContext(uint64_t token);

private:
    RSUIContextManager();
    ~RSUIContextManager() noexcept;
    RSUIContextManager(const RSUIContextManager&) = delete;
    RSUIContextManager(const RSUIContextManager&&) = delete;
    RSUIContextManager& operator=(const RSUIContextManager&) = delete;
    RSUIContextManager& operator=(const RSUIContextManager&&) = delete;

    uint64_t GenerateToken(int32_t tid);
    std::shared_ptr<RSUIContext> GetRandomUITaskRunnerCtx() const;

    mutable std::mutex mutex_;
    std::unordered_map<uint64_t, std::shared_ptr<RSUIContext>> rsUIContextMap_;
    uint32_t instanceIdCounter_ = 0;
    bool isMultiInstanceOpen_ = false;

    friend class RSUIDirector;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_MANAGER_H