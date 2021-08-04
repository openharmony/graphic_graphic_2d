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

#include "input_listener_manager.h"

#include <mutex>

#include <wayland-client-protocol.h>

namespace OHOS {
namespace {
std::function<InputListeners()> g_getFocus = []() { return InputListeners(); };
std::function<InputListeners(void *)> g_getInputCallback = [](void *) { return InputListeners(); };
std::function<void(void *)> g_setFocus = [](void *) {};
}

InputListener::InputListener(void *w)
{
    window = w;
}

void InputListenerManager::Init()
{
    delegator.Dep<WaylandService>()->OnAppear(&InputListenerManager::OnAppear);
    auto instance = SingletonContainer::Get<InputListenerManager>();
    g_getFocus = std::bind(&InputListenerManager::GetFocus, instance);
    g_getInputCallback = std::bind(&InputListenerManager::GetInputCallback, instance, std::placeholders::_1);
    g_setFocus = std::bind(&InputListenerManager::SetFocus, instance, std::placeholders::_1);
}

void InputListenerManager::Deinit()
{
    if (seat != nullptr) {
        wl_seat_destroy(seat);
        seat = nullptr;
    }

    if (pointer != nullptr) {
        wl_pointer_destroy(pointer);
        pointer = nullptr;
    }

    if (keyboard != nullptr) {
        wl_keyboard_destroy(keyboard);
        keyboard = nullptr;
    }

    if (touch != nullptr) {
        wl_touch_destroy(touch);
        touch = nullptr;
    }

    g_getFocus = []() { return InputListeners(); };
    g_getInputCallback = [](void *) { return InputListeners(); };
    g_setFocus = [](void *) {};
}

InputListenerManager::~InputListenerManager()
{
    Deinit();
}

sptr<InputListenerManager> InputListenerManager::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new InputListenerManager();
        }
    }
    return instance;
}

sptr<InputListener> InputListenerManager::AddListener(void *window)
{
    sptr<InputListener> listener = new InputListener(window);
    if (listener == nullptr) {
        return nullptr;
    }
    if (inputListenersMap.find(window) == inputListenersMap.end()) {
        inputListenersMap[window] = std::vector<sptr<InputListener>>();
    }
    inputListenersMap[window].push_back(listener);
    return listener;
}

void InputListenerManager::RemoveListener(sptr<InputListener> &listener)
{
    if (listener == nullptr) {
        return;
    }
    auto& v = inputListenersMap[listener->GetWindow()];
    auto match = [&listener](const auto& l) {
        return listener == l;
    };
    auto it = std::find_if(v.begin(), v.end(), match);
    if (it != v.end()) {
        v.erase(it);
    }
    if (v.empty()) {
        inputListenersMap.erase(listener->GetWindow());
    }
}

InputListeners InputListenerManager::GetFocus()
{
    return GetInputCallback(focus);
}

void InputListenerManager::SetFocus(void *window)
{
    focus = window;
}

InputListeners InputListenerManager::GetInputCallback(void *window)
{
    static InputListeners nullInputCallback;
    if (inputListenersMap.find(window) == inputListenersMap.end()) {
        return nullInputCallback;
    }
    return inputListenersMap[window];
}

void InputListenerManager::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    if (iname == "wl_seat") {
        constexpr uint32_t wlSeatVersion = 1;
        seat = static_cast<struct wl_seat *>(get(&wl_seat_interface, wlSeatVersion));
        if (seat == nullptr) {
            return;
        }

        static struct wl_seat_listener listener = { SeatHandleCapabilities };
        wl_seat_add_listener(seat, &listener, nullptr);
    }
}

void InputListenerManager::SeatHandleCapabilities(void *, struct wl_seat *, uint32_t caps)
{
    InputListenerManager::RegisterPointerListener(caps);
    InputListenerManager::RegisterKeyboardListener(caps);
    InputListenerManager::RegisterTouchListener(caps);
}

namespace {
void OnPointerEnter(void *, struct wl_pointer *,
                    uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{
    double x = wl_fixed_to_double(sx);
    double y = wl_fixed_to_double(sy);

    auto window = static_cast<void *>(wl_surface_get_user_data(surface));
    g_setFocus(window);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerEnter) {
            listener->pointerEnter(window, serial, x, y);
        }
    }
}

void OnPointerLeave(void *, struct wl_pointer *,
                    uint32_t serial, struct wl_surface *surface)
{
    auto window = static_cast<void *>(wl_surface_get_user_data(surface));
    const auto& inputListeners = g_getInputCallback(window);
    for (const auto& listener : inputListeners) {
        if (listener->pointerLeave) {
            listener->pointerLeave(window, serial);
        }
    }
}

