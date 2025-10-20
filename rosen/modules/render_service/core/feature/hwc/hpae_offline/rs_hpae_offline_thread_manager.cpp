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

#include "rs_hpae_offline_thread_manager.h"
#include "rs_hpae_offline_util.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"
#if defined(ROSEN_OHOS)
#include "ffrt_inner.h"

namespace {
ffrt::mutex g_ffrtThreadMutex;
constexpr uint32_t FFRT_QOS_LEVEL = 5;
}
#endif

namespace OHOS::Rosen {
bool RSHpaeOfflineThreadManager::PostTask(const std::function<void()>& task)
{
#if defined(ROSEN_OHOS)
    ffrt::submit_h([this, task]() {
        std::unique_lock<ffrt::mutex> lock(g_ffrtThreadMutex);
        task();
    }, {}, {}, ffrt::task_attr().qos(FFRT_QOS_LEVEL));
#endif
    return true;
}
}