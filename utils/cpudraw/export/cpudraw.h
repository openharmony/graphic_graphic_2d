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

#ifndef UTILS_INCLUDE_CPUDRAW_EXPORT_CPUDRAW_H
#define UTILS_INCLUDE_CPUDRAW_EXPORT_CPUDRAW_H

#include <cstdint>

struct CpudrawRect {
    double x;
    double y;
    double w;
    double h;

    bool Contain(int32_t x2, int32_t y2);
};

class Cpudraw {
public:
    Cpudraw(uint32_t *vaddr, int32_t width, int32_t height);

    void SetColor(const uint32_t &color);
    void SetBorder(const int32_t &border);

    void DrawBorder(const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h);
    void DrawBorder(const struct CpudrawRect &rect);

    void DrawRect(const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h);
    void DrawRect(const struct CpudrawRect &rect);

private:
    int32_t Min(const int32_t &a, const int32_t &b);
    int32_t Max(const int32_t &a, const int32_t &b);

    uint32_t *addr = nullptr;
    int32_t width = 0;
    int32_t height = 0;
    uint32_t color = 0xffffffff;
    int32_t border = 0;
};

#endif // UTILS_INCLUDE_CPUDRAW_EXPORT_CPUDRAW_H
