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

#ifndef RS_CORE_FEATURE_HPAE_OFFLINE_THREAD_MANAGER_H
#define RS_CORE_FEATURE_HPAE_OFFLINE_THREAD_MANAGER_H
#include "event_handler.h"
#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSHpaeOfflineThreadManager final {
public:
    RSHpaeOfflineThreadManager() { CreateThread(); }
    ~RSHpaeOfflineThreadManager() { DestroyThread(); }
    bool PostTask(const std::function<void()>& task);

private:
    void CreateThread();
    void DestroyThread();
    
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
}
#endif // RS_CORE_FEATURE_HPAE_OFFLINE_THREAD_MANAGER_H