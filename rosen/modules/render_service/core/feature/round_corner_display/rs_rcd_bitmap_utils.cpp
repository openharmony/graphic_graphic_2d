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

#include <cstdint>

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
// RGBA_8888/BGRA_8888: 4 bytes per pixel, the alpha channel is the 4th byte (index 3).
constexpr int32_t BYTES_PER_PIXEL_RGBA8888 = 4;
constexpr int32_t BYTES_PER_PIXEL_ALPHA8 = 1;
// Alpha channel offset within a 8888 pixel; for both RGBA_8888 (R,G,B,A) and
// BGRA_8888 (B,G,R,A) the alpha is the 4th byte (index 3).
constexpr int32_t ALPHA_CHANNEL_OFFSET = 3;
}

bool RCDBitmapUtils::ExtractAlphaChannel(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap)
{
    RS_OPTIONAL_TRACE_FMT("ExtractAlphaChannel");

    auto colorType = srcBitmap.GetColorType();
    int32_t width = srcBitmap.GetWidth();
    int32_t height = srcBitmap.GetHeight();
    const uint8_t* srcPixels = static_cast<const uint8_t*>(srcBitmap.GetPixels());
    int32_t srcRowBytes = srcBitmap.GetRowBytes();
    // Compute the minimum row bytes in 64-bit to avoid signed-integer overflow
    // when width * BYTES_PER_PIXEL_RGBA8888 exceeds INT32_MAX.
    int64_t minSrcRowBytes = static_cast<int64_t>(width) * BYTES_PER_PIXEL_RGBA8888;
    bool isInvalid = (srcBitmap.IsValid()) || (colorType != Drawing::ColorType::COLORTYPE_RGBA_8888 &&
        colorType != Drawing::ColorType::COLORTYPE_BGRA_8888) || (width <= 0 || height <= 0) ||
        (srcPixels == nullptr) || (minSrcRowBytes > INT32_MAX) || (srcRowBytes < static_cast<int32_t>(minSrcRowBytes));
    if (isInvalid) {
        RS_LOGE("[%{public}s] srcBitmap is invalid \n", __func__);
        return false;
    }

    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bool succeeded = dstBitmap.Build(width, height, format);
    uint8_t* dstPixels = static_cast<uint8_t*>(dstBitmap.GetPixels());
    int32_t dstRowBytes = dstBitmap.GetRowBytes();
    // Compute the minimum row bytes in 64-bit to avoid signed-integer overflow
    // when width * BYTES_PER_PIXEL_ALPHA8 exceeds INT32_MAX.
    int64_t minDstRowBytes = static_cast<int64_t>(width) * BYTES_PER_PIXEL_ALPHA8;
    succeeded = succeeded && (dstPixels != nullptr) && (minDstRowBytes <= INT32_MAX) &&
        (dstRowBytes >= static_cast<int32_t>(minDstRowBytes));
    if (!succeeded) {
        RS_LOGE("[%{public}s] Build Alpha8 bitmap failed \n", __func__);
        return false;
    }

    // Extract the alpha byte from each source pixel into the destination.
    // For both RGBA_8888 and BGRA_8888 layouts the alpha is the 4th byte (index 3),
    // so a single loop handles both formats.
    for (int32_t y = 0; y < height; y++) {
        const uint8_t* srcRow = srcPixels + y * srcRowBytes;
        uint8_t* dstRow = dstPixels + y * dstRowBytes;
        // Point to the first alpha byte in the source row; alpha is the 4th byte
        // (index 3) for both RGBA_8888 and BGRA_8888 layouts. Advance srcPtr by
        // BYTES_PER_PIXEL_RGBA8888 and dstPtr by 1 on each iteration.
        const uint8_t* srcPtr = srcRow + ALPHA_CHANNEL_OFFSET;
        uint8_t* dstPtr = dstRow;
        for (int32_t x = 0; x < width; x++) {
            // Extract alpha from the 8888 pixel via pointer increment.
            *dstPtr++ = *srcPtr;
            srcPtr += BYTES_PER_PIXEL_RGBA8888;
        }
    }

    return true;
}

