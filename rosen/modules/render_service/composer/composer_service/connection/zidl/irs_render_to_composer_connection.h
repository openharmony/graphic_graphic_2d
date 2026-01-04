/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_COMPOSER_SERVICE_CONNECTION_ZIDL_IRS_RENDER_TO_COMPOSER_CONNECTION_H
#define RENDER_SERVICE_COMPOSER_SERVICE_CONNECTION_ZIDL_IRS_RENDER_TO_COMPOSER_CONNECTION_H

#include <set>
#include "common/rs_macros.h"
#include "graphic_common.h"
#include <iremote_broker.h>
#include "irs_composer_to_render_connection.h"
#include "rs_layer_transaction_data.h"
#include <surface_buffer.h>
#include <sync_fence.h>

namespace OHOS::Rosen {
class RSB_EXPORT IRSRenderToComposerConnection : public IRemoteBroker {
public:
    virtual bool CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData) = 0;
    virtual void CleanLayerBufferBySurfaceId(uint64_t surfaceId) = 0;
    virtual void ClearFrameBuffers() = 0;
    virtual void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds) = 0;
    virtual void SetScreenBacklight(uint32_t level) = 0;
    virtual void SetComposerToRenderConnection(const sptr<IRSComposerToRenderConnection>& composerToRenderConn) = 0;
    virtual void PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IRSRenderToComposerConnection");

protected:
    enum {
        IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS,
        IRENDER_TO_COMPOSER_CONNECTION_CLEAN_LAYER_BUFFER_BY_SURFACE_ID,
        IRENDER_TO_COMPOSER_CONNECTION_CLEAR_FRAME_BUFFERS,
        IRENDER_TO_COMPOSER_CONNECTION_CLEAR_REDRAW_GPU_COMPOSITION_CACHE,
        IRENDER_TO_COMPOSER_CONNECTION_SET_BACKLIGHT_LEVEL,
        IRENDER_TO_COMPOSER_CONNECTION_SET_COMPOSER_TO_RENDER_CONNECTION,
        IRENDER_TO_COMPOSER_CONNECTION_PREALLOC_PROTECTED_FRAME_BUFFERS,
    };
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_SERVICE_CONNECTION_ZIDL_IRS_RENDER_TO_COMPOSER_CONNECTION_H