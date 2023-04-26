/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef SKIAIMAGE_H
#define SKIAIMAGE_H

#include "include/core/SkBitmap.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPicture.h"
#ifdef ACE_ENABLE_GPU
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#endif
#include "skia_bitmap.h"
#include "skia_color_space.h"
#include "skia_matrix.h"
#include "skia_paint.h"
#include "skia_picture.h"

#include "impl_interface/image_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImage : public ImageImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaImage() noexcept;
    explicit SkiaImage(sk_sp<SkImage> skImg) noexcept;
    ~SkiaImage() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    void* BuildFromBitmap(const Bitmap& bitmap) override;
    void* BuildFromPicture(const Picture& picture, const SizeI& dimensions, const Matrix& matrix, const Brush& brush,
        BitDepth bitDepth, std::shared_ptr<ColorSpace> colorSpace) override;
#ifdef ACE_ENABLE_GPU
    bool BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap) override;
    bool BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
        CompressedType type) override;
#endif
    int GetWidth() const override;
    int GetHeight() const override;
    uint32_t GetUniqueID() const override;
    bool ReadPixels(Bitmap& bitmap, int x, int y) override;
    bool IsTextureBacked() const override;

    const sk_sp<SkImage> GetImage() const;

    /*
     * @brief  Update the member variable to skImage, adaptation layer calls.
     */
    void SetSkImage(const sk_sp<SkImage>& skImage);
#ifdef ACE_ENABLE_GPU
    /*
     * @brief  Export Skia member variables for use by the adaptation layer.
     */
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> GetGrContext() const;
#else
    sk_sp<GrContext> GetGrContext() const;
#endif
#endif

private:
#ifdef ACE_ENABLE_GPU
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
#endif
    sk_sp<SkImage> skiaImage_;
    SkiaPaint skiaPaint_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
