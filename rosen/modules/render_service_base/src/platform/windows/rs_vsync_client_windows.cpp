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

#include "rs_vsync_client_windows.h"

#include <chrono>
#include <sys/time.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<RSVsyncClient> RSVsyncClient::Create()
{
    return std::make_unique<RSVsyncClientWindows>();
}

RSVsyncClientWindows::~RSVsyncClientWindows()
{
    running_ = false;
    if (vsyncThread_) {
        vsyncThread_->join();
    }
}

void RSVsyncClientWindows::RequestNextVsync()
{
    if (vsyncThread_ == nullptr) {
        running_ = true;
        auto func = std::bind(&RSVsyncClientWindows::VsyncThreadMain, this);
        vsyncThread_ = std::make_unique<std::thread>(func);
    }

    having_ = true;
}

void RSVsyncClientWindows::SetVsyncCallback(VsyncCallback callback)
{
    std::unique_lock lock(mutex_);
    vsyncCallback_ = callback;
}

void RSVsyncClientWindows::VsyncThreadMain()
{
    /* the coefficient of converting seconds to nanoseconds */
    constexpr int64_t SEC_TO_NANOSEC = 1000000000;
    while (running_) {
        /* The number of frames previewed is 30,33 =1000/30 */
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        int64_t now = ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec;
        if (having_.load()) {
            having_ = false;
            VsyncCallback vsyncCallbackTmp = nullptr;
            {
                std::unique_lock lock(mutex_);
                vsyncCallbackTmp = vsyncCallback_;
            }
            if (vsyncCallbackTmp) {
                vsyncCallbackTmp(now);
            }
        }
    }
}
} // namespace Rosen
} // namespace OHOS
