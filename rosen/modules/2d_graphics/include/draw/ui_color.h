/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef UI_COLOR_H
#define UI_COLOR_H

#include <cstdint>
#include <stdint.h>
#include <string>

#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API UIColor {
public:
    UIColor() noexcept;
    UIColor(float red, float green, float blue, float alpha, float headroom = 1.0f) noexcept;
    UIColor(const UIColor& color) noexcept;

    ~UIColor() {}

    float GetRed() const;

    float GetGreen() const;

    float GetBlue() const;

    float GetAlpha() const;

    float GetHeadroom() const;

    void SetRed(float red);

    void SetGreen(float green);

    void SetBlue(float blue);

    void SetAlpha(float alpha);

    void SetHeadroom(float headroom);

    void SetRgb(float red, float green, float blue);

    void SetRgbF(float red, float green, float blue, float alpha = 1.0);

    bool operator==(const UIColor& that) const;
    bool operator!=(const UIColor& that) const;

    void Dump(std::string& out) const;

private:
    float red_;
    float green_;
    float blue_;
    float alpha_;
    float headroom_ = 1.0; // HDR Brightness Ratio.
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
