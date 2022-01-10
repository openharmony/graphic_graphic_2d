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

#ifndef ROSEN_RENDER_SERVICE_BASE_ISURFACE_CAPTURE_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_ISURFACE_CAPTURE_CALLBACK_PROXY_H

#ifdef ROSEN_OHOS
#include <iremote_proxy.h>

#include "ipc_callbacks/surface_capture_callback.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceCaptureCallbackProxy : public IRemoteProxy<RSISurfaceCaptureCallback> {
public:
    explicit RSSurfaceCaptureCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSSurfaceCaptureCallbackProxy() noexcept = default;

    void OnSurfaceCapture(NodeId id, Media::PixelMap* pixelmap) override;

private:
    static inline BrokerDelegator<RSSurfaceCaptureCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_ISURFACE_CAPTURE_CALLBACK_PROXY_H
