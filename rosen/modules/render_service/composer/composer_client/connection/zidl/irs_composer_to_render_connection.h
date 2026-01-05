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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_IRS_COMPOSER_TO_RENDER_CONNECTION_H
#define RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_IRS_COMPOSER_TO_RENDER_CONNECTION_H

#include <set>
#include "common/rs_macros.h"
#include "graphic_common.h"
#include <iremote_broker.h>
#include "rs_layer_common_def.h"
#include "screen_manager/screen_types.h"
#include <surface_buffer.h>
#include <sync_fence.h>

namespace OHOS {
namespace Rosen {
struct ReleaseLayerBuffersInfo {
    uint64_t screenId = 0;
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>> timestampVec = {};
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseBufferFenceVec = {};
    int64_t lastSwapBufferTime = 0; /* 每帧回执时长 */
};
using NotifyScreenNodeBufferReleasedCB = std::function<void(ScreenId screenId)>;
using ReleaseLayerBuffersCB = std::function<void(ReleaseLayerBuffersInfo& releaseLayerInfo)>;

class IRSComposerToRenderConnection : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.ComposerToRenderConnection");

    IRSComposerToRenderConnection() = default;
    virtual ~IRSComposerToRenderConnection() noexcept = default;

    virtual int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo& releaseLayerInfo) = 0;
    virtual int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t>& lppNodeIds) = 0;
    virtual void RegisterNotifyScreenNodeBufferReleasedCB(NotifyScreenNodeBufferReleasedCB callback) = 0;
    virtual void RegisterReleaseLayerBuffersCB(ReleaseLayerBuffersCB callback) = 0;

protected:
    enum {
        ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS = 0,
        NOTIFY_LPP_LAYER_TO_RENDER = 1,
    };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_IRS_COMPOSER_TO_RENDER_CONNECTION_H