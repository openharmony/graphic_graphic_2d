/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef SKIAPIXMAP_H
#define SKIAPIXMAP_H

#include "include/core/SkPixmap.h"

#include "impl_interface/pixmap_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API SkiaPixmap : public PixmapImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaPixmap();
    SkiaPixmap(const ImageInfo& imageInfo, const void* addr, size_t rowBytes);
    ~SkiaPixmap() override;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    const SkPixmap& ExportSkiaPixmap() const;
    std::shared_ptr<ColorSpace> GetColorSpace() override;
    ColorType GetColorType() override;
    AlphaType GetAlphaType() override;
    size_t GetRowBytes() override;
    const void* GetAddr() override;
    int32_t GetWidth() override;
    int32_t GetHeight() override;
private:
    SkPixmap skiaPixmap_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
