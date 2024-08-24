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

#include "texgine/typography_types.h"

#include <memory>
#include <variant>

#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "text_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
TextAlign operator&(TextAlign a, TextAlign b)
{
    return static_cast<TextAlign>(static_cast<size_t>(a) & static_cast<size_t>(b));
}

TextAlign operator|(TextAlign a, TextAlign b)
{
    return static_cast<TextAlign>(static_cast<size_t>(a) | static_cast<size_t>(b));
}

TextAlign operator^(TextAlign a, TextAlign b)
{
    return static_cast<TextAlign>(static_cast<size_t>(a) ^ static_cast<size_t>(b));
}

TextAlign operator~(TextAlign a)
{
    return static_cast<TextAlign>(~static_cast<size_t>(a));
}

void operator&=(TextAlign &a, const TextAlign &b)
{
    a = static_cast<TextAlign>(static_cast<size_t>(a) & static_cast<size_t>(b));
}

void operator|=(TextAlign &a, const TextAlign &b)
{
    a = static_cast<TextAlign>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

void operator^=(TextAlign &a, const TextAlign &b)
{
    a = static_cast<TextAlign>(static_cast<size_t>(a) ^ static_cast<size_t>(b));
}

TextDecoration operator&(const TextDecoration &a, const TextDecoration &b)
{
    return static_cast<enum TextDecoration>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

TextDecoration operator|(const TextDecoration &a, const TextDecoration &b)
{
    return static_cast<enum TextDecoration>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

TextDecoration operator^(const TextDecoration &a, const TextDecoration &b)
{
    return static_cast<enum TextDecoration>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}

TextDecoration operator+(const TextDecoration &a, const TextDecoration &b)
{
    return static_cast<enum TextDecoration>(static_cast<int32_t>(a) + static_cast<int32_t>(b));
}

void operator&=(TextDecoration &a, const TextDecoration &b)
{
    a = static_cast<enum TextDecoration>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

void operator|=(TextDecoration &a, const TextDecoration &b)
{
    a = static_cast<enum TextDecoration>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

void operator^=(TextDecoration &a, const TextDecoration &b)
{
    a = static_cast<enum TextDecoration>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}

void operator+=(TextDecoration &a, const TextDecoration &b)
{
    a = static_cast<enum TextDecoration>(static_cast<int32_t>(a) + static_cast<int32_t>(b));
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
