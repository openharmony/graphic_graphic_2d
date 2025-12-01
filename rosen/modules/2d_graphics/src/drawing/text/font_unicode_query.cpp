/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "text/font_unicode_query.h"

#include <vector>

#include "font_harfbuzz.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
#ifdef USE_M133_SKIA
using HBFace = std::unique_ptr<hb_face_t, SkFunctionObject<hb_face_destroy>>;
using HBSet = std::unique_ptr<hb_set_t, SkFunctionObject<hb_set_destroy>>;
#else
template <typename T, typename P, P* p> using resource = std::unique_ptr<T, SkFunctionWrapper<P, p>>;
using HBFace = resource<hb_face_t, decltype(hb_face_destroy), hb_face_destroy>;
using HBSet = resource<hb_set_t, decltype(hb_set_destroy), hb_set_destroy>;
#endif

static constexpr uint32_t UNICODE_MAX_CODEPOINT = 0x10FFFF;
static constexpr uint32_t UNICODE_SURROGATE_HIGH_MIN = 0xD800;
static constexpr uint32_t UNICODE_SURROGATE_LOW_MAX = 0xDFFF;
static constexpr uint32_t UNICODE_NONCHARACTER_BLOCK_START = 0xFDD0;
static constexpr uint32_t UNICODE_NONCHARACTER_BLOCK_END = 0xFDEF;
static constexpr uint32_t UNICODE_PLANE_FINAL_MASK = 0xFFFF;
static constexpr uint32_t UNICODE_PLANE_FINAL_THRESHOLD = 0xFFFE;

bool IsValidUnicode(uint32_t codePoint)
{
    return codePoint == UNICODE_MAX_CODEPOINT ||
           (codePoint < UNICODE_MAX_CODEPOINT && // Whole unicode range
           // Surrogate Area linked no character
           !(codePoint >= UNICODE_SURROGATE_HIGH_MIN &&
             codePoint <= UNICODE_SURROGATE_LOW_MAX) &&
           !(codePoint >= UNICODE_NONCHARACTER_BLOCK_START &&
             codePoint <= UNICODE_NONCHARACTER_BLOCK_END) &&
           ((codePoint & UNICODE_PLANE_FINAL_MASK) <
            UNICODE_PLANE_FINAL_THRESHOLD)); // The last two codepoints in each plane is invalid
}

std::vector<uint32_t> FontUnicodeQuery::GenerateUnicodeItem(const std::shared_ptr<Typeface>& typeface)
{
    if (!typeface) {
        LOGE("Drawing_Text [GenerateUnicodeItem] typeface is nullptr!");
        return {};
    }
    HBFace hbFace = FontHarfbuzz::CreateHbFace(*typeface);
    if (!hbFace) {
        LOGE("Drawing_Text [GenerateUnicodeItem] hbFace is nullptr!");
        return {};
    }
    HBSet unicodeSetHb;
    unicodeSetHb.reset(hb_set_create());
    if (!unicodeSetHb) {
        LOGE("Drawing_Text [GenerateUnicodeItem] unicodeSetHb is nullptr!");
        return {};
    }
    hb_face_collect_unicodes(hbFace.get(), unicodeSetHb.get());
    uint32_t population = hb_set_get_population(unicodeSetHb.get());
    std::vector<uint32_t> unicodeVector;
    unicodeVector.reserve(population);
    hb_codepoint_t codePoint = HB_SET_VALUE_INVALID;
    while (hb_set_next(unicodeSetHb.get(), &codePoint)) {
        if (IsValidUnicode(codePoint)) {
            unicodeVector.push_back(codePoint);
        }
    }
    return unicodeVector;
}
} // Drawing
} // Rosen
} // OHOS