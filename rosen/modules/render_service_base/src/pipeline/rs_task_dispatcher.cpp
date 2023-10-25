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

#include "pipeline/rs_task_dispatcher.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSTaskDispatcher& RSTaskDispatcher::GetInstance()
{
    static RSTaskDispatcher instance;
    return instance;
}

void RSTaskDispatcher::RegisterTaskDispatchFunc(pid_t tid, const std::function<void(RSTask, bool)>& taskDispatchFunc)
{
    if (taskDispatchFunc) {
        taskDispatchFuncMap_.emplace(tid, taskDispatchFunc);
    }
}

void RSTaskDispatcher::PostTask(pid_t tid, const RSTask& task, bool isSyncTask)
{
    if (taskDispatchFuncMap_.count(tid)) {
        taskDispatchFuncMap_.at(tid)(task, isSyncTask);
    } else {
        if (task) {
            task();
        }
    }
}

bool RSTaskDispatcher::HasRegisteredTask(pid_t tid) const
{
    if (taskDispatchFuncMap_.count(tid)) {
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS
