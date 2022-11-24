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

#include "cpudraw.h"

#include <scoped_bytrace.h>

bool CpudrawRect::Contain(int32_t x2, int32_t y2)
{
    return x <= x2 && x2 <= x + w && y <= y2 && y2 <= y + h;
}

Cpudraw::Cpudraw(uint32_t *vaddr, int32_t width, int32_t height)
    : addr(vaddr), width(width), height(height)
{
}

void Cpudraw::SetColor(const uint32_t &color)
{
    this->color = color;
}

void Cpudraw::SetBorder(const int32_t &border)
{
    this->border = border;
}

void Cpudraw::DrawBorder(const struct CpudrawRect &rect)
{
    DrawBorder(rect.x, rect.y, rect.w, rect.h);
}

void Cpudraw::DrawBorder(const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h)
{
    ScopedBytrace trace(__func__);
    DrawRect(x, y, border, h);
    DrawRect(x + w - border, y, border, h);
    DrawRect(x, y, w, border);
    DrawRect(x, y + h - border, w, border);
}

void Cpudraw::DrawRect(const struct CpudrawRect &rect)
{
    DrawRect(rect.x, rect.y, rect.w, rect.h);
}

void Cpudraw::DrawRect(const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h)
{
    ScopedBytrace trace(__func__);
    for (int32_t j = Max(y, 0); j < Min(y + h, height); j++) {
        for (int32_t i = Max(x, 0); i < Min(x + w, width); i++) {
            addr[j * width + i] = color;
        }
    }
}

int32_t Cpudraw::Min(const int32_t &a, const int32_t &b)
{
    return a < b ? a : b;
}

int32_t Cpudraw::Max(const int32_t &a, const int32_t &b)
{
    return a > b ? a : b;
}
