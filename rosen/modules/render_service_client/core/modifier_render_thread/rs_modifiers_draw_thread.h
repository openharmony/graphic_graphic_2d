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

#ifndef RS_MODIFIERS_DRAW_THREAD_H
#define RS_MODIFIERS_DRAW_THREAD_H

#include <future>
#include <mutex>

#include "common/rs_common_def.h"
#include "event_handler.h"
#include "refbase.h"
#include "transaction/rs_transaction_data.h"

#ifdef ACCESSIBILITY_ENABLE
#include "accessibility_config.h"
#ifdef ENABLE_IPC_SECURITY
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"
#endif
#endif

namespace OHOS {
namespace Rosen {

namespace Detail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> scheduledTask(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(scheduledTask, scheduledTask->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};

#ifdef ACCESSIBILITY_ENABLE
class HighContrastObserver : public AccessibilityConfig::AccessibilityConfigObserver {
public:
    HighContrastObserver(bool& highContrast) : highContrast_(highContrast) {}

    void OnConfigChanged(const AccessibilityConfig::CONFIG_ID id, const AccessibilityConfig::ConfigValue& value)
    {
        // Non-system app, the first value is incorrect.
        if (!first_ || IsSystemApp()) {
            highContrast_ = value.highContrastText;
        }
        first_ = false;
    }

    bool IsSystemApp()
    {
        if (!isSystemApp_.has_value()) {
#ifdef ENABLE_IPC_SECURITY
            uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
            isSystemApp_ = Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(tokenId);
#else
            isSystemApp_ = false;
#endif
        }
        return isSystemApp_.value();
    }

private:
    bool highContrast_;
    bool first_ = true;
    std::optional<bool> isSystemApp_;
};
#endif
} // namespace Detail

class RSB_EXPORT RSModifiersDrawThread final {
public:
    static RSModifiersDrawThread& Instance();

    void SetCacheDir(const std::string& path);
#ifdef ACCESSIBILITY_ENABLE
    bool GetHighContrast();
#endif

    void PostTask(const std::function<void()>&& task);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([scheduledTask_(std::move(scheduledTask))]() { scheduledTask_->Run(); });
        return std::move(taskFuture);
    }

    static std::unique_ptr<RSTransactionData>& ConvertTransaction(std::unique_ptr<RSTransactionData>& transactionData);

private:
    RSModifiersDrawThread();
    ~RSModifiersDrawThread();

    RSModifiersDrawThread(const RSModifiersDrawThread&) = delete;
    RSModifiersDrawThread(const RSModifiersDrawThread&&) = delete;
    RSModifiersDrawThread& operator=(const RSModifiersDrawThread&) = delete;
    RSModifiersDrawThread& operator=(const RSModifiersDrawThread&&) = delete;

    static bool TargetCommad(
    Drawing::DrawCmdList::HybridRenderType hybridRenderType, uint16_t type, uint16_t subType, bool cmdListEmpty);

#ifdef ACCESSIBILITY_ENABLE
    void SubscribeHighContrastChange();
    void UnsubscribeHighContrastChange();
#endif

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::mutex mutex_;
    bool isStarted_ = false;

#ifdef ACCESSIBILITY_ENABLE
    bool highContrast_ = false;
    std::shared_ptr<Detail::HighContrastObserver> highContrastObserver_ = nullptr;
#endif

    void Start();
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_MODIFIERS_DRAW_THREAD_H