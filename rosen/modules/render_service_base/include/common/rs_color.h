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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COLOR_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COLOR_H

#include <sys/types.h>
#include <stdint.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "surface_type.h"
#include "draw/color.h"

namespace OHOS {
namespace Rosen {
using scalar = Drawing::scalar;
namespace {
constexpr static uint8_t RGB_MAX_VALUE = 255;
constexpr static uint8_t COLOR_ARRAY_RED_INDEX = 0;
constexpr static uint8_t COLOR_ARRAY_GREEN_INDEX = 1;
constexpr static uint8_t COLOR_ARRAY_BLUE_INDEX = 2;
}

class RSB_EXPORT RSColor final {
public:
    RSColor() noexcept : alpha_(0), blue_(0), green_(0), red_(0) {}
    explicit RSColor(uint32_t rgba) noexcept;
    RSColor(int16_t red, int16_t green, int16_t blue) noexcept;
    RSColor(int16_t red, int16_t green, int16_t blue, int16_t alpha) noexcept;

    RSColor(const RSColor& rhs) noexcept
    {
        red_ = rhs.red_;
        green_ = rhs.green_;
        blue_ = rhs.blue_;
        alpha_ = rhs.alpha_;
        colorSpace_ = rhs.colorSpace_;
    }

    RSColor& operator=(const RSColor& rhs) noexcept
    {
        red_ = rhs.red_;
        green_ = rhs.green_;
        blue_ = rhs.blue_;
        alpha_ = rhs.alpha_;
        colorSpace_ = rhs.colorSpace_;
        return *this;
    }

    ~RSColor() = default;
    bool operator==(const RSColor& rhs) const;
    inline bool operator!=(const RSColor& rhs) const
    {
        return !operator==(rhs);
    }
    bool IsNearEqual(const RSColor& other, int16_t threshold = std::numeric_limits<int16_t>::epsilon()) const;
    RSColor operator+(const RSColor& rhs) const;
    RSColor operator-(const RSColor& rhs) const;
    RSColor operator*(float scale) const;
    RSColor operator/(float scale) const;
    RSColor& operator*=(float scale);
    uint32_t AsRgbaInt() const;
    static RSColor FromRgbaInt(uint32_t rgba);
    uint32_t AsArgbInt() const;
    static RSColor FromArgbInt(uint32_t rgba);
    uint32_t AsBgraInt() const;
    static RSColor FromBgraInt(uint32_t bgra);

    int16_t GetBlue() const;
    int16_t GetGreen() const;
    int16_t GetRed() const;
    int16_t GetAlpha() const;
    scalar GetRedF() const;
    scalar GetGreenF() const;
    scalar GetBlueF() const;
    scalar GetAlphaF() const;
    Drawing::Color4f GetColor4f() const;
    GraphicColorGamut GetColorSpace() const;
    void SetBlue(int16_t blue);
    void SetGreen(int16_t green);
    void SetRed(int16_t red);
    void SetAlpha(int16_t alpha);
    void SetColorSpace(const GraphicColorGamut colorSpace);
    void MultiplyAlpha(float alpha);

    void ConvertToP3ColorSpace();
    void ConvertToSRGBColorSpace();

    void Dump(std::string& out) const;

    static constexpr size_t GetBytesPerPixelInt()
    {
        return sizeof(int64_t);
    }

private:
    struct {
        int16_t alpha_ : 16;
        int16_t blue_ : 16;
        int16_t green_ : 16;
        int16_t red_ : 16;
    };
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
};
} // namespace Rosen
} // namespace OHOS

using Color = OHOS::Rosen::RSColor;
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COLOR_H
