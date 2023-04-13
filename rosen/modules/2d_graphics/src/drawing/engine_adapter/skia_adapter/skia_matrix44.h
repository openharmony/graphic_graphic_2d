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

#include "include/core/SkMatrix44.h"
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
    void Multiply(const Matrix44& a, const Matrix44& b) override;
    void SetMatrix44(const std::array<scalar, MATRIX44_SIZE>& buffer) override;
    Matrix ConvertToMatrix() override;

    /*
     * @brief  Export Skia member variables for use by the adaptation layer.
     */
    const SkMatrix44& GetSkMatrix44() const;

private:
    SkMatrix44 skMatrix44_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
