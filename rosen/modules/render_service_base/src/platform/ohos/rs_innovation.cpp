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

#include "platform/common/rs_innovation.h"

#include <dlfcn.h>
#include <parameters.h>

namespace OHOS {
namespace Rosen {
void RSInnovation::OpenInnovationSo()
{
    innovationHandle = dlopen("libgraphic_innovation.z.so", RTLD_NOW);
    GetParallelCompositionFunc();
}

void RSInnovation::CloseInnovationSo()
{
    if (innovationHandle) {
        ResetParallelCompositionFunc();
        dlclose(innovationHandle);
    }
}

// parallel composition
bool RSInnovation::GetParallelCompositionEnabled(bool isUniRender)
{
    static bool parallelcompositionEnabled =
        std::atoi((system::GetParameter("persist.rosen.parallelcomposition.enabled", "0")).c_str()) != 0;
    if (isUniRender) {
        return parallelcompositionEnabled;
    }
    return _s_parallelCompositionLoaded && parallelcompositionEnabled;
}

void RSInnovation::GetParallelCompositionFunc()
{
    if (innovationHandle) {
        _s_createParallelSyncSignal = dlsym(innovationHandle, "CreateParallelSyncSignal");
        _s_signalCountDown = dlsym(innovationHandle, "SignalCountDown");
        _s_signalAwait = dlsym(innovationHandle, "SignalAwait");
        _s_assignTask = dlsym(innovationHandle, "AssignTask");
        _s_removeStoppedThreads = dlsym(innovationHandle, "RemoveStoppedThreads");
        _s_checkForSerialForced = dlsym(innovationHandle, "CheckForSerialForced");
        _s_parallelCompositionLoaded =
            (_s_createParallelSyncSignal != nullptr) &&
            (_s_signalCountDown != nullptr) &&
            (_s_signalAwait != nullptr) &&
            (_s_assignTask != nullptr) &&
            (_s_removeStoppedThreads != nullptr) &&
            (_s_checkForSerialForced != nullptr);
    }
}

void RSInnovation::ResetParallelCompositionFunc()
{
    if (_s_parallelCompositionLoaded) {
        _s_parallelCompositionLoaded = false;
        _s_createParallelSyncSignal = nullptr;
        _s_signalCountDown = nullptr;
        _s_signalAwait = nullptr;
        _s_assignTask = nullptr;
        _s_removeStoppedThreads = nullptr;
        _s_checkForSerialForced = nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS
