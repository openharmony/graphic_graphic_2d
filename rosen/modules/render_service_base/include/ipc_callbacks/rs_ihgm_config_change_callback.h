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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IHGM_CONFIG_CHANGE_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IHGM_CONFIG_CHANGE_CALLBACK_H

#include <iremote_broker.h>
#include "transaction/rs_hgm_config_data.h"

namespace OHOS {
namespace Rosen {
class RSIHgmConfigChangeCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.HgmConfigChangeCallback");
    RSIHgmConfigChangeCallback() = default;
    virtual ~RSIHgmConfigChangeCallback() noexcept = default;
    virtual void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) = 0;
    virtual void OnHgmRefreshRateModeChanged(int32_t refreshRateModeName) = 0;
    virtual void OnHgmRefreshRateUpdate(int32_t refreshRateUpdate) = 0;
};
}
}

#endif