/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_HDR_RS_BASE_HDR_UTIL_H
#define RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_HDR_RS_BASE_HDR_UTIL_H

#include "engine/rs_base_render_util.h"
#include "rs_layer.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {
class RSBaseHdrUtil {
public:
    RSBaseHdrUtil() = default;
    ~RSBaseHdrUtil() = default;

    static HdrStatus CheckIsHdrSurfaceBuffer(const sptr<SurfaceBuffer> surfaceBuffer);
    static bool CheckIsHDRSelfProcessingBuffer(const sptr<SurfaceBuffer>& surfaceBuffer);
    static bool CheckIsSurfaceBufferWithMetadata(const sptr<SurfaceBuffer> surfaceBuffer);
    static bool CheckIsSurfaceBufferWithAiHdrMetadata(const sptr<SurfaceBuffer> surfaceBuffer);
    static void SetBufferHDRParam(BufferDrawParam& params, const RSLayerPtr& layer);
    static bool GetRGBA1010108Enabled();
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_HDR_RS_BASE_HDR_UTIL_H