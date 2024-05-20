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
#ifndef RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_OHOS_H
#define RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_OHOS_H

#include <atomic>
#include <functional>
#include <vector>

#include <event_handler.h>
#include "vsync_receiver.h"

#include "platform/drawing/rs_vsync_client.h"

namespace OHOS {
namespace Rosen {
class RSVsyncClientOhos : public RSVsyncClient {
public:
    explicit RSVsyncClientOhos();
    ~RSVsyncClientOhos() override = default;

    void RequestNextVsync() override;
    void SetVsyncCallback(VsyncCallback callback) override;

private:
    static void OnVsync(int64_t nanoTimestamp, void* client);

    void VsyncCallback(int64_t nanoTimestamp);

    std::atomic_bool requestFlag_ = false;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RSVsyncClient::VsyncCallback vsyncCallback_ = nullptr;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_OHOS_H