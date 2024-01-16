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
    void ImportSkiaPixmap(const SkPixmap& skPixmap);
    const SkPixmap& ExportSkiaPixmap() const;
    std::shared_ptr<ColorSpace> GetColorSpace() const override;
    ColorType GetColorType() const override;
    AlphaType GetAlphaType() const override;
    ColorQuad GetColor(int x, int y) const override;
    size_t GetRowBytes() const override;
    const void* GetAddr() const override;
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    bool ScalePixels(const Pixmap& dst, const SamplingOptions& options) const override;
private:
    SkPixmap skiaPixmap_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