void OnPointerMotion(void *, struct wl_pointer *,
                     uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    double x = wl_fixed_to_double(sx);
    double y = wl_fixed_to_double(sy);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerMotion) {
            listener->pointerMotion(listener->GetWindow(), time, x, y);
        }
    }
}

void OnPointerButton(void *, struct wl_pointer *,
                     uint32_t serial, uint32_t time, uint32_t button, uint32_t s)
{
    auto state = static_cast<PointerButtonState>(s);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerButton) {
            listener->pointerButton(listener->GetWindow(), serial, time, button, state);
        }
    }
}

void OnPointerAxis(void *, struct wl_pointer *,
                   uint32_t time, uint32_t a, wl_fixed_t v)
{
    auto value = wl_fixed_to_double(v);
    auto axis = static_cast<PointerAxis>(a);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerAxis) {
            listener->pointerAxis(listener->GetWindow(), time, axis, value);
        }
    }
}

void OnPointerFrame(void *, struct wl_pointer *)
{
    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerFrame) {
            listener->pointerFrame(listener->GetWindow());
        }
    }
}

void OnPointerAxisSource(void *, struct wl_pointer *,
                         uint32_t as)
{
    auto axisSource = static_cast<PointerAxisSource>(as);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerAxisSource) {
            listener->pointerAxisSource(listener->GetWindow(), axisSource);
        }
    }
}

void OnPointerAxisStop(void *, struct wl_pointer *,
                       uint32_t time, uint32_t a)
{
    auto axis = static_cast<PointerAxis>(a);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerAxisStop) {
            listener->pointerAxisStop(listener->GetWindow(), time, axis);
        }
    }
}

void OnPointerAxisDiscrete(void *, struct wl_pointer *,
                           uint32_t a, int32_t discrete)
{
    auto axis = static_cast<PointerAxis>(a);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->pointerAxisDiscrete) {
            listener->pointerAxisDiscrete(listener->GetWindow(), axis, discrete);
        }
    }
}

void OnKeyboardKeymap(void *, struct wl_keyboard *,
                      uint32_t f, int32_t fd, uint32_t size)
{
    auto format = static_cast<KeyboardKeymapFormat>(f);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->keyboardKeymap) {
            listener->keyboardKeymap(listener->GetWindow(), format, fd, size);
        }
    }
}

void OnKeyboardEnter(void *, struct wl_keyboard *,
                     uint32_t serial, struct wl_surface *surface, struct wl_array *ks)
{
    std::vector<uint32_t> keys;
    auto k = static_cast<uint32_t *>(ks->data);
    while (reinterpret_cast<const char *>(k) < reinterpret_cast<const char *>(ks->data) + ks->size) {
        keys.push_back(*k++);
    }

    auto window = static_cast<void *>(wl_surface_get_user_data(surface));
    g_setFocus(window);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->keyboardEnter) {
            listener->keyboardEnter(listener->GetWindow(), serial, keys);
        }
    }
}

void OnKeyboardLeave(void *, struct wl_keyboard *,
                     uint32_t serial, struct wl_surface *surface)
{
    auto window = static_cast<void *>(wl_surface_get_user_data(surface));
    const auto& inputListeners = g_getInputCallback(window);
    for (const auto& listener : inputListeners) {
        if (listener->keyboardLeave) {
            listener->keyboardLeave(window, serial);
        }
    }
}

void OnKeyboardKey(void *, struct wl_keyboard *,
                   uint32_t serial, uint32_t time, uint32_t key, uint32_t s)
{
    auto state = static_cast<KeyboardKeyState>(s);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->keyboardKey) {
            listener->keyboardKey(listener->GetWindow(), serial, time, key, state);
        }
    }
}

void OnKeyboardModifiers(void *, struct wl_keyboard *,
                         uint32_t serial, uint32_t modsDepressed,
                         uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
{
    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->keyboardModifiers) {
            listener->keyboardModifiers(listener->GetWindow(), serial, modsDepressed, modsLatched, modsLocked, group);
        }
    }
}

void OnKeyboardRepeatInfo(void *, struct wl_keyboard *,
                          int32_t rate, int32_t delay)
{
    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->keyboardRepeatInfo) {
            listener->keyboardRepeatInfo(listener->GetWindow(), rate, delay);
        }
    }
}

