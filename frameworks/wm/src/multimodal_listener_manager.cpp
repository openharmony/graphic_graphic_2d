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

#include "multimodal_listener_manager.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMMultimodalListener" };
constexpr int32_t SEND_OK = 0;
constexpr int32_t SEND_ERROR = -1;
}

MultimodalListener::MultimodalListener(void *w)
{
    window = w;
}

sptr<MultimodalListenerManager> MultimodalListenerManager::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new MultimodalListenerManager();
        }
    }
    return instance;
}

sptr<MultimodalListener> MultimodalListenerManager::AddListener(void *window)
{
    auto l = delegator.Dep<InputListenerManager>()->AddListener(window);
    sptr<MultimodalListener> ml = new MultimodalListener(window);
    ml->input = l;

    l->pointerMotion = std::bind(&MultimodalListenerManager::PointerHandleMotion, this, POINTER_ENTER_ARG);
    l->pointerButton = std::bind(&MultimodalListenerManager::PointerHandleButton, this, POINTER_BUTTON_ARG);
    l->pointerFrame = std::bind(&MultimodalListenerManager::PointerHandleFrame, this, POINTER_FRAME_ARG);
    l->pointerAxis = std::bind(&MultimodalListenerManager::PointerHandleAxis, this, POINTER_AXIS_ARG);
    l->keyboardKey = std::bind(&MultimodalListenerManager::KeyboardHandleKey, this, KEYBOARD_KEY_ARG);
    l->touchDown = std::bind(&MultimodalListenerManager::TouchHandleDown, this, TOUCH_DOWN_ARG);
    l->touchUp = std::bind(&MultimodalListenerManager::TouchHandleUp, this, TOUCH_UP_ARG);
    l->touchMotion = std::bind(&MultimodalListenerManager::TouchHandleMotion, this, TOUCH_MOTION_ARG);
    l->touchFrame = std::bind(&MultimodalListenerManager::TouchHandleFrame, this, TOUCH_FRAME_ARG);
    l->touchShape = std::bind(&MultimodalListenerManager::TouchHandleShape, this, TOUCH_SHAPE_ARG);
    l->touchOrientation = std::bind(
        &MultimodalListenerManager::TouchHandleOrientation, this, TOUCH_ORIENTATION_ARG);

    if (windowCallback.find(window) == windowCallback.end()) {
        windowCallback[window] = std::vector<sptr<MultimodalListener>>();
    }

    windowCallback[window].push_back(ml);
    return ml;
}

void MultimodalListenerManager::RemoveListener(sptr<MultimodalListener> &listener)
{
    if (listener == nullptr) {
        return;
    }
    auto &v = windowCallback[listener->GetWindow()];
    auto match = [&listener](const auto &l) {
        return listener == l;
    };
    auto it = std::find_if(v.begin(), v.end(), match);
    if (it != v.end()) {
        v.erase(it);
    }

    if (v.empty()) {
        windowCallback.erase(listener->GetWindow());
    }

    delegator.Dep<InputListenerManager>()->RemoveListener(listener->input);
}

void MultimodalListenerManager::PointerHandleMotion(void *data, uint32_t time, double x, double y)
{
    if (static_cast<uint64_t>(mouseProperty.actionButton) & MouseEvent::LEFT_BUTTON) {
        mouseProperty.action = MouseEvent::MOVE;
    } else {
        mouseProperty.action = MouseEvent::HOVER_MOVE;
    }

    MmiPoint point(x, y, 0);
    mouseProperty.mmiPoint = point;
    occurredTime = time;
    SendCallbackForMouse(data);
}

uint32_t MultimodalListenerManager::GetMouseButton(uint32_t button)
{
    constexpr uint32_t left = 0x110;
    constexpr uint32_t right = 0x111;
    constexpr uint32_t middle = 0x112;
    constexpr uint32_t forward = 0x115;
    constexpr uint32_t back = 0x116;
    switch (button) {
        case left:
            return MouseEvent::LEFT_BUTTON;
        case right:
            return MouseEvent::RIGHT_BUTTON;
        case middle:
            return MouseEvent::MIDDLE_BUTTON;
        case forward:
            return MouseEvent::FORWARD_BUTTON;
        case back:
            return MouseEvent::BACK_BUTTON;
        default:
            return MouseEvent::NONE_BUTTON;
    }
}

