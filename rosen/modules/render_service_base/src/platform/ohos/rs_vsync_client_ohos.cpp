/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <string>
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<RSVsyncClient> RSVsyncClient::Create()
{
    return std::make_unique<RSVsyncClientOhos>();
}

RSVsyncClientOhos::RSVsyncClientOhos()
    : runner_(AppExecFwk::EventRunner::Create(true)), handler_(std::make_shared<AppExecFwk::EventHandler>(runner_))
{
    static int sequence = 0;
    std::string name = "RSVsyncClientOhos";
    name += "_" + std::to_string(GetRealPid()) + "_" + std::to_string(sequence);
    auto rsClient = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    while (receiver_ == nullptr) {
        receiver_ = rsClient->CreateVSyncReceiver(name, handler_);
    }
    receiver_->Init();
    if (runner_) {
        runner_->Run();
    }
}

void RSVsyncClientOhos::RequestNextVsync()
{
    if (!requestFlag_.load()) {
        requestFlag_.store(true);
        handler_->PostTask([this]() {
            VSyncReceiver::FrameCallback fcb = {
                .userData_ = this,
                .callback_ = OnVsync,
            };
            receiver_->RequestNextVSync(fcb);
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
