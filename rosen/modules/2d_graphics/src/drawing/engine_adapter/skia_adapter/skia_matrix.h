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

#ifndef SKIA_MATRIX_H
#define SKIA_MATRIX_H

#include <memory>

#include "include/core/SkMatrix.h"

#include "impl_interface/matrix_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMatrix : public MatrixImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaMatrix();
    explicit SkiaMatrix(const Matrix& other);
    ~SkiaMatrix() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void Rotate(scalar degree, scalar px, scalar py) override;
    void Translate(scalar dx, scalar dy) override;
    void Scale(scalar sx, scalar sy, scalar px, scalar py) override;
    void SetScale(scalar sx, scalar sy) override;
    void SetScaleTranslate(scalar sx, scalar sy, scalar dx, scalar dy) override;
    void SetSkew(scalar kx, scalar ky) override;
    void SetSkew(scalar kx, scalar ky, scalar px, scalar py) override;
    const SkMatrix& ExportSkiaMatrix() const;

    void PreRotate(scalar degree) override;
    void PostRotate(scalar degree) override;
    void PostRotate(scalar degree, scalar px, scalar py) override;
    void PreTranslate(scalar dx, scalar dy) override;
    void PostTranslate(scalar dx, scalar dy) override;
    void PreScale(scalar sx, scalar sy) override;
    void PostScale(scalar sx, scalar sy) override;
    void PostScale(scalar sx, scalar sy, scalar px, scalar py) override;
    void PreSkew(scalar kx, scalar ky) override;
    void PostSkew(scalar kx, scalar ky) override;
    void PreSkew(scalar kx, scalar ky, scalar px, scalar py) override;
    void PostSkew(scalar kx, scalar ky, scalar px, scalar py) override;
    void PreConcat(const Matrix& other) override;
    void PreConcat(const Matrix44& other) override;
    void PostConcat(const Matrix& other) override;
    void PostConcat(const Matrix44& other) override;

    bool Invert(Matrix& inverse) const override;
    void Multiply(const Matrix& a, const Matrix& b) override;
    bool Equals(const Matrix& a, const Matrix& b) const override;
    void SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2) override;
    void MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, uint32_t count) const override;
    bool MapRect(Rect& dst, const Rect& src) const override;
    void Set(int index, scalar value) override;
    scalar Get(int index) const override;
    void GetAll(std::array<scalar, MatrixImpl::MATRIX_SIZE>& buffer) const override;
    void SetAll(std::array<scalar, MatrixImpl::MATRIX_SIZE>& buffer) override;
    bool IsIdentity() const override;

    void ImportMatrix(const SkMatrix& skMatrix);
    SkMatrix& ExportMatrix();

    void Clone(const Matrix& other) override;
    void PreRotate(scalar degree, scalar px, scalar py) override;
    void PreScale(scalar sx, scalar sy, scalar px, scalar py) override;
    void Reset() override;

    bool GetMinMaxScales(scalar scaleFactors[2]) override;
    bool HasPerspective() const override;

private:
    SkMatrix skMatrix_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