void MultimodalListenerManager::ProcessActionEvent(ActionEventInfo &actionEvent, TouchProperty &touchProperty)
{
    constexpr uint32_t actionTypeOne = 1;
    constexpr uint32_t actionTypeTwo = 2;
    constexpr uint32_t actionTypeThree = 3;
    constexpr uint32_t actionTypeFour = 4;
    constexpr uint32_t actionTypeFive = 5;

    if (actionEvent.isUp) {
        if (actionEventInfo.touchCount == 0) {
            touchProperty.action = actionTypeTwo;
        } else {
            touchProperty.action = actionTypeFive;
        }
        actionEvent.isUp = false;
    } else if (actionEvent.isDown) {
        if (actionEventInfo.touchCount == actionTypeOne) {
            touchProperty.action = actionTypeOne;
        } else {
            touchProperty.action = actionTypeFour;
        }
        actionEvent.isDown = false;
    } else if (actionEvent.isMotion) {
        if (actionEventInfo.touchCount > 0) {
            touchProperty.action = actionTypeThree;
        }
        actionEvent.isMotion = false;
    }
}

void MultimodalListenerManager::TouchEventEncap(
    ActionEventInfo &actionEvent, TouchEvent &touchEvent, int32_t size)
{
    MultimodalProperty multimodalProperty = {
        .highLevelEvent = 0,
        .uuid = "",
        .sourceType = 0,
        .occurredTime = 0,
        .deviceId = "",
        .inputDeviceId = 0,
        .isHighLevelEvent = false,
    };
    MmiPoint mp(0, 0, 0);
    ManipulationProperty manipulationProperty = {
        .startTime = 0,
        .operationState = 0,
        .pointerCount = 0,
        .pointerId = 0,
        .mp = MmiPoint(0, 0, 0),
        .touchArea = 0,
        .touchPressure = 0,
        .offsetX = 0,
        .offsetY = 0
    };
    TouchProperty touchProperty = {
        .action = 0,
        .index = 0,
        .forcePrecision = 0,
        .maxForce = 0,
        .tapCount = 0
    };
    WMLOGFD("touchCount = %{public}d", actionEventInfo.touchCount);

    ProcessActionEvent(actionEvent, touchProperty);
    if (actionEvent.touchEventInfos[0].isRefreshed) {
        mp.px_ = actionEvent.touchEventInfos[0].x;
        mp.py_ = actionEvent.touchEventInfos[0].y;

        manipulationProperty.startTime = actionEvent.touchEventInfos[0].startTime;
        manipulationProperty.mp = mp;
        manipulationProperty.touchPressure = actionEvent.touchEventInfos[0].touchPressure;
        actionEvent.touchEventInfos[0].isRefreshed = false;
    }

    touchEvent.Initialize(multimodalProperty, manipulationProperty, touchProperty);
}

void MultimodalListenerManager::PointerHandleButton(void *data, uint32_t serial,
    uint32_t time, uint32_t button, PointerButtonState state)
{
    constexpr uint32_t buttonSerialBase = 1;
    if (serial <= buttonSerial && serial > buttonSerialBase) {
        WMLOGFI("recv repeat button %{public}d %{public}d %{public}d %{public}d",
            serial, button, state, time);
        return;
    }

    uint32_t changeButton = GetMouseButton(button);
    uint32_t pressedButtons = mouseProperty.pressedButtons;
    if (state == POINTER_BUTTON_STATE_PRESSED) {
        pressedButtons |= changeButton;
    } else {
        pressedButtons ^= changeButton;
    }

    mouseProperty.pressedButtons = pressedButtons;
    mouseProperty.actionButton = changeButton;
    occurredTime = time;

    int32_t result = SendCallbackForMouse(data);
    if (result == SEND_OK) {
        buttonSerial = serial;
    }
}

