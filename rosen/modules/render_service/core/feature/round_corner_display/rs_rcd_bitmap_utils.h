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
#include "feature/round_corner_display/rs_round_corner_config.h"

namespace OHOS {
namespace Rosen {

class RCDBitmapUtils {
public:
RCDBitmapUtils() = default;
virtual ~RCDBitmapUtils() = default;
// Extract the alpha channel from an RGBA_8888/BGRA_8888 bitmap into an Alpha8 bitmap.
// Used during resource loading to compress a decoded PNG (RGBA/BGRA) into a compact
// Alpha8 mask that matches the hardware layer mask format.
static bool ExtractAlphaChannel(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap);

// Convert an Alpha8 bitmap into an RGBA_8888 bitmap.
// Used during hardware resource preparation to expand the stored Alpha8 mask back to
// RGBA_8888, which is the format expected by the hardware composer buffer.
static bool ConvertAlpha8ToRgba8888(const Drawing::Bitmap& srcBitmap, Drawing::Bitmap& dstBitmap);

// load single image as Drawingimage
static bool LoadImg(const char* path, std::shared_ptr<Drawing::Image>& img);

static bool DecodeAlphaBitmap(std::shared_ptr<Drawing::Image> image, Drawing::Bitmap &bitmap);

static bool DecodeBitmap(std::shared_ptr<Drawing::Image> image, Drawing::Bitmap &bitmap);

static std::shared_ptr<Drawing::Bitmap> LoadBitmap(const char* path);

// Reuse the decoded image when the resource config is identical to avoid
// repeated LoadImg. If target is resource-equal to any candidate, reuse the
// candidate image; otherwise load the image from file.
static void LoadOrReuseImage(const rs_rcd::RoundCornerLayer& target,
    const std::initializer_list<std::pair<const rs_rcd::RoundCornerLayer&,
    std::shared_ptr<Drawing::Image>>>& candidates,
    std::shared_ptr<Drawing::Image>& outImage);

// Reuse the bitmap when the source image is shared to avoid repeated decoding.
// If target image is shared with any candidate, reuse the candidate bitmap;
// otherwise decode the bitmap from the image.
static void DecodeOrReuseBitmap(const std::shared_ptr<Drawing::Image>& targetImage,
    const std::initializer_list<std::pair<std::shared_ptr<Drawing::Image>,
    std::shared_ptr<Drawing::Bitmap>>>& candidates,
    std::shared_ptr<Drawing::Bitmap>& outBitmap);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_RCD_BITMAP_UTILS_H
