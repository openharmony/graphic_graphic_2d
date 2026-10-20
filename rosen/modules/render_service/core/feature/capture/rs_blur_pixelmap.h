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

#ifndef RENDER_SERVICE_FEATURE_CAPTURE_RS_BLUR_PIXELMAP_H
#define RENDER_SERVICE_FEATURE_CAPTURE_RS_BLUR_PIXELMAP_H

#include <memory>
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
bool BlurPixelMapOnRenderThread(
    const std::shared_ptr<Media::PixelMap>& pixelMap, float blurRadius,
    std::shared_ptr<Media::PixelMap>& blurredPixelMap);
#endif

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_FEATURE_CAPTURE_RS_BLUR_PIXELMAP_H