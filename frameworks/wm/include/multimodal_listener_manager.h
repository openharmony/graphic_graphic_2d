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

#ifndef FRAMEWORKS_WM_INCLUDE_MULTIMODAL_LISTENER_MANAGER_H
#define FRAMEWORKS_WM_INCLUDE_MULTIMODAL_LISTENER_MANAGER_H

#include <map>
#include <vector>

#include <key_event.h>
#include <mouse_event.h>
#include <refbase.h>
#include <touch_event.h>

#include "input_listener_manager.h"
#include "singleton_delegator.h"

namespace OHOS {
class MultimodalListenerManager;
class MultimodalListener : public RefBase {
    friend class MultimodalListenerManager;

public:
    OnKeyFunc keyboardKeyCb = nullptr;
    OnTouchFunc onTouchCb = nullptr;

    void *GetWindow() const
    {
        return window;
    }

private:
    MultimodalListener(void *w);
    ~MultimodalListener() = default;

    void *window;
    sptr<InputListener> input;
};

struct TouchEventInfo {
    bool isRefreshed;
    bool isValid;
    int32_t serial;
    uint32_t startTime;
    uint32_t currentTime;
    int32_t x;
    int32_t y;
    float touchPressure;
};

struct ActionEventInfo {
    int32_t touchCount;
    bool isDown;
    bool isUp;
    bool isMotion;
    TouchEventInfo touchEventInfos[MAX_TOUCH_NUM];
};

class MultimodalListenerManager : public RefBase {
public:
    static sptr<MultimodalListenerManager> GetInstance();

    MOCKABLE sptr<MultimodalListener> AddListener(void *window);
    MOCKABLE void RemoveListener(sptr<MultimodalListener> &listener);

private:
    MultimodalListenerManager() = default;
    MOCKABLE ~MultimodalListenerManager() = default;
    static inline sptr<MultimodalListenerManager> instance = nullptr;
    static inline SingletonDelegator<MultimodalListenerManager> delegator;

    void PointerHandleMotion(void *data, uint32_t time, double x, double y);
    void PointerHandleButton(void *data,
        uint32_t serial, uint32_t time, uint32_t button, PointerButtonState state);
    void PointerHandleFrame(void *data);
    void PointerHandleAxis(void *data, uint32_t time, PointerAxis axis, double value);
    void KeyboardHandleKey(void *data,
        uint32_t serial, uint32_t time, uint32_t key, KeyboardKeyState state);
    void TouchHandleDown(void *data, uint32_t serial, uint32_t time, int32_t id, double x, double y);
    void TouchHandleUp(void *data, uint32_t serial, uint32_t time, int32_t id);
    void TouchHandleMotion(void *data, uint32_t time, int32_t id, double x, double y);
    void TouchHandleFrame(void *data);
    void TouchHandleShape(void *data, int32_t id, double major, double minor);
    void TouchHandleOrientation(void *data, int32_t id, double orientation);

    int32_t SendCallbackForMouse(void *data);
    uint32_t GetMouseButton(uint32_t button);
    void ProcessActionEvent(ActionEventInfo& actionEvent, TouchProperty& touchProperty);
    void TouchEventEncap(ActionEventInfo& actionEvent, TouchEvent& touchEvent, int32_t size);

    struct ActionEventInfo actionEventInfo = {};
    void *touchWindows[MAX_TOUCH_NUM] = {};
    struct MouseProperty mouseProperty;
    uint32_t occurredTime = 0;
    uint32_t buttonSerial = 0;

    std::vector<sptr<MultimodalListener>> GetInputCallback(void *window);
    std::map<void *, std::vector<sptr<MultimodalListener>>> windowCallback;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_MULTIMODAL_LISTENER_MANAGER_H
