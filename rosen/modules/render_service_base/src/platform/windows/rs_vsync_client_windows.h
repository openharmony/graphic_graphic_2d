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
#ifndef RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_WINDOWS_H
#define RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_WINDOWS_H

#include "platform/drawing/rs_vsync_client.h"

namespace OHOS {
namespace Rosen {
class RSVsyncClientWindows : public RSVsyncClient {
public:
    RSVsyncClientWindows() = default;
    ~RSVsyncClientWindows() override = default;

    void RequestNextVsync() override {}
    void SetVsyncCallback(VsyncCallback callback) override {}

private:
    static void OnVsync(int64_t nanoTimestamp, void* client) {}

    void VsyncCallback(int64_t nanoTimestamp) {}
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_WINDOWS_H