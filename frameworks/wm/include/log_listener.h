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

#ifndef FRAMEWORKS_WM_INCLUDE_LOG_LISTENER_H
#define FRAMEWORKS_WM_INCLUDE_LOG_LISTENER_H

#include "input_listener_manager.h"

#include "singleton_delegator.h"

namespace OHOS {
class LogListener : public RefBase {
public:
    static sptr<LogListener> GetInstance();

    MOCKABLE sptr<InputListener> AddListener(void *window);
    MOCKABLE void RemoveListener(sptr<InputListener> &listener);

private:
    LogListener() = default;
    MOCKABLE ~LogListener() = default;
    static inline sptr<LogListener> instance = nullptr;
    static inline SingletonDelegator<LogListener> delegator;

    void PointerHandleEnter(void *data, uint32_t serial, double x, double y);
    void PointerHandleLeave(void *data, uint32_t serial);
    void PointerHandleMotion(void *data, uint32_t time, double x, double y);
    void PointerHandleButton(void *data,
        uint32_t serial, uint32_t time, uint32_t button, PointerButtonState state);
    void PointerHandleFrame(void *data);
    void PointerHandleAxis(void *data, uint32_t time, PointerAxis axis, double value);
    void PointerHandleAxisSource(void *data, PointerAxisSource axisSource);
    void PointerHandleAxisStop(void *data, uint32_t time, PointerAxis axis);
    void PointerHandleAxisDiscrete(void *data, PointerAxis axis, int32_t discrete);
    void KeyboardHandleKeyMap(void *data, KeyboardKeymapFormat format, int32_t fd, uint32_t size);
    void KeyboardHandleEnter(void *data, uint32_t serial, const std::vector<uint32_t> &keys);
    void KeyboardHandleLeave(void *data, uint32_t serial);
    void KeyboardHandleKey(void *data,
        uint32_t serial, uint32_t time, uint32_t key, KeyboardKeyState state);
    void KeyboardHandleModifiers(void *data,
        uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group);
    void KeyboardHandleRepeatInfo(void *data, int32_t rate, int32_t delay);
    void TouchHandleDown(void *data, uint32_t serial, uint32_t time, int32_t id, double x, double y);
    void TouchHandleUp(void *data, uint32_t serial, uint32_t time, int32_t id);
    void TouchHandleMotion(void *data, uint32_t time, int32_t id, double x, double y);
    void TouchHandleFrame(void *data);
    void TouchHandleCancel(void *data);
    void TouchHandleShape(void *data, int32_t id, double major, double minor);
    void TouchHandleOrientation(void *data, int32_t id, double orientation);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_LOG_LISTENER_H
