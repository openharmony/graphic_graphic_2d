/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "drawing_rect.h"

#include "utils/rect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Rect* CastToRect(OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<Rect*>(cRect);
}

OH_Drawing_Rect* OH_Drawing_RectCreate(float left, float top, float right, float bottom)
{
    return (OH_Drawing_Rect*)new Rect(left, top, right, bottom);
}

bool OH_Drawing_RectIntersect(OH_Drawing_Rect* cRect, const OH_Drawing_Rect* other)
{
    Rect* rect = CastToRect(cRect);
    const Rect* otherRect = reinterpret_cast<const Rect*>(other);
    if (rect == nullptr || otherRect == nullptr) {
        return false;
    }
    return rect->Intersect(*otherRect);
}

bool OH_Drawing_RectJoin(OH_Drawing_Rect* cRect, const OH_Drawing_Rect* other)
{
    Rect* rect = CastToRect(cRect);
    const Rect* otherRect = reinterpret_cast<const Rect*>(other);
    if (rect == nullptr || otherRect == nullptr) {
        return false;
    }
    return rect->Join(*otherRect);
}

void OH_Drawing_RectSetTop(OH_Drawing_Rect* cRect, float top)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return;
    }

    rect->SetTop(top);
}

void OH_Drawing_RectSetBottom(OH_Drawing_Rect* cRect, float bottom)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return;
    }

    rect->SetBottom(bottom);
}

void OH_Drawing_RectSetLeft(OH_Drawing_Rect* cRect, float left)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return;
    }

    rect->SetLeft(left);
}

void OH_Drawing_RectSetRight(OH_Drawing_Rect* cRect, float right)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return;
    }

    rect->SetRight(right);
}

float OH_Drawing_RectGetTop(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return 0.f;
    }

    return rect->GetTop();
}

float OH_Drawing_RectGetBottom(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return 0.f;
    }

    return rect->GetBottom();
}

float OH_Drawing_RectGetLeft(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return 0.f;
    }

    return rect->GetLeft();
}

float OH_Drawing_RectGetRight(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return 0.f;
    }

    return rect->GetRight();
}

float OH_Drawing_RectGetHeight(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return 0.f;
    }

    return rect->GetHeight();
}

float OH_Drawing_RectGetWidth(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return 0.f;
    }

    return rect->GetWidth();
}

void OH_Drawing_RectCopy(OH_Drawing_Rect* sRect, OH_Drawing_Rect* dRect)
{
    Rect* rectSrc = CastToRect(dRect);
    if (rectSrc == nullptr) {
        return;
    }

    Rect* rectDst = CastToRect(sRect);
    if (rectDst == nullptr) {
        return;
    }

    rectSrc->SetLeft(rectDst->GetLeft());
    rectSrc->SetTop(rectDst->GetTop());
    rectSrc->SetRight(rectDst->GetRight());
    rectSrc->SetBottom(rectDst->GetBottom());
}

void OH_Drawing_RectDestroy(OH_Drawing_Rect* cRect)
{
    delete CastToRect(cRect);
}
