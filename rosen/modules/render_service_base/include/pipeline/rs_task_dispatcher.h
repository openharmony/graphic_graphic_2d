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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_TASK_DISPATCHER_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_TASK_DISPATCHER_H

#include <functional>
#include <unordered_map>
#include <unistd.h>
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSTaskDispatcher final {
public:
using RSTask = std::function<void()>;

    static RSTaskDispatcher& GetInstance();
    void RegisterTaskDispatchFunc(pid_t tid, const std::function<void(RSTask, bool)>& taskDispatchFunc);
    void PostTask(pid_t tid, const RSTask& task, bool isSyncTask = false);
    bool HasRegisteredTask(pid_t tid) const;
private:
    RSTaskDispatcher() = default;
    ~RSTaskDispatcher() = default;
    RSTaskDispatcher(const RSTaskDispatcher&);
    RSTaskDispatcher(const RSTaskDispatcher&&);
    RSTaskDispatcher& operator=(const RSTaskDispatcher&);
    RSTaskDispatcher& operator=(const RSTaskDispatcher&&);
    std::unordered_map<pid_t, std::function<void(RSTask, bool)>> taskDispatchFuncMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_PIPELINE_RS_TASK_DISPATCHER_H
