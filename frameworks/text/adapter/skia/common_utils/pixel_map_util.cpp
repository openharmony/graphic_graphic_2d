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

#include "pixel_map_util.h"

#include "utils/text_log.h"

namespace OHOS::Rosen::SPText {

std::shared_ptr<OHOS::Media::PixelMap> CreatePixelMap(
    const ImageOptions& options, const std::vector<skia::textlayout::PathInfo>& pathInfos)
{
    if (options.width < 0 || options.height < 0) {
        TEXT_LOGE("Invalid image options size invalid, %{public}d %{public}d", options.width, options.height);
        return nullptr;
    }
    
    // Create a PixelMap from the RSPath
    int32_t width = options.width;
    int32_t height = options.height;

    OHOS::Media::InitializationOptions opts;
    opts.pixelFormat = OHOS::Media::PixelFormat::RGBA_8888;
    opts.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    opts.size.width = width;
    opts.size.height = height;
    opts.editable = true;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = OHOS::Media::PixelMap::Create(opts);
    if (pixelMap == nullptr) {
        TEXT_LOGE("Failed to create PixelMap");
        return nullptr;
    }
    uint8_t* pixel = const_cast<uint8_t*>(pixelMap->GetPixels());
    if (pixel == nullptr) {
        TEXT_LOGE("Failed to get PixelMap pixels");
        return nullptr;
    }    

    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format{Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    bitmap.Build(width, height, format);
    bitmap.SetPixels(pixel);
    std::shared_ptr<Drawing::Canvas> bitmapCanvas = std::make_shared<Drawing::Canvas>();
    bitmapCanvas->Bind(bitmap);
    bitmapCanvas->Translate(options.offsetX, options.offsetY);

    float prefX = 0;
    float prefY = 0;
    for (size_t i = 0; i < pathInfos.size(); i++) {
        const auto& pathInfo = pathInfos[i];
        Drawing::Pen pen;
        pen.SetAntiAlias(true);
        pen.SetColor(pathInfo.textStyle.getColor());
        bitmapCanvas->AttachPen(pen);
        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        brush.SetColor(pathInfo.textStyle.getColor());
        bitmapCanvas->AttachBrush(brush);

        bitmapCanvas->Translate(pathInfo.point.GetX() - prefX, pathInfo.point.GetY() - prefY);
        bitmapCanvas->DrawPath(pathInfo.path);
        bitmapCanvas->Flush();
        prefX = pathInfo.point.GetX();
        prefY = pathInfo.point.GetY();
    }
    
    return std::move(pixelMap);
}
}