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

#include "feature/round_corner_display/rs_rcd_bitmap_utils.h"

#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace rs_rcd {
// RGBA_8888/BGRA_8888: 4 bytes per pixel, the alpha channel is the 4th byte (index 3).
constexpr int32_t bytesPerPixelRgba8888 = 4;
constexpr int32_t bytesPerPixelAlpha8 = 1;
// Alpha channel offset within a 8888 pixel; for both RGBA_8888 (R,G,B,A) and
// BGRA_8888 (B,G,R,A) the alpha is the 4th byte (index 3).
constexpr int32_t alphaChannelOffset = 3;

bool ExtractAlphaChannel(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap)
{
    RS_TRACE_NAME("rs_rcd::ExtractAlphaChannel");

    auto colorType = srcBitmap.GetColorType();
    int32_t width = srcBitmap.GetWidth();
    int32_t height = srcBitmap.GetHeight();
    const uint8_t *srcPixels = static_cast<const uint8_t *>(srcBitmap.GetPixels());
    int32_t srcRowBytes = srcBitmap.GetRowBytes();
    bool isInvalid = (srcBitmap.IsValid()) || (colorType != Drawing::ColorType::COLORTYPE_RGBA_8888 &&
        colorType != Drawing::ColorType::COLORTYPE_BGRA_8888) || (width <= 0 || height <= 0) ||
        (srcPixels == nullptr) || (srcRowBytes < width * bytesPerPixelRgba8888);
    if (isInvalid) {
        RS_LOGE("[%{public}s] srcBitmap is invalid \n", __func__);
        return false;
    }

    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bool succeeded = dstBitmap.Build(width, height, format);
    uint8_t *dstPixels = static_cast<uint8_t *>(dstBitmap.GetPixels());
    int32_t dstRowBytes = dstBitmap.GetRowBytes();
    succeeded &= ((dstPixels != nullptr) & (dstRowBytes >= (width * bytesPerPixelAlpha8)));
    if (!succeeded) {
        RS_LOGE("[%{public}s] Build Alpha8 bitmap failed \n", __func__);
        return false;
    }

    // Extract the alpha byte from each source pixel into the destination.
    // For both RGBA_8888 and BGRA_8888 layouts the alpha is the 4th byte (index 3),
    // so a single loop handles both formats.
    for (int32_t y = 0; y < height; y++) {
        const uint8_t *srcRow = srcPixels + y * srcRowBytes;
        uint8_t *dstRow = dstPixels + y * dstRowBytes;
        // Point to the first alpha byte in the source row; alpha is the 4th byte
        // (index 3) for both RGBA_8888 and BGRA_8888 layouts. Advance srcPtr by
        // bytesPerPixelRgba8888 and dstPtr by 1 on each iteration.
        const uint8_t *srcPtr = srcRow + alphaChannelOffset;
        uint8_t *dstPtr = dstRow;
        for (int32_t x = 0; x < width; x++) {
            // Extract alpha from the 8888 pixel via pointer increment.
            *dstPtr++ = *srcPtr;
            srcPtr += bytesPerPixelRgba8888;
        }
    }

    return true;
}

bool ConvertAlpha8ToRgba8888(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap)
{
    RS_TRACE_NAME("rs_rcd::ConvertAlpha8ToRgba8888");

    int32_t width = srcBitmap.GetWidth();
    int32_t height = srcBitmap.GetHeight();
    bool isInvalid = (srcBitmap.IsValid()) || (srcBitmap.GetColorType() != Drawing::ColorType::COLORTYPE_ALPHA_8) ||
        (width <= 0 || height <= 0);
    if (isInvalid) {
        RS_LOGE("RCD: srcBitmap is invalid");
        return false;
    }

    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bool succeeded = dstBitmap.Build(width, height, format) && (dstBitmap.GetPixels() != nullptr);
    if (!succeeded) {
        RS_LOGE("RCD: build rgba8888 bitmap failed when converting from alpha8");
        return false;
    }

    // Copy pixels from the Alpha8 source into the RGBA_8888 destination.
    // ReadPixels performs the color-type conversion: each source alpha byte is
    // written into the alpha channel of the corresponding RGBA_8888 pixel, with
    // the R/G/B channels filled accordingly (typically 0). The srcX/srcY offset
    // (0, 0) means copying from the top-left corner of the source bitmap.
    if (!srcBitmap.ReadPixels(dstBitmap.GetImageInfo(), dstBitmap.GetPixels(),
        static_cast<size_t>(dstBitmap.GetRowBytes()), 0, 0)) {
        RS_LOGE("RCD: read pixels from alpha8 bitmap failed");
        return false;
    }

    return true;
}
} // namespace rs_rcd
} // namespace Rosen
} // namespace OHOS
