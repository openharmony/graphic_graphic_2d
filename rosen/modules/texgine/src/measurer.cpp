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

#include "measurer.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
Measurer::Measurer(const std::vector<uint16_t> &text, const FontCollection &fontCollection)
    : text_(text), fontCollection_(fontCollection)
{
}

void Measurer::SetFontStyle(const FontStyles &style)
{
    style_ = style;
}

void Measurer::SetLocale(const std::string &locale)
{
    locale_ = locale;
}

void Measurer::SetRTL(bool rtl)
{
    rtl_ = rtl;
}

void Measurer::SetSize(double size)
{
    size_ = size;
}

void Measurer::SetRange(size_t start, size_t end)
{
    startIndex_ = start;
    endIndex_ = end;
}

void Measurer::SetFontFeatures(const FontFeatures &features)
{
    fontFeatures_ = &features;
}

void Measurer::SetSpacing(double letterSpacing, double wordSpacing)
{
    letterSpacing_ = letterSpacing;
    wordSpacing_ = wordSpacing;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