void OnTouchDown(void *, struct wl_touch *,
                 uint32_t serial, uint32_t time,
                 struct wl_surface *surface,
                 int32_t id, wl_fixed_t tx, wl_fixed_t ty)
{
    double x = wl_fixed_to_double(tx);
    double y = wl_fixed_to_double(ty);

    auto window = static_cast<void *>(wl_surface_get_user_data(surface));
    g_setFocus(window);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->touchDown) {
            listener->touchDown(listener->GetWindow(), serial, time, id, x, y);
        }
    }
}

void OnTouchUp(void *, struct wl_touch *,
               uint32_t serial, uint32_t time, int32_t id)
{
    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->touchUp) {
            listener->touchUp(listener->GetWindow(), serial, time, id);
        }
    }
}

void OnTouchMotion(void *, struct wl_touch *,
                   uint32_t time, int32_t id, wl_fixed_t tx, wl_fixed_t ty)
{
    double x = wl_fixed_to_double(tx);
    double y = wl_fixed_to_double(ty);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->touchMotion) {
            listener->touchMotion(listener->GetWindow(), time, id, x, y);
        }
    }
}

void OnTouchFrame(void *, struct wl_touch *)
{
    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->touchFrame) {
            listener->touchFrame(listener->GetWindow());
        }
    }
}

void OnTouchCancel(void *, struct wl_touch *)
{
    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->touchCancel) {
            listener->touchCancel(listener->GetWindow());
        }
    }
}

void OnTouchShape(void *, struct wl_touch *,
                  int32_t id, wl_fixed_t ma, wl_fixed_t mi)
{
    double major = wl_fixed_to_double(ma);
    double minor = wl_fixed_to_double(mi);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->touchShape) {
            listener->touchShape(listener->GetWindow(), id, major, minor);
        }
    }
}

void OnTouchOrientation(void *, struct wl_touch *,
                        int32_t id, wl_fixed_t o)
{
    double orientation = wl_fixed_to_double(o);

    const auto& inputListeners = g_getFocus();
    for (const auto& listener : inputListeners) {
        if (listener->touchOrientation) {
            listener->touchOrientation(listener->GetWindow(), id, orientation);
        }
    }
}
} // namespace

void InputListenerManager::RegisterPointerListener(uint32_t caps)
{
    bool havePointerCapability = !!(caps & WL_SEAT_CAPABILITY_POINTER);
    if (havePointerCapability == true && pointer == nullptr) {
        static struct wl_pointer_listener listener = {
            OnPointerEnter,
            OnPointerLeave,
            OnPointerMotion,
            OnPointerButton,
            OnPointerAxis,
            OnPointerFrame,
            OnPointerAxisSource,
            OnPointerAxisStop,
            OnPointerAxisDiscrete,
        };

        pointer = wl_seat_get_pointer(seat);
        if (pointer) {
            wl_pointer_add_listener(pointer, &listener, nullptr);
        }
    }

    if (havePointerCapability == false && pointer != nullptr) {
        wl_pointer_destroy(pointer);
        pointer = nullptr;
    }
}

void InputListenerManager::RegisterKeyboardListener(uint32_t caps)
{
    bool haveKeyboardCapability = !!(caps & WL_SEAT_CAPABILITY_KEYBOARD);
    if (haveKeyboardCapability == true && keyboard == nullptr) {
        static struct wl_keyboard_listener listener = {
            OnKeyboardKeymap,
            OnKeyboardEnter,
            OnKeyboardLeave,
            OnKeyboardKey,
            OnKeyboardModifiers,
            OnKeyboardRepeatInfo,
        };

        keyboard = wl_seat_get_keyboard(seat);
        if (keyboard) {
            wl_keyboard_add_listener(keyboard, &listener, nullptr);
        }
    }

    if (haveKeyboardCapability == false && keyboard != nullptr) {
        wl_keyboard_destroy(keyboard);
        keyboard = nullptr;
    }
}

void InputListenerManager::RegisterTouchListener(uint32_t caps)
{
    bool haveTouchCapability = !!(caps & WL_SEAT_CAPABILITY_TOUCH);
    if (haveTouchCapability == true && touch == nullptr) {
        static const struct wl_touch_listener listener = {
            OnTouchDown,
            OnTouchUp,
            OnTouchMotion,
            OnTouchFrame,
            OnTouchCancel,
            OnTouchShape,
            OnTouchOrientation,
        };

        touch = wl_seat_get_touch(seat);
        if (touch) {
            wl_touch_add_listener(touch, &listener, nullptr);
        }
    }

    if (haveTouchCapability == false && touch != nullptr) {
        wl_touch_destroy(touch);
        touch = nullptr;
    }
}
} // namespace OHOS
