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

#ifndef FRAMEWORKS_WM_INCLUDE_WINDOW_ATTRIBUTE_H
#define FRAMEWORKS_WM_INCLUDE_WINDOW_ATTRIBUTE_H

#include <functional>

#include <wm_common.h>

namespace OHOS {
class WindowAttribute {
public:
    void OnPositionChange(WindowPositionChangeFunc func);
    void OnSizeChange(WindowSizeChangeFunc func);
    void OnVisibilityChange(WindowVisibilityChangeFunc func);
    void OnTypeChange(WindowTypeChangeFunc func);

    int32_t         GetID() const;
    int32_t         GetX() const;
    int32_t         GetY() const;
    uint32_t        GetWidth() const;
    uint32_t        GetHeight() const;
    uint32_t        GetDestWidth() const;
    uint32_t        GetDestHeight() const;
    bool            GetVisibility() const;
    WindowType      GetType() const;

    // setter return true mean attr changed
    void SetID(int32_t id);
    bool SetXY(int32_t x, int32_t y);
    bool SetWidthHeight(uint32_t width, uint32_t height);
    bool SetDestWidthHeight(uint32_t width, uint32_t height);
    bool SetVisibility(bool visibility);
    bool SetType(WindowType type);

private:
    int32_t         winID = 0;
    int32_t         winX = 0;
    int32_t         winY = 0;
    uint32_t        winWidth = 0;
    uint32_t        winHeight = 0;
    uint32_t        winDestWidth = 0;
    uint32_t        winDestHeight = 0;
    bool            winVisibility = false;
    WindowType winType = static_cast<WindowType>(0);

    WindowPositionChangeFunc   positionChangeListener = nullptr;
    WindowSizeChangeFunc       sizeChangeListener = nullptr;
    WindowVisibilityChangeFunc visibilityChangeListener = nullptr;
    WindowTypeChangeFunc       typeChangeListener = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_ATTRIBUTE_H
