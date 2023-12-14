/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_MATRIX44_H
#define SKIA_MATRIX44_H

#ifdef NEW_SKIA
#include "include/core/SkM44.h"
#else
#include "include/core/SkMatrix44.h"
#endif
#include "impl_interface/matrix44_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMatrix44 : public Matrix44Impl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaMatrix44();
    ~SkiaMatrix44() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void Translate(scalar dx, scalar dy, scalar dz) override;
    void Scale(scalar sx, scalar sy, scalar sz) override;
    void PreTranslate(scalar dx, scalar dy, scalar dz) override;
    void PostTranslate(scalar dx, scalar dy, scalar dz) override;
    void PreScale(scalar sx, scalar sy, scalar sz) override;
    void Multiply(const Matrix44& a, const Matrix44& b) override;
    void SetCol(int column, scalar x, scalar y, scalar z, scalar w) override;
    void SetMatrix44ColMajor(const std::array<scalar, MATRIX44_SIZE>& buffer) override;
    void SetMatrix44RowMajor(const std::array<scalar, MATRIX44_SIZE>& buffer) override;
    Matrix ConvertToMatrix() override;

    /*
     * @brief  Export Skia member variables for use by the adaptation layer.
     */
    const SkM44& GetSkMatrix44() const;

private:
    SkM44 skMatrix44_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
