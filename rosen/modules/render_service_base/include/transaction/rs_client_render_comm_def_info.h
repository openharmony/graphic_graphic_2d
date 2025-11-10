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

#ifndef RENDER_SERVICE_BASE_CLIENT_RENDER_COMM_DEF_INFO_H
#define RENDER_SERVICE_BASE_CLIENT_RENDER_COMM_DEF_INFO_H

#include "event_handler.h"
#include "common/rs_macros.h"
#include "platform/ohos/rs_irender_service.h"
#include "vsync_receiver.h"

namespace OHOS {
namespace Rosen {
class SurfaceBufferCallback {
public:
    SurfaceBufferCallback() = default;
    virtual ~SurfaceBufferCallback() noexcept = default;
    virtual void OnFinish(const FinishCallbackRet& ret) = 0;
    virtual void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) = 0;
};

class SurfaceCaptureCallback {
public:
    SurfaceCaptureCallback() {}
    virtual ~SurfaceCaptureCallback() {}
    virtual void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) = 0;
    virtual void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelmap,
        std::shared_ptr<Media::PixelMap> pixelmapHDR) = 0;
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CLIENT_RENDER_COMM_DEF_INFO_H