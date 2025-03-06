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

#include "rs_ui_context_manager.h"

#include <atomic>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

namespace {
static std::atomic_bool g_instanceValid = false;
}

RSUIContextManager::RSUIContextManager()
{
    g_instanceValid.store(true);
    isMultiInstanceOpen_ = RSSystemProperties::GetRSClientMultiInstanceEnabled();
    RS_LOGI("multi-instance, create RSUIContextManager, isMultiInstanceOpen_ %{public}d", isMultiInstanceOpen_);
}

RSUIContextManager::~RSUIContextManager() noexcept
{
    std::unique_lock<std::mutex> lock(mutex_);
    rsUIContextMap_.clear();
    g_instanceValid.store(false);
}

RSUIContextManager& RSUIContextManager::MutableInstance()
{
    static RSUIContextManager rsUIContextManager;
    return rsUIContextManager;
}

const RSUIContextManager& RSUIContextManager::Instance()
{
    return MutableInstance();
}

std::shared_ptr<RSUIContext> RSUIContextManager::CreateRSUIContext()
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    int32_t tid = gettid();
    uint64_t token = GenerateToken(tid);
    auto iter = rsUIContextMap_.find(token);
    if (iter != rsUIContextMap_.end()) {
        ROSEN_LOGW("RSUIContextManager::CreateRSUIContext: context token %{public}" PRIu64 " already exists", token);
        return iter->second;
    }
    auto newContext = std::shared_ptr<RSUIContext>(new RSUIContext(token));
    rsUIContextMap_[token] = newContext;
    return newContext;
}

const std::shared_ptr<RSUIContext> RSUIContextManager::GetRSUIContext(uint64_t token) const
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = rsUIContextMap_.find(token);
    if (iter == rsUIContextMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

void RSUIContextManager::DestroyContext(uint64_t token)
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load()) {
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = rsUIContextMap_.find(token);
    if (iter != rsUIContextMap_.end()) {
        rsUIContextMap_.erase(iter);
    } else {
        ROSEN_LOGW("RSUIContextManager::DestroyContext: context token %{public}" PRIu64 " does not exist", token);
    }
}

uint64_t RSUIContextManager::GenerateToken(int32_t tid)
{
    ++instanceIdCounter_;
    return (static_cast<uint64_t>(tid) << 32) | instanceIdCounter_; // 32 for 64-bit unsignd number shift
}

std::shared_ptr<RSUIContext> RSUIContextManager::GetRandomUITaskRunnerCtx() const
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load() || rsUIContextMap_.empty()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto& [_, ctx] : rsUIContextMap_) {
        if (ctx->HasTaskRunner()) {
            return ctx;
        }
    }
    return nullptr;
}

} // namespace Rosen
} // namespace OHOS