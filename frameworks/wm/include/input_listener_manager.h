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

#ifndef FRAMEWORKS_WM_INCLUDE_INPUT_LISTENER_MANAGER_H
#define FRAMEWORKS_WM_INCLUDE_INPUT_LISTENER_MANAGER_H

#include <map>
#include <vector>

#include <refbase.h>
#include <wayland-client-protocol.h>
#include <window_manager_input_type.h>

#include "singleton_delegator.h"
#include "wayland_service.h"

namespace OHOS {
class InputListenerManager;
class InputListener : public RefBase {
    friend class InputListenerManager;

public:
    PointerEnterFunc pointerEnter = nullptr;
    PointerLeaveFunc pointerLeave = nullptr;
    PointerMotionFunc pointerMotion = nullptr;
    PointerButtonFunc pointerButton = nullptr;
    PointerFrameFunc pointerFrame = nullptr;
    PointerAxisFunc pointerAxis = nullptr;
    PointerAxisSourceFunc pointerAxisSource = nullptr;
    PointerAxisStopFunc pointerAxisStop = nullptr;
    PointerAxisDiscreteFunc pointerAxisDiscrete = nullptr;

    KeyboardKeymapFunc keyboardKeymap = nullptr;
    KeyboardEnterFunc keyboardEnter = nullptr;
    KeyboardLeaveFunc keyboardLeave = nullptr;
    KeyboardKeyFunc keyboardKey = nullptr;
    KeyboardModifiersFunc keyboardModifiers = nullptr;
    KeyboardRepeatInfoFunc keyboardRepeatInfo = nullptr;

    TouchDownFunc touchDown = nullptr;
    TouchUpFunc touchUp = nullptr;
    TouchMotionFunc touchMotion = nullptr;
    TouchFrameFunc touchFrame = nullptr;
    TouchCancelFunc touchCancel = nullptr;
    TouchShapeFunc touchShape = nullptr;
    TouchOrientationFunc touchOrientation = nullptr;

    void *GetWindow() const
    {
        return window;
    }
private:
    InputListener(void *w);
    ~InputListener() = default;

    void *window;
};

struct Seat {
        struct wl_seat *seat;
        struct wl_pointer *pointer;
        struct wl_keyboard *keyboard;
        struct wl_touch *touch;
};

using InputListeners = std::vector<sptr<InputListener>>;

class InputListenerManager : public RefBase {
public:
    static sptr<InputListenerManager> GetInstance();

    MOCKABLE void Init();
    MOCKABLE void Deinit();

    MOCKABLE sptr<InputListener> AddListener(void *window);
    MOCKABLE void RemoveListener(sptr<InputListener> &listener);

protected:
    InputListeners GetFocus();
    InputListeners GetInputCallback(void *window);
    void SetFocus(void *window);

    std::map<void *, InputListeners> inputListenersMap;
    void *focus = nullptr;

private:
    InputListenerManager() = default;
    MOCKABLE ~InputListenerManager() override;
    static inline sptr<InputListenerManager> instance = nullptr;
    static inline SingletonDelegator<InputListenerManager> delegator;

    static void OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver);
    static inline std::vector<Seat> seats;
    static uint32_t FindSeatNum(struct wl_seat *seat);

    static inline void SeatHandleCapabilities(void *, struct wl_seat *, uint32_t caps);
    static inline void RegisterPointerListener(uint32_t caps, struct wl_seat *seat);
    static inline void RegisterKeyboardListener(uint32_t caps, struct wl_seat *seat);
    static inline void RegisterTouchListener(uint32_t caps, struct wl_seat *seat);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_INPUT_LISTENER_MANAGER_H