void MultimodalListenerManager::PointerHandleFrame(void *data)
{
    WMLOGFI("%{public}d %{public}f %{public}f %{public}d %{public}d",
        mouseProperty.action,
        mouseProperty.mmiPoint.GetX(), mouseProperty.mmiPoint.GetY(),
        mouseProperty.actionButton, mouseProperty.pressedButtons);
    SendCallbackForMouse(data);
}

void MultimodalListenerManager::PointerHandleAxis(void *data, uint32_t time, PointerAxis axis, double value)
{
    mouseProperty.action = MouseEvent::SCROLL;
    mouseProperty.scrollingDelta = value;
    mouseProperty.scrollType = axis;
    occurredTime = time;
}

void MultimodalListenerManager::KeyboardHandleKey(void *data,
    uint32_t serial, uint32_t time, uint32_t key, KeyboardKeyState state)
{
    KeyEvent event;
    struct MultimodalProperty multiProperty = {
        .highLevelEvent = 0,
        .uuid = "",
        .sourceType = MultimodalEvent::KEYBOARD,
        .occurredTime = time,
        .deviceId = "",
        .inputDeviceId = 0,
        .isHighLevelEvent = false,
    };
    struct KeyProperty keyProperty = {
        .isPressed = (state == KEYBOARD_KEY_STATE_PRESSED),
        .keyCode = key,
        .keyDownDuration = 0,
    };

    static uint32_t keyDownTime = 0;
    if (state == KEYBOARD_KEY_STATE_PRESSED) {
        keyDownTime = time;
    } else {
        keyProperty.keyDownDuration = time - keyDownTime;
    }

    constexpr uint32_t linuxKeyBack = 158;
    if (key == linuxKeyBack) {
        keyProperty.keyCode = KeyEvent::CODE_BACK;
    }

    event.Initialize(multiProperty, keyProperty);
    const auto &mls = GetInputCallback(data);
    for (const auto &ml : mls) {
        if (ml->keyboardKeyCb) {
            ml->keyboardKeyCb(event);
        }
    }
}

void MultimodalListenerManager::TouchHandleDown(void *data,
    uint32_t serial, uint32_t time, int32_t id, double x, double y)
{
    if (id < MAX_TOUCH_NUM) {
        actionEventInfo.touchCount++;
        actionEventInfo.isDown = true;
        actionEventInfo.touchEventInfos[id].isRefreshed = true;
        actionEventInfo.touchEventInfos[id].serial = serial;
        actionEventInfo.touchEventInfos[id].startTime = time;
        actionEventInfo.touchEventInfos[id].currentTime = time;
        actionEventInfo.touchEventInfos[id].x = x;
        actionEventInfo.touchEventInfos[id].y = y;
    }

    if (id < MAX_TOUCH_NUM) {
        touchWindows[id] = data;
    }
}

void MultimodalListenerManager::TouchHandleUp(void *data, uint32_t serial, uint32_t time, int32_t id)
{
    if (id < MAX_TOUCH_NUM) {
        actionEventInfo.touchCount--;
        actionEventInfo.isUp = true;
        actionEventInfo.touchEventInfos[id].isRefreshed = true;
        actionEventInfo.touchEventInfos[id].serial = serial;
        actionEventInfo.touchEventInfos[id].currentTime = time;
    }
    void *window = nullptr;
    if (id < MAX_TOUCH_NUM) {
        window = touchWindows[id];
        touchWindows[id] = nullptr;
    }
    WMLOGFD("window: %{public}p", window);

    while (actionEventInfo.isUp || actionEventInfo.isDown || actionEventInfo.isMotion) {
        TouchEvent touchEvent;
        TouchEventEncap(actionEventInfo, touchEvent, MAX_TOUCH_NUM);

        const auto &mls = GetInputCallback(data);
        for (const auto &ml : mls) {
            if (ml->onTouchCb) {
                ml->onTouchCb(touchEvent);
            }
        }
    }
}

