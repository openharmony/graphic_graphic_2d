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

#ifndef HPAE_BASE_RS_HPAE_PERF_THREAD_H
#define HPAE_BASE_RS_HPAE_PERF_THREAD_H

#include <future>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include "event_handler.h"
#include "refbase.h"

namespace OHOS::Rosen {

// reference: HgmTaskHandleThread
class RSHpaePerfThread {
public:
    static RSHpaePerfThread& Instance();
    const std::shared_ptr<AppExecFwk::EventRunner>& GetRunner() const { return runner_; }
    std::shared_ptr<AppExecFwk::EventHandler> CreateHandler();
    void PostTask(const std::function<void()>& task, int64_t delayTime = 0);
    void PostSyncTask(const std::function<void()>& task);

private:
    RSHpaePerfThread();
    ~RSHpaePerfThread() = default;
    RSHpaePerfThread(const RSHpaePerfThread&);
    RSHpaePerfThread(const RSHpaePerfThread&&);
    RSHpaePerfThread& operator=(const RSHpaePerfThread&);
    RSHpaePerfThread& operator=(const RSHpaePerfThread&&);

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> handler_ = nullptr;
    int32_t curThreadId_ = -1;
};

} // OHOS::Rosen

#endif // HPAE_BASE_RS_HPAE_PERF_THREAD_H