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

#ifndef INTERFACES_INNERKITS_WMCLIENT_WINDOW_H
#define INTERFACES_INNERKITS_WMCLIENT_WINDOW_H

#include <refbase.h>
#include <surface.h>
#include <promise.h>

#include "window_manager_input_type.h"
#include "window_manager_type.h"

namespace OHOS {
class Window : public RefBase {
public:
    virtual sptr<Surface> GetSurface() const = 0;
    virtual int32_t GetID() const = 0;

    virtual sptr<Promise<WMError>> Show() = 0;
    virtual sptr<Promise<WMError>> Hide() = 0;
    virtual sptr<Promise<WMError>> Move(int32_t x, int32_t y) = 0;
    virtual sptr<Promise<WMError>> SwitchTop() = 0;
    virtual sptr<Promise<WMError>> SetWindowType(WindowType type) = 0;
    virtual sptr<Promise<WMError>> SetWindowMode(WindowMode mode) = 0;
    virtual sptr<Promise<WMError>> Resize(uint32_t width, uint32_t height) = 0;
    virtual sptr<Promise<WMError>> ScaleTo(uint32_t width, uint32_t height) = 0;
    virtual WMError Rotate(WindowRotateType type) = 0;
    virtual WMError Destroy() = 0;

    // prop listener
    virtual void OnPositionChange(WindowPositionChangeFunc func) = 0;
    virtual void OnSizeChange(WindowSizeChangeFunc func) = 0;
    virtual void OnVisibilityChange(WindowVisibilityChangeFunc func) = 0;
    virtual void OnTypeChange(WindowTypeChangeFunc func) = 0;

    // listener
    virtual WMError OnTouch(OnTouchFunc cb) = 0;
    virtual WMError OnKey(OnKeyFunc cb) = 0;

    // pointer listener
    virtual WMError OnPointerEnter(PointerEnterFunc func) = 0;
    virtual WMError OnPointerLeave(PointerLeaveFunc func) = 0;
    virtual WMError OnPointerMotion(PointerMotionFunc func) = 0;
    virtual WMError OnPointerButton(PointerButtonFunc func) = 0;
    virtual WMError OnPointerFrame(PointerFrameFunc func) = 0;
    virtual WMError OnPointerAxis(PointerAxisFunc func) = 0;
    virtual WMError OnPointerAxisSource(PointerAxisSourceFunc func) = 0;
    virtual WMError OnPointerAxisStop(PointerAxisStopFunc func) = 0;
    virtual WMError OnPointerAxisDiscrete(PointerAxisDiscreteFunc func) = 0;

    // keyboard listener
    virtual WMError OnKeyboardKeymap(KeyboardKeymapFunc func) = 0;
    virtual WMError OnKeyboardEnter(KeyboardEnterFunc func) = 0;
    virtual WMError OnKeyboardLeave(KeyboardLeaveFunc func) = 0;
    virtual WMError OnKeyboardKey(KeyboardKeyFunc func) = 0;
    virtual WMError OnKeyboardModifiers(KeyboardModifiersFunc func) = 0;
    virtual WMError OnKeyboardRepeatInfo(KeyboardRepeatInfoFunc func) = 0;

    // touch listener
    virtual WMError OnTouchDown(TouchDownFunc func) = 0;
    virtual WMError OnTouchUp(TouchUpFunc func) = 0;
    virtual WMError OnTouchMotion(TouchMotionFunc func) = 0;
    virtual WMError OnTouchFrame(TouchFrameFunc func) = 0;
    virtual WMError OnTouchCancel(TouchCancelFunc func) = 0;
    virtual WMError OnTouchShape(TouchShapeFunc func) = 0;
    virtual WMError OnTouchOrientation(TouchOrientationFunc func) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_H