void MultimodalListenerManager::TouchHandleMotion(void *data, uint32_t time, int32_t id, double x, double y)
{
    if (id < MAX_TOUCH_NUM) {
        actionEventInfo.isMotion = true;
        actionEventInfo.touchEventInfos[id].isRefreshed = true;
        actionEventInfo.touchEventInfos[id].currentTime = time;
        actionEventInfo.touchEventInfos[id].x = x;
        actionEventInfo.touchEventInfos[id].y = y;
    }

    void *window = nullptr;
    if (id < MAX_TOUCH_NUM) {
        window = touchWindows[id];
    }
    WMLOGFD("window: %{public}p", window);
}

void MultimodalListenerManager::TouchHandleFrame(void *data)
{
    while (actionEventInfo.isUp || actionEventInfo.isDown || actionEventInfo.isMotion) {
        TouchEvent touchEvent;
        TouchEventEncap(actionEventInfo, touchEvent, MAX_TOUCH_NUM);

        const auto &mls = GetInputCallback(data);
        for (const auto &ml : mls) {
            if (ml->onTouchCb) {
                ml->onTouchCb(touchEvent);
            }
        }
    }
}

void MultimodalListenerManager::TouchHandleShape(void *data, int32_t id, double major, double minor)
{
    if (id < MAX_TOUCH_NUM) {
        actionEventInfo.touchEventInfos[id].touchPressure = (major + minor) / 2.0f;
    }

    void *window = nullptr;
    if (id < MAX_TOUCH_NUM) {
        window = touchWindows[id];
    }
    WMLOGFD("window: %{public}p", window);
}

void MultimodalListenerManager::TouchHandleOrientation(void *data, int32_t id, double orientation)
{
    void *window = nullptr;
    if (id < MAX_TOUCH_NUM) {
        window = touchWindows[id];
    }
    WMLOGFD("window: %{public}p", window);
}

int32_t MultimodalListenerManager::SendCallbackForMouse(void *data)
{
    WMLOGFI("%{public}d %{public}f %{public}f %{public}d %{public}x",
            mouseProperty.action,
            mouseProperty.mmiPoint.GetX(),
            mouseProperty.mmiPoint.GetY(),
            mouseProperty.actionButton,
            mouseProperty.pressedButtons);
    TouchEvent touchEvent;
    std::shared_ptr<MouseEvent> mouseEvent = std::make_shared<MouseEvent>();
    struct MultimodalProperty multimodal = {
        .highLevelEvent = 0,
        .uuid = "",
        .sourceType = MultimodalEvent::MOUSE,
        .occurredTime = occurredTime,
        .deviceId = "",
        .inputDeviceId = 0,
        .isHighLevelEvent = false,
    };
    struct ManipulationProperty manipulationProperty = {
        .startTime = occurredTime,
        .operationState = 0,
        .pointerCount = 0,
        .pointerId = 0,
        .mp = MmiPoint(),
        .touchArea = 0,
        .touchPressure = 0,
        .offsetX = 0,
        .offsetY = 0
    };
    struct TouchProperty touch = {
        .action = TouchEvent::OTHER,
        .index = 0,
        .forcePrecision = 0,
        .maxForce = 0,
        .tapCount = 0
    };
    mouseEvent->Initialize(multimodal, mouseProperty);
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    touchEvent.SetMultimodalEvent(mouseEvent);

    const auto &mls = GetInputCallback(data);
    for (const auto &ml : mls) {
        if (ml->onTouchCb) {
            ml->onTouchCb(touchEvent);
        }
    }
    return SEND_ERROR;
}

std::vector<sptr<MultimodalListener>> MultimodalListenerManager::GetInputCallback(void *window)
{
    static std::vector<sptr<MultimodalListener>> nullInputCallback;
    if (windowCallback.find(window) == windowCallback.end()) {
        return nullInputCallback;
    }
    return windowCallback[window];
}
} // namespace OHOS
