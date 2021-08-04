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

#include "log_listener.h"

#include <mutex>

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMLogListener" };
} // namespace

sptr<LogListener> LogListener::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new LogListener();
        }
    }
    return instance;
}

sptr<InputListener> LogListener::AddListener(void *window)
{
    auto l = delegator.Dep<InputListenerManager>()->AddListener(window);
    l->pointerEnter = std::bind(&LogListener::PointerHandleEnter, this, POINTER_ENTER_ARG);
    l->pointerLeave = std::bind(&LogListener::PointerHandleLeave, this, POINTER_LEAVE_ARG);
    l->pointerMotion = std::bind(&LogListener::PointerHandleMotion, this, POINTER_MOTION_ARG);
    l->pointerButton = std::bind(&LogListener::PointerHandleButton, this, POINTER_BUTTON_ARG);
    l->pointerFrame = std::bind(&LogListener::PointerHandleFrame, this, POINTER_FRAME_ARG);
    l->pointerAxis = std::bind(&LogListener::PointerHandleAxis, this, POINTER_AXIS_ARG);
    l->pointerAxisSource = std::bind(&LogListener::PointerHandleAxisSource, this, POINTER_AXIS_SOURCE_ARG);
    l->pointerAxisStop = std::bind(&LogListener::PointerHandleAxisStop, this, POINTER_AXIS_STOP_ARG);
    l->pointerAxisDiscrete = std::bind(&LogListener::PointerHandleAxisDiscrete, this, POINTER_AXIS_DISCRETE_ARG);
    l->keyboardKeymap = std::bind(&LogListener::KeyboardHandleKeyMap, this, KEYBOARD_KEYMAP_ARG);
    l->keyboardEnter = std::bind(&LogListener::KeyboardHandleEnter, this, KEYBOARD_ENTER_ARG);
    l->keyboardLeave = std::bind(&LogListener::KeyboardHandleLeave, this, KEYBOARD_LEAVE_ARG);
    l->keyboardKey = std::bind(&LogListener::KeyboardHandleKey, this, KEYBOARD_KEY_ARG);
    l->keyboardModifiers = std::bind(&LogListener::KeyboardHandleModifiers, this, KEYBOARD_MODIFIERS_ARG);
    l->keyboardRepeatInfo = std::bind(&LogListener::KeyboardHandleRepeatInfo, this, KEYBOARD_REPEAT_INFO_ARG);
    l->touchDown = std::bind(&LogListener::TouchHandleDown, this, TOUCH_DOWN_ARG);
    l->touchUp = std::bind(&LogListener::TouchHandleUp, this, TOUCH_UP_ARG);
    l->touchMotion = std::bind(&LogListener::TouchHandleMotion, this, TOUCH_MOTION_ARG);
    l->touchFrame = std::bind(&LogListener::TouchHandleFrame, this, TOUCH_FRAME_ARG);
    l->touchCancel = std::bind(&LogListener::TouchHandleCancel, this, TOUCH_CANCEL_ARG);
    l->touchShape = std::bind(&LogListener::TouchHandleShape, this, TOUCH_SHAPE_ARG);
    l->touchOrientation = std::bind(&LogListener::TouchHandleOrientation, this, TOUCH_ORIENTATION_ARG);
    return l;
}

void LogListener::RemoveListener(sptr<InputListener> &listener)
{
    delegator.Dep<InputListenerManager>()->RemoveListener(listener);
}

void LogListener::PointerHandleEnter(void *data, uint32_t serial, double x, double y)
{
    WMLOGFD("serial: %{public}u, (%{public}lf, %{public}lf)", serial, x, y);
}

void LogListener::PointerHandleLeave(void *data, uint32_t serial)
{
    WMLOGFD("serial: %{public}u", serial);
}

void LogListener::PointerHandleMotion(void *data, uint32_t time, double x, double y)
{
    WMLOGFD("time: %{public}u, (%{public}lf, %{public}lf)", time, x, y);
}

void LogListener::PointerHandleButton(void *data, uint32_t serial,
    uint32_t time, uint32_t button, PointerButtonState state)
{
    WMLOGFD("serial: %{public}u, button: %{public}u, state: %{public}u, time: %{public}d",
        serial, button, state, time);
}

void LogListener::PointerHandleFrame(void *data)
{
    WMLOGFD("");
}

void LogListener::PointerHandleAxis(void *data, uint32_t time, PointerAxis axis, double value)
{
    WMLOGFD("");
}

void LogListener::PointerHandleAxisSource(void *data, PointerAxisSource axisSource)
{
    WMLOGFD("axisSource: %{public}d", axisSource);
}

void LogListener::PointerHandleAxisStop(void *data, uint32_t time, PointerAxis axis)
{
    WMLOGFD("time: %{public}u, axis: %{public}d", time, axis);
}

void LogListener::PointerHandleAxisDiscrete(void *data, PointerAxis axis, int32_t discrete)
{
    WMLOGFD("axis: %{public}d, discrete: %{public}d", axis, discrete);
}

void LogListener::KeyboardHandleKeyMap(void *data,
    KeyboardKeymapFormat format, int32_t fd, uint32_t size)
{
    WMLOGFD("format: %{public}d, size: %{public}u", format, size);
}

void LogListener::KeyboardHandleEnter(void *data, uint32_t serial, const std::vector<uint32_t> &keys)
{
    WMLOGFD("serial: %{public}u", serial);
}

void LogListener::KeyboardHandleLeave(void *data, uint32_t serial)
{
    WMLOGFD("serial: %{public}u", serial);
}

void LogListener::KeyboardHandleKey(void *data,
    uint32_t serial, uint32_t time, uint32_t key, KeyboardKeyState state)
{
    WMLOGFD("serial: %{public}u, time: %{public}u, key: %{public}u, state: %{public}d",
        serial, time, key, state);
}

void LogListener::KeyboardHandleModifiers(void *data, uint32_t serial,
    uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
{
    WMLOGFD("serial: %{public}u, modsDepressed: %{public}u", serial, modsDepressed);
}

void LogListener::KeyboardHandleRepeatInfo(void *data, int32_t rate, int32_t delay)
{
    WMLOGFD("rate: %{public}d, delay: %{public}d", rate, delay);
}

void LogListener::TouchHandleDown(void *data,
    uint32_t serial, uint32_t time, int32_t id, double x, double y)
{
    WMLOGFD("serial: %{public}u, time: %{public}u, id: %{public}d, (%{public}lf, %{public}lf)",
        serial, time, id, x, y);
}

void LogListener::TouchHandleUp(void *data, uint32_t serial, uint32_t time, int32_t id)
{
    WMLOGFD("serial: %{public}u, time: %{public}u, id: %{public}d", serial, time, id);
}

void LogListener::TouchHandleMotion(void *data, uint32_t time, int32_t id, double x, double y)
{
    WMLOGFD("time: %{public}u, id: %{public}d, (%{public}lf, %{public}lf)", time, id, x, y);
}

void LogListener::TouchHandleFrame(void *data)
{
    WMLOGFD("");
}

void LogListener::TouchHandleCancel(void *data)
{
    WMLOGFD("");
}

void LogListener::TouchHandleShape(void *data, int32_t id, double major, double minor)
{
    WMLOGFD("id: %{public}d, major: %{public}lf, minor: %{public}lf", id, major, minor);
}

void LogListener::TouchHandleOrientation(void *data, int32_t id, double orientation)
{
    WMLOGFD("id: %{public}d, orientation: %{public}lf", id, orientation);
}
}
