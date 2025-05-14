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

#include "drawing_canvas_utils.h"
#include "array_mgr.h"
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
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return rect->Intersect(*otherRect);
}

bool OH_Drawing_RectJoin(OH_Drawing_Rect* cRect, const OH_Drawing_Rect* other)
{
    Rect* rect = CastToRect(cRect);
    const Rect* otherRect = reinterpret_cast<const Rect*>(other);
    if (rect == nullptr || otherRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return rect->Join(*otherRect);
}

void OH_Drawing_RectSetTop(OH_Drawing_Rect* cRect, float top)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    rect->SetTop(top);
}

void OH_Drawing_RectSetBottom(OH_Drawing_Rect* cRect, float bottom)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    rect->SetBottom(bottom);
}

void OH_Drawing_RectSetLeft(OH_Drawing_Rect* cRect, float left)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    rect->SetLeft(left);
}

void OH_Drawing_RectSetRight(OH_Drawing_Rect* cRect, float right)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    rect->SetRight(right);
}

float OH_Drawing_RectGetTop(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0.f;
    }
    return rect->GetTop();
}

float OH_Drawing_RectGetBottom(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0.f;
    }
    return rect->GetBottom();
}

float OH_Drawing_RectGetLeft(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0.f;
    }
    return rect->GetLeft();
}

float OH_Drawing_RectGetRight(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0.f;
    }
    return rect->GetRight();
}

float OH_Drawing_RectGetHeight(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0.f;
    }
    return rect->GetHeight();
}

float OH_Drawing_RectGetWidth(OH_Drawing_Rect* cRect)
{
    Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0.f;
    }
    return rect->GetWidth();
}

void OH_Drawing_RectCopy(OH_Drawing_Rect* sRect, OH_Drawing_Rect* dRect)
{
    Rect* rectSrc = CastToRect(dRect);
    if (rectSrc == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    Rect* rectDst = CastToRect(sRect);
    if (rectDst == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    rectSrc->SetLeft(rectDst->GetLeft());
    rectSrc->SetTop(rectDst->GetTop());
    rectSrc->SetRight(rectDst->GetRight());
    rectSrc->SetBottom(rectDst->GetBottom());
}

void OH_Drawing_RectDestroy(OH_Drawing_Rect* cRect)
{
    if (!cRect) {
        return;
    }
    delete CastToRect(cRect);
}

OH_Drawing_Array* OH_Drawing_RectCreateArray(size_t size)
{
    if ((size == 0) || (size > (std::numeric_limits<uint16_t>::max() + 1))) {
        return nullptr;
    }
    ObjectArray *obj = new(std::nothrow) ObjectArray();
    if (obj == nullptr) {
        return nullptr;
    }
    obj->num = size;
    obj->addr = reinterpret_cast<OH_Drawing_Rect*>(new Rect[size]);
    if (obj->addr == nullptr) {
        return nullptr;
    }
    obj->type = ObjectType::DRAWING_RECT;
    return reinterpret_cast<OH_Drawing_Array*>(obj);
}

OH_Drawing_ErrorCode OH_Drawing_RectGetArraySize(OH_Drawing_Array* rectArray, size_t* pSize)
{
    if (rectArray == nullptr || pSize == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    ObjectArray *obj = reinterpret_cast<ObjectArray*>(rectArray);
    if (obj->type != ObjectType::DRAWING_RECT) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *pSize = obj->num;
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RectGetArrayElement(OH_Drawing_Array* rectArray, size_t index,
    OH_Drawing_Rect** rect)
{
    if (rectArray == nullptr || rect == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    ObjectArray *obj = reinterpret_cast<ObjectArray*>(rectArray);
    if (index >= obj->num || obj->type != ObjectType::DRAWING_RECT || obj->addr == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *rect = (OH_Drawing_Rect*)&(reinterpret_cast<Rect*>(obj->addr)[index]);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RectDestroyArray(OH_Drawing_Array* rectArray)
{
    if (rectArray == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    ObjectArray *obj = reinterpret_cast<ObjectArray*>(rectArray);
    if (obj->type != ObjectType::DRAWING_RECT || obj->addr == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    delete[] reinterpret_cast<Rect*>(obj->addr);
    delete obj;
    return OH_DRAWING_SUCCESS;
}
