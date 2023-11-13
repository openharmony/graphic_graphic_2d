/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HGM_TASK_HANDLE_THREAD_H
#define HGM_TASK_HANDLE_THREAD_H

#include "event_handler.h"

namespace OHOS::Rosen {
class HgmTaskHandleThread {
public:
    static HgmTaskHandleThread& Instance();
    void PostTask(const std::function<void()>& task, int64_t delayTime = 0);

private:
    HgmTaskHandleThread();
    ~HgmTaskHandleThread() = default;
    HgmTaskHandleThread(const HgmTaskHandleThread&);
    HgmTaskHandleThread(const HgmTaskHandleThread&&);
    HgmTaskHandleThread& operator=(const HgmTaskHandleThread&);
    HgmTaskHandleThread& operator=(const HgmTaskHandleThread&&);

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
}
#endif // HGM_TASK_HANDLE_THREAD_H
