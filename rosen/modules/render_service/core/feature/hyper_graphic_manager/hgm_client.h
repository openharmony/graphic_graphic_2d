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

#ifndef RENDER_PROCESS_HYPER_GRAPHIC_MANAGER_HGM_CLIENT_H
#define RENDER_PROCESS_HYPER_GRAPHIC_MANAGER_HGM_CLIENT_H

#include <vector>

#include "render_server/transaction/zidl/rs_irender_to_service_connection.h"

namespace OHOS {
namespace Rosen {
class HgmClient {
public:
    HgmClient(const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);
    ~HgmClient() = default;

    sptr<HgmServiceToProcessInfo> NotifyRpHgmFrameRate(uint64_t timestamp, uint64_t vsyncId,
        const sptr<HgmProcessToServiceInfo>& info);
    void AddScreenId(ScreenId screenId);
    void RemoveScreenId(ScreenId screenId);

private:
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_;
    std::unordered_set<ScreenId> screenIds_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_PROCESS_HYPER_GRAPHIC_MANAGER_HGM_CLIENT_H