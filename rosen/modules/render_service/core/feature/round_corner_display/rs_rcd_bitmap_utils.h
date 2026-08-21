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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_RCD_BITMAP_UTILS_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_RCD_BITMAP_UTILS_H

#include "image/bitmap.h"

namespace OHOS {
namespace Rosen {
// Extract the alpha channel from an RGBA_8888/BGRA_8888 bitmap into an Alpha8 bitmap.
// Used during resource loading to compress a decoded PNG (RGBA/BGRA) into a compact
// Alpha8 mask that matches the hardware layer mask format.
bool ExtractAlphaChannel(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap);

// Convert an Alpha8 bitmap into an RGBA_8888 bitmap.
// Used during hardware resource preparation to expand the stored Alpha8 mask back to
// RGBA_8888, which is the format expected by the hardware composer buffer.
bool ConvertAlpha8ToRgba8888(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap);
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_RCD_BITMAP_UTILS_H
