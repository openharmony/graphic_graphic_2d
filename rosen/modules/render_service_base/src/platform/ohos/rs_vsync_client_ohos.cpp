/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "rs_vsync_client_ohos.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<RSVsyncClient> RSVsyncClient::Create()
{
    return std::make_unique<RSVsyncClientOhos>();
}

RSVsyncClientOhos::RSVsyncClientOhos()
    : runner_(AppExecFwk::EventRunner::Create(true)), handler_(std::make_shared<AppExecFwk::EventHandler>(runner_))
{
    if (runner_) {
        runner_->Run();
    }
}

void RSVsyncClientOhos::RequestNextVsync()
{
    if (!requestFlag_) {
        requestFlag_.store(true);
        handler_->PostTask([this]() {
            struct FrameCallback cb = {
                .timestamp_ = 0,
                .userdata_ = this,
                .callback_ = OnVsync,
            };
            VsyncError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
            if (ret != VSYNC_ERROR_OK) {
                ROSEN_LOGE("RSVsyncClientOhos::RequestNextVsync fail: %s", VsyncErrorStr(ret).c_str());
            }
        });
    }
}

void RSVsyncClientOhos::SetVsyncCallback(RSVsyncClient::VsyncCallback callback)
{
    vsyncCallback_ = callback;
}

void RSVsyncClientOhos::VsyncCallback(int64_t nanoTimestamp)
{
    requestFlag_.store(false);
    if (vsyncCallback_ != nullptr) {
        vsyncCallback_(nanoTimestamp);
    }
}

void RSVsyncClientOhos::OnVsync(int64_t nanoTimestamp, void* client)
{
    auto vsyncClient = static_cast<RSVsyncClientOhos*>(client);
    if (vsyncClient) {
        vsyncClient->VsyncCallback(nanoTimestamp);
    } else {
        ROSEN_LOGE("RSVsyncClientOhos::OnVsync vsyncClient is null");
    }
}
} // namespace Rosen
} // namespace OHOS
