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

#ifndef IMAGEIMPL_H
#define IMAGEIMPL_H

#include "base_impl.h"

#include "draw/brush.h"
#include "effect/color_space.h"
#include "image/bitmap.h"
#include "image/picture.h"
#include "utils/matrix.h"
#include "utils/size.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
#ifdef ACE_ENABLE_GPU
class GPUContext;
class TextureInfo;
enum class TextureOrigin;
enum class CompressedType;
#endif
enum class BitDepth;
class ImageImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    ImageImpl() noexcept {}
    ~ImageImpl() override {}
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }
    virtual void* BuildFromBitmap(const Bitmap& bitmap) = 0;
    virtual void* BuildFromPicture(const Picture& picture, const SizeI& dimensions, const Matrix& matrix,
        const Brush& brush, BitDepth bitDepth, std::shared_ptr<ColorSpace> colorSpace) = 0;
#ifdef ACE_ENABLE_GPU
    virtual bool BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap) = 0;
    virtual bool BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
        CompressedType type) = 0;
    virtual bool BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace) = 0;
#endif
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual uint32_t GetUniqueID() const = 0;
    virtual bool ReadPixels(Bitmap& bitmap, int x, int y) = 0;
    virtual bool IsTextureBacked() const = 0;

    // using for recording, should to remove after using shared memory
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
