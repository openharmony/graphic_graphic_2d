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

#include "window_attribute.h"

namespace OHOS {
void WindowAttribute::OnPositionChange(WindowPositionChangeFunc func)
{
    positionChangeListener = func;
}

void WindowAttribute::OnSizeChange(WindowSizeChangeFunc func)
{
    sizeChangeListener = func;
}

void WindowAttribute::OnVisibilityChange(WindowVisibilityChangeFunc func)
{
    visibilityChangeListener = func;
}

void WindowAttribute::OnTypeChange(WindowTypeChangeFunc func)
{
    typeChangeListener = func;
}

int32_t WindowAttribute::GetID() const
{
    return winID;
}

int32_t WindowAttribute::GetX() const
{
    return winX;
}

int32_t WindowAttribute::GetY() const
{
    return winY;
}

uint32_t WindowAttribute::GetWidth() const
{
    return winWidth;
}

uint32_t WindowAttribute::GetHeight() const
{
    return winHeight;
}

uint32_t WindowAttribute::GetDestWidth() const
{
    return winDestWidth;
}

uint32_t WindowAttribute::GetDestHeight() const
{
    return winDestHeight;
}

bool WindowAttribute::GetVisibility() const
{
    return winVisibility;
}

WindowType WindowAttribute::GetType() const
{
    return winType;
}

void WindowAttribute::SetID(int32_t id)
{
    winID = id;
}

bool WindowAttribute::SetXY(int32_t x, int32_t y)
{
    if (x != winX || y != winY) {
        winX = x;
        winY = y;
        if (positionChangeListener != nullptr) {
            positionChangeListener(x, y);
        }
        return true;
    }
    return false;
}

bool WindowAttribute::SetWidthHeight(uint32_t width, uint32_t height)
{
    if (width != winWidth || height != winHeight) {
        winWidth = width;
        winHeight = height;
        if (sizeChangeListener != nullptr) {
            sizeChangeListener(width, height);
        }
        return true;
    }
    return false;
}

bool WindowAttribute::SetDestWidthHeight(uint32_t width, uint32_t height)
{
    if (width != winDestWidth || height != winDestHeight) {
        winDestWidth = width;
        winDestHeight = height;
        return true;
    }
    return false;
}

bool WindowAttribute::SetVisibility(bool visibility)
{
    if (visibility != winVisibility) {
        winVisibility = visibility;
        if (visibilityChangeListener != nullptr) {
            visibilityChangeListener(visibility);
        }
        return true;
    }
    return false;
}

bool WindowAttribute::SetType(WindowType type)
{
    if (type != winType) {
        winType = type;
        if (typeChangeListener != nullptr) {
            typeChangeListener(type);
        }
        return true;
    }
    return false;
}
} // namespace OHOS
