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

#ifndef FRAMEWORKS_WMSERVER_SRC_RECTS_H
#define FRAMEWORKS_WMSERVER_SRC_RECTS_H

#include <cstdint>
#include <vector>

class Rects {
public:
    Rects() = default;
    Rects(int32_t x, int32_t y, int32_t w, int32_t h);
    virtual ~Rects() = default;

    int32_t GetSize() const;
    int32_t GetX(int32_t index) const;
    int32_t GetY(int32_t index) const;
    int32_t GetW(int32_t index) const;
    int32_t GetH(int32_t index) const;

    Rects operator -(Rects &other);
    Rects &operator -=(Rects &other);

private:
    struct Rect {
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;
    };
    static void Intersect(const struct Rect &a, const struct Rect &b, struct Rect &out);
    static void Subtrace(const struct Rect &rect, const struct Rect &inner, std::vector<struct Rect> &rects);
    static void RectsSubtrace(std::vector<struct Rect> &lrects, std::vector<struct Rect> rrects);

    std::vector<struct Rect> rects;
};

#endif // FRAMEWORKS_WMSERVER_SRC_RECTS_H