bool RCDBitmapUtils::ConvertAlpha8ToRgba8888(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap)
{
    RS_OPTIONAL_TRACE_FMT("ConvertAlpha8ToRgba8888");

    int32_t width = srcBitmap.GetWidth();
    int32_t height = srcBitmap.GetHeight();
    bool isInvalid = (srcBitmap.IsValid()) || (srcBitmap.GetColorType() != Drawing::ColorType::COLORTYPE_ALPHA_8) ||
        (width <= 0 || height <= 0);
    if (isInvalid) {
        RS_LOGE("RCD: srcBitmap is invalid");
        return false;
    }

    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bool succeeded = dstBitmap.Build(width, height, format);
    auto* dstPixels = dstBitmap.GetPixels();
    succeeded = succeeded && (dstPixels != nullptr);
    if (!succeeded) {
        RS_LOGE("RCD: build rgba8888 bitmap failed when converting from alpha8");
        return false;
    }

    // Copy pixels from the Alpha8 source into the RGBA_8888 destination.
    // ReadPixels performs the color-type conversion: each source alpha byte is
    // written into the alpha channel of the corresponding RGBA_8888 pixel, with
    // the R/G/B channels filled accordingly (typically 0). The srcX/srcY offset
    // (0, 0) means copying from the top-left corner of the source bitmap.
    succeeded = srcBitmap.ReadPixels(dstBitmap.GetImageInfo(), dstPixels,
        static_cast<size_t>(dstBitmap.GetRowBytes()), 0, 0);
    if (!succeeded) {
        RS_LOGE("RCD: read pixels from alpha8 bitmap failed");
        return false;
    }

    return true;
}

bool RCDBitmapUtils::LoadImg(const char* path, std::shared_ptr<Drawing::Image>& img)
{
    if (path == nullptr) {
        RS_LOGE("[%{public}s] null path! \n", __func__);
        return false;
    }
    std::string filePath = std::string(rs_rcd::PATH_CONFIG_DIR) + "/" + path;
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Read Img(%{public}s) \n", __func__, filePath.c_str());
    std::shared_ptr<Drawing::Data> drData = Drawing::Data::MakeFromFileName(filePath.c_str());
    if (drData == nullptr) {
        RS_LOGE("[%{public}s] Open picture file failed! \n", __func__);
        return false;
    }
    img = std::make_shared<Drawing::Image>();
    if (!img->MakeFromEncoded(drData)) {
        img = nullptr;
        RS_LOGE("[%{public}s] Decode picture file failed! \n", __func__);
        return false;
    }
    return true;
}

bool RCDBitmapUtils::DecodeAlphaBitmap(std::shared_ptr<Drawing::Image> image, Drawing::Bitmap &bitmap)
{
    if (image == nullptr) {
        RS_LOGE("[%{public}s] No image found \n", __func__);
        return false;
    }
    Drawing::Bitmap srcBitmap;
    if (!image->AsLegacyBitmap(srcBitmap)) {
        RS_LOGE("[%{public}s] Create bitmap from drImage failed \n", __func__);
        return false;
    }
    // If the image color type is RGBA_8888 or BGRA_8888, extract its alpha channel
    // to an Alpha8 bitmap to reduce memory and match the hardware layer mask format.
    // If extraction fails, fall back to using the original bitmap directly, which is
    // still acceptable for downstream hardware resource processing.
    bool succeeded = ExtractAlphaChannel(srcBitmap, bitmap);
    if (!succeeded) {
        bitmap = srcBitmap;
    }
    return true;
}

bool RCDBitmapUtils::DecodeBitmap(std::shared_ptr<Drawing::Image> image, Drawing::Bitmap &bitmap)
{
    if (image == nullptr) {
        RS_LOGE("[%{public}s] No image found \n", __func__);
        return false;
    }
    if (!image->AsLegacyBitmap(bitmap)) {
        RS_LOGE("[%{public}s] Create bitmap from drImage failed \n", __func__);
        return false;
    }
    return true;
}

std::shared_ptr<Drawing::Bitmap> RCDBitmapUtils::LoadBitmap(const char* path)
{
    std::shared_ptr<Drawing::Image> imgTmp = nullptr;
    LoadImg(path, imgTmp);
    auto bitmapPtr = std::make_shared<Drawing::Bitmap>();
    if (!DecodeBitmap(imgTmp, *bitmapPtr)) {
        return nullptr;
    }
    return bitmapPtr;
}

void RCDBitmapUtils::LoadOrReuseImage(const rs_rcd::RoundCornerLayer& target,
    const std::initializer_list<std::pair<const rs_rcd::RoundCornerLayer&,
    std::shared_ptr<Drawing::Image>>>& candidates,
    std::shared_ptr<Drawing::Image>& outImage)
{
    for (const auto& candidate : candidates) {
        if (target.IsResourceEqual(candidate.first)) {
            outImage = candidate.second;
            return;
        }
    }
    LoadImg(target.fileName.c_str(), outImage);
}

void RCDBitmapUtils::DecodeOrReuseBitmap(const std::shared_ptr<Drawing::Image>& targetImage,
    const std::initializer_list<std::pair<std::shared_ptr<Drawing::Image>,
    std::shared_ptr<Drawing::Bitmap>>>& candidates,
    std::shared_ptr<Drawing::Bitmap>& outBitmap)
{
    for (const auto& candidate : candidates) {
        if (targetImage == candidate.first && candidate.second != nullptr) {
            outBitmap = candidate.second;
            return;
        }
    }
    outBitmap = std::make_shared<Drawing::Bitmap>();
    DecodeAlphaBitmap(targetImage, *outBitmap);
}
} // namespace Rosen
} // namespace OHOS
