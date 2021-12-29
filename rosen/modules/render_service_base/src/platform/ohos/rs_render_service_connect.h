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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECT_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECT_H

#include <singleton.h>

#include "platform/ohos/rs_irender_service.h"

namespace OHOS {
namespace Rosen {

class RSRenderServiceConnect : public Singleton<RSRenderServiceConnect> {
public:
    static sptr<RSIRenderService> GetRenderService();

private:
    class RenderServiceDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        RenderServiceDeathRecipient() = default;
        ~RenderServiceDeathRecipient() final = default;

        DISALLOW_COPY_AND_MOVE(RenderServiceDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final override;
    };

    bool Connect();
    void ConnectDied();

    std::mutex renderConnectlock_;
    sptr<RSIRenderService> renderService_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;

    DECLARE_SINGLETON(RSRenderServiceConnect)
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECT_H
