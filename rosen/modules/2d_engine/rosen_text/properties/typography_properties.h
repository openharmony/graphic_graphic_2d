/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_TYPOGRAPHY_PROPERTIES_H
#define ROSEN_TEXT_TYPOGRAPHY_PROPERTIES_H

#include "rosen_text/properties/typography_style.h"
#include "utils/rect.h"
namespace rosen {
class TypographyProperties {
public:
    enum class RectWidthStyle {
        TIGHT,
        MAX,
    };

    enum class Affinity {
        UPSTREAM,
        DOWNSTREAM,
    };

    enum class RectHeightStyle {
        TIGHT,
        MAX,
        INCLUDELINESPACEMIDDLE,
        INCLUDELINESPACETOP,
        INCLUDELINESPACEBOTTOM,
        STRUCT,
    };

    struct TextBox {
        OHOS::Rosen::Drawing::Rect rect_;
        TextDirection direction_;
        TextBox(OHOS::Rosen::Drawing::Rect rect, TextDirection direction) : rect_(rect), direction_(direction) {}
    };

    struct PositionAndAffinity {
        const size_t pos_;
        const Affinity affinity_;
        PositionAndAffinity(size_t pos, Affinity affinity) : pos_(pos), affinity_(affinity) {}
    };

    template<typename T>
    struct Range {
        T start_, end_;
        Range() : start_(), end_() {}
        Range(T a, T b) : start_(a), end_(b) {}
        bool operator == (const Range<T>& rhs) const
        {
            return start_ == rhs.start_ && end_ == rhs.end_;
        }

        T Width() const
        {
            return end_ - start_;
        }

        void Shift(T offset)
        {
            start_ += offset;
            end_ += offset;
        }
    };
};
} // namespace rosen
#endif // ROSEN_TEXT_TYPOGRAPHY_PROPERTIES_H
