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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_RS_COMPOSER_TO_RENDER_CONNECTION_PROXY_H
#define RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_RS_COMPOSER_TO_RENDER_CONNECTION_PROXY_H

#include <iremote_proxy.h>
#include <memory>
#include "irs_composer_to_render_connection.h"

namespace OHOS {
namespace Rosen {
class RSComposerToRenderConnectionProxy : public IRemoteProxy<IRSComposerToRenderConnection> {
public:
    explicit RSComposerToRenderConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSComposerToRenderConnectionProxy() noexcept = default;

    int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo& releaseLayerInfo) override;
    int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t>& lppNodeIds) override;
    void RegisterNotifyScreenNodeBufferReleasedCB(NotifyScreenNodeBufferReleasedCB callback) override {};
    void RegisterReleaseLayerBuffersCB(ReleaseLayerBuffersCB callback) override {};

private:
    static inline BrokerDelegator<RSComposerToRenderConnectionProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_RS_COMPOSER_TO_RENDER_CONNECTION_PROXY_H