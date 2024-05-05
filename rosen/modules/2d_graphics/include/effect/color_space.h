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

#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H

#include <string>

#include "include/core/SkColorSpace.h"

#include "drawing/engine_adapter/impl_interface/color_space_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class CMSTransferFuncType {
    SRGB,
    DOT2,
    LINEAR,
    REC2020,
};

enum class CMSMatrixType {
    SRGB,
    ADOBE_RGB,
    DCIP3,
    REC2020,
    XYZ,
};

// CMSMatrix3x3 is a 3x3 float type matrix.
constexpr static int MATRIX3_SIZE = 3;

struct CMSMatrix3x3 {
    float vals[MATRIX3_SIZE][MATRIX3_SIZE];
};

struct CMSTransferFunction {
    float g, a, b, c, d, e, f;
};

class DRAWING_API ColorSpace {
public:
    enum class ColorSpaceType {
        NO_TYPE,
        SRGB,
        SRGB_LINEAR,
        REF_IMAGE,
        RGB,
    };

    /**
     * @brief Create the sRGB color space.
     *
     * @return A shared ptr to ColorSpace
     */
    static std::shared_ptr<ColorSpace> CreateSRGB();

    /**
     * @brief Colorspace with the sRGB primaries, but a linear (1.0) gamma.
     * @return A shared ptr to ColorSpace
     */
    static std::shared_ptr<ColorSpace> CreateSRGBLinear();
    static std::shared_ptr<ColorSpace> CreateRefImage(const Image& image);
    /**
     * @brief Create a ColorSpace form a transfer function and a row-major 3x3 transformation to XYZ.
     * @param func    A transfer function type
     * @param matrix  A row-major 3x3 transformation type to XYZ
     * @return        A shared pointer to ColorSpace that its type is RGB.
     */
    static std::shared_ptr<ColorSpace> CreateRGB(const CMSTransferFuncType& func, const CMSMatrixType& matrix);
    static std::shared_ptr<ColorSpace> CreateCustomRGB(const CMSTransferFunction& func, const CMSMatrix3x3& matrix);
    /**
     * @brief Create a ColorSpace form a adaptro impl, only used by ImageInfo to ccreate from adaptor image info
     * @param impl    A adaptor impl of color space
     * @return        A shared pointer to ColorSpace that its type is RGB.
     */
    static std::shared_ptr<ColorSpace> CreateFromImpl(std::shared_ptr<ColorSpaceImpl> impl);

    ColorSpace() noexcept;
    virtual ~ColorSpace() = default;
    ColorSpaceType GetType() const;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    ColorSpace(std::shared_ptr<ColorSpaceImpl> impl) noexcept;
    ColorSpace(ColorSpaceType t) noexcept;
    ColorSpace(ColorSpaceType t, const Image& image) noexcept;
    ColorSpace(ColorSpaceType t, const CMSTransferFuncType& func, const CMSMatrixType& matrix) noexcept;
    ColorSpace(ColorSpaceType t, const CMSTransferFunction& func, const CMSMatrix3x3& matrix) noexcept;

    /**
     * @brief Caller use method toProfile of SkColorSpace, the parameter is type skcms_ICCProfile.
     * In order not to encapsulate this method. Drawing ColorSpace needs to be converted to SkColorSpace.
     * @return     A shared pointer to SkColorSpace.
     */
    sk_sp<SkColorSpace> GetSkColorSpace() const;
    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);

private:
    ColorSpaceType type_;
    std::shared_ptr<ColorSpaceImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
