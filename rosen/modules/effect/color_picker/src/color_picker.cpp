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

#include "color_picker.h"
#include "hilog/log.h"
#include "effect_errors.h"
#include "effect_utils.h"
#include "color.h"
#include "pixel_map.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkRect.h"
#include "include/core/SkImageFilter.h"
#include "include/effects/SkImageFilters.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkFont.h"
#include "include/core/SkTypeface.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using OHOS::HiviewDFX::HiLog;

std::shared_ptr<ColorPicker> ColorPicker::CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
    uint32_t &errorCode)
{
    if (pixmap == nullptr) {
        HiLog::Info(LABEL, "[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    ColorPicker *colorPicker = new (std::nothrow) ColorPicker(pixmap);
    if (colorPicker == nullptr) {
        HiLog::Info(LABEL, "[ColorPicker]failed to create ColorPicker with pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    errorCode = SUCCESS;
    return std::shared_ptr<ColorPicker>(colorPicker);
}

std::shared_ptr<Media::PixelMap> ColorPicker::GetScaledPixelMap()
{
    // Create scaled pixelmap
    OHOS::Media::InitializationOptions options;
    options.alphaType = pixelmap_->GetAlphaType();
    options.pixelFormat = pixelmap_->GetPixelFormat();
    options.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    options.size.width = 1;
    options.size.height = 1;
    options.editable = true;
    std::unique_ptr<Media::PixelMap> newPixelMap = Media::PixelMap::Create(*pixelmap_.get(), options);
    return std::move(newPixelMap);
}

uint32_t ColorPicker::GetMainColor(ColorManager::Color &color)
{
    if (pixelmap_ == nullptr) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = GetScaledPixelMap();

    // get color
    uint32_t colorVal = 0;
    int x = 0;
    int y = 0;
    bool bSucc = pixelMap->GetARGB32Color(x, y, colorVal);
    HiLog::Info(LABEL, "[newpix].argb.ret=%{public}d, %{public}x", bSucc, colorVal);
    color = ColorManager::Color(colorVal);
    return SUCCESS;
}


ColorPicker::ColorPicker(std::shared_ptr<Media::PixelMap> pixmap)
{
    if (pixmap == nullptr) {
        return ;
    }
    pixelmap_ = pixmap;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
