/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_HETERO_HDR_UTIL_H
#define RS_HETERO_HDR_UTIL_H

#include <condition_variable>
#include <cstdint>
#include <dlfcn.h>
#include <set>
#include <unordered_map>
#include <vector>

#include <buffer_handle.h>

#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/drawing/rs_surface.h"

namespace OHOS {
namespace Rosen {
namespace RSHeteroHDRUtilConst {
constexpr std::uint32_t HDR_HETERO_NO = 0;
constexpr std::uint32_t HDR_HETERO = 1U << 0;
constexpr std::uint32_t HDR_HETERO_HDR = 1U << 1;
constexpr std::uint32_t HDR_HETERO_AIHDR = 1U << 2;
}

class RSHeteroHDRUtil {
public:
    static void GenDrawHDRBufferParams(const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable,
        MDCRectT& hpaeDstRect, bool isFixedDstBuffer, BufferDrawParam& drawableParams);

    static void CalculateDrawBufferRectMatrix(const RSSurfaceRenderParams* surfaceParams, MDCRectT& hpaeDstRect,
        bool isFixedDstBuffer, BufferDrawParam& param);

    static bool ValidateSurface(RSSurfaceRenderParams* surfaceParams);
private:
    RSHeteroHDRUtil() = default;
    ~RSHeteroHDRUtil() = default;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_HETERO_HDR_UTIL_H