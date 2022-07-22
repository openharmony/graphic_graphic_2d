/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H

#include "include/core/SkBitmap.h"

#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_handler.h"

#ifdef RS_ENABLE_EGLIMAGE
#include "include/gpu/GrContext.h"
#include "rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE


namespace OHOS {
namespace Rosen {
class RSTransactionData;

class RSBaseRenderUtil {
public:
    static void DropFrameProcess(RSSurfaceHandler& surfaceHandler);
    static bool ConsumeAndUpdateBuffer(RSSurfaceHandler& surfaceHandler);
    static bool ReleaseBuffer(RSSurfaceHandler& surfaceHandler);
    static bool ConvertBufferToBitmap(sptr<SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer, ColorGamut dstGamut,
        SkBitmap& bitmap);
#ifdef RS_ENABLE_EGLIMAGE
    static bool ConvertBufferToEglImage(sptr<SurfaceBuffer> buffer, std::shared_ptr<RSEglImageManager> eglImageManager,
        GrContext* grContext, sptr<SyncFence> acquireFence, sk_sp<SkImage>& image);
#endif

    static std::unique_ptr<RSTransactionData> ParseTransactionData(MessageParcel& parcel);

private:
    static bool IsBufferValid(const sptr<SurfaceBuffer>& buffer);
    static bool CreateYuvToRGBABitMap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        SkBitmap& bitmap);
    static bool CreateNewColorGamutBitmap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newGamutBuffer,
        SkBitmap& bitmap, ColorGamut srcGamut, ColorGamut dstGamut);
    static bool CreateBitmap(sptr<OHOS::SurfaceBuffer> buffer, SkBitmap& bitmap);
};
}
}
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H
