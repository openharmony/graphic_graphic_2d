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

#ifndef ROSEN_MODULES_TEXGINE_SRC_MEASURER_H
#define ROSEN_MODULES_TEXGINE_SRC_MEASURER_H

#include <cstdint>
#include <vector>

#include "char_groups.h"
#include "font_collection.h"
#include "texgine/text_style.h"
#include "texgine/typography.h"
#include "word_breaker.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class Measurer {
public:
    static std::unique_ptr<Measurer> Create(const std::vector<uint16_t> &text, const FontCollection &fontCollection);

    Measurer(const std::vector<uint16_t> &text, const FontCollection &fontCollection);
    virtual ~Measurer() = default;
    void SetFontStyle(const FontStyles &style);
    void SetLocale(const std::string &locale);
    void SetRTL(bool rtl);
    void SetSize(double size);
    void SetRange(size_t start, size_t end);
    void SetFontFeatures(const FontFeatures &features);
    void SetSpacing(double letterSpacing, double wordSpacing);
    virtual const std::vector<Boundary> &GetWordBoundary() const = 0;
    virtual int Measure(CharGroups &cgs) = 0;

protected:
    const std::vector<uint16_t> &text_;
    const FontCollection &fontCollection_;
    FontStyles style_;
    std::string locale_ = "";
    bool rtl_ = false;
    double size_ = 16.0; // default text_style fontSize_
    size_t startIndex_ = 0;
    size_t endIndex_ = 0;
    const FontFeatures *fontFeatures_ = nullptr;
    double letterSpacing_ = 0.0;
    double wordSpacing_ = 0.0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_MEASURER_H
