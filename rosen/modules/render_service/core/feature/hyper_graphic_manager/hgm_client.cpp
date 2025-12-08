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

#include "hgm_client.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "HgmClient"

namespace OHOS {
namespace Rosen {
std::shared_ptr<HgmClient> HgmClient::Create(const sptr<RSIRenderToServiceConnection>& renderToServiceConnection)
{
    return std::make_shared<HgmClient>(renderToServiceConnection);
}

HgmClient::HgmClient(const sptr<RSIRenderToServiceConnection>& renderToServiceConnection) :
    renderToServiceConnection_(renderToServiceConnection) {}

sptr<HgmServiceToProcessInfo> HgmClient::NotifyRenderServiceProcessHgmFrameRate(uint64_t timestamp, uint64_t vsyncId, sptr<HgmProcessToServiceInfo> info)
{
    if (renderToServiceConnection_ == nullptr) {
        RS_LOGE("%{public}s: renderToServiceConnection_ is nullptr", __func__);
        return nullptr;
    }
    return renderToServiceConnection_->NotifyRenderServiceProcessHgmFrameRate(timestamp, vsyncId, screenIds_, info);
}

void HgmClient::AddScreenId(ScreenId screenId)
{
    RS_LOGI("HgmClient::AddScreenId");
    screenIds_.insert(screenId);
}
} // namespace Rosen
} // namespace OHOS