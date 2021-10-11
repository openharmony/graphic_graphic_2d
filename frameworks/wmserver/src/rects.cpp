/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "rects.h"

#include <algorithm>

template<typename T>
T max(T a, T b)
{
    return (a > b) ? a : b;
}

template<typename T>
T min(T a, T b)
{
    return (a < b) ? a : b;
}

Rects::Rects(int32_t x, int32_t y, int32_t w, int32_t h)
{
    struct Rect rect = {x, y, w, h};
    rects.push_back(rect);
}

int32_t Rects::GetSize() const
{
    return rects.size();
}

int32_t Rects::GetX(int32_t index) const
{
    if (index >= 0 && index < rects.size()) {
        return rects[index].x;
    }
    return 0;
}

int32_t Rects::GetY(int32_t index) const
{
    if (index >= 0 && index < rects.size()) {
        return rects[index].y;
    }
    return 0;
}

int32_t Rects::GetW(int32_t index) const
{
    if (index >= 0 && index < rects.size()) {
        return rects[index].w;
    }
    return 0;
}

int32_t Rects::GetH(int32_t index) const
{
    if (index >= 0 && index < rects.size()) {
        return rects[index].h;
    }
    return 0;
}

Rects Rects::operator -(Rects &other)
{
    auto lrects = rects;
    RectsSubtrace(lrects, other.rects);

    decltype(lrects) totalRects;
    struct Rect totalRect = {};
    for (const auto &rect : lrects) {
        totalRect.x = min(totalRect.x, rect.x);
        totalRect.y = min(totalRect.y, rect.y);
        totalRect.w = max(totalRect.w, rect.x + rect.w);
        totalRect.h = max(totalRect.h, rect.y + rect.h);
    }
    totalRect.w -= totalRect.x;
    totalRect.h -= totalRect.y;
    totalRects.push_back(totalRect);
    RectsSubtrace(totalRects, lrects);

    Rects result;
    result.rects = lrects;
    if (totalRects.size() == 0) {
        totalRects.push_back(totalRect);
        result.rects = totalRects;
    }
    return result;
}

Rects &Rects::operator -=(Rects &other)
{
    *this = *this - other;
    return *this;
}

void Rects::Intersect(const struct Rect &a, const struct Rect &b, struct Rect &out)
{
    out.x = max(a.x, b.x);
    out.y = max(a.y, b.y);
    out.w = min(a.x + a.w, b.x + b.w) - out.x;
    out.h = min(a.y + a.h, b.y + b.h) - out.y;
}

void Rects::Subtrace(const struct Rect &rect, const struct Rect &inner, std::vector<struct Rect> &result)
{
    int32_t topDiff = inner.y - rect.y;
    if (topDiff > 0) {
        struct Rect r = {rect.x, rect.y, rect.w, topDiff};
        result.push_back(r);
    }

    int32_t bottomDiff = rect.y + rect.h - inner.y - inner.h;
    if (bottomDiff > 0) {
        struct Rect r = {rect.x, rect.y + rect.h - bottomDiff, rect.w, bottomDiff};
        result.push_back(r);
    }

    int32_t middleYDiff = inner.h;
    if (middleYDiff > 0) {
        int32_t leftDiff = inner.x - rect.x;
        if (leftDiff > 0) {
            struct Rect r = {rect.x, inner.y, leftDiff, middleYDiff};
            result.push_back(r);
        }

        int32_t rightDiff = rect.x + rect.w - inner.x - inner.w;
        if (rightDiff > 0) {
            struct Rect r = {inner.x + inner.w, inner.y, rightDiff, middleYDiff};
            result.push_back(r);
        }
    }
}

void Rects::RectsSubtrace(std::vector<struct Rect> &lrects, std::vector<struct Rect> rrects)
{
    while (true) {
        if (rrects.size() == 0) {
            break;
        }

        for (auto it = rrects.begin(); it != rrects.end(); it++) {
            struct Rect inter = {};
            auto findRect = [&it, &inter](const auto &item) {
                Rects::Intersect(*it, item, inter);
                return inter.w > 0 && inter.h > 0;
            };
            auto jt = std::find_if(lrects.begin(), lrects.end(), findRect);
            if (jt == lrects.end()) {
                rrects.erase(it);
                break;
            }

            // found, lrect -= inter, rrect -= inter
            struct Rect rrect = *it;
            rrects.erase(it);
            Subtrace(rrect, inter, rrects);

            struct Rect lrect = *jt;
            lrects.erase(jt);
            Subtrace(lrect, inter, lrects);
            break;
        }
    }
}
