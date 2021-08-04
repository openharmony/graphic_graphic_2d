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

#ifndef INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_INPUT_TYPE_H
#define INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_INPUT_TYPE_H

#include <functional>

namespace OHOS {
enum PointerButtonState {
    POINTER_BUTTON_STATE_RELEASED,
    POINTER_BUTTON_STATE_PRESSED,
};

enum PointerAxis {
    POINTER_AXIS_VERTICAL_SCROLL,
    POINTER_AXIS_HORIZONTAL_SCROLL,
};

enum PointerAxisSource {
    POINTER_AXIS_SOURCE_WHEEL,
    POINTER_AXIS_SOURCE_FINGER,
    POINTER_AXIS_SOURCE_CONTINUOUS,
    POINTER_AXIS_SOURCE_WHEEL_TILT,
};

enum KeyboardKeymapFormat {
    KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP,
    KEYBOARD_KEYMAP_FORMAT_XKB_V1,
};

enum KeyboardKeyState {
    KEYBOARD_KEY_STATE_RELEASED,
    KEYBOARD_KEY_STATE_PRESSED,
};

#define __BIND1_ARGS ::std::placeholders::_1
#define __BIND2_ARGS __BIND1_ARGS, ::std::placeholders::_2
#define __BIND3_ARGS __BIND2_ARGS, ::std::placeholders::_3
#define __BIND4_ARGS __BIND3_ARGS, ::std::placeholders::_4
#define __BIND5_ARGS __BIND4_ARGS, ::std::placeholders::_5
#define __BIND6_ARGS __BIND5_ARGS, ::std::placeholders::_6
#define __BIND7_ARGS __BIND6_ARGS, ::std::placeholders::_7
#define __BIND8_ARGS __BIND7_ARGS, ::std::placeholders::_8

#define POINTER_ENTER_ARG __BIND4_ARGS
using PointerEnterFunc = std::function<void(void *, uint32_t serial,
                                            double x, double y)>;

#define POINTER_LEAVE_ARG __BIND2_ARGS
using PointerLeaveFunc = std::function<void(void *, uint32_t serial)>;

#define POINTER_MOTION_ARG __BIND4_ARGS
using PointerMotionFunc = std::function<void(void *, uint32_t time,
                                             double x, double y)>;

#define POINTER_BUTTON_ARG __BIND5_ARGS
using PointerButtonFunc = std::function<void(void *, uint32_t serial,
                                             uint32_t time,
                                             uint32_t button,
                                             PointerButtonState state)>;

#define POINTER_FRAME_ARG __BIND1_ARGS
using PointerFrameFunc = std::function<void(void *)>;

#define POINTER_AXIS_ARG __BIND4_ARGS
using PointerAxisFunc = std::function<void(void *, uint32_t time,
                                           PointerAxis axis,
                                           double value)>;

#define POINTER_AXIS_SOURCE_ARG __BIND2_ARGS
using PointerAxisSourceFunc = std::function<void(void *, PointerAxisSource axisSource)>;

#define POINTER_AXIS_STOP_ARG __BIND3_ARGS
using PointerAxisStopFunc = std::function<void(void *, uint32_t time,
                                               PointerAxis axis)>;

#define POINTER_AXIS_DISCRETE_ARG __BIND3_ARGS
using PointerAxisDiscreteFunc = std::function<void(void *, PointerAxis axis,
                                                   int32_t discrete)>;

#define KEYBOARD_KEYMAP_ARG __BIND4_ARGS
using KeyboardKeymapFunc = std::function<void(void *, KeyboardKeymapFormat format,
                                              int32_t fd,
                                              uint32_t size)>;

#define KEYBOARD_ENTER_ARG __BIND3_ARGS
using KeyboardEnterFunc = std::function<void(void *, uint32_t serial,
                                             const std::vector<uint32_t> &keys)>;

#define KEYBOARD_LEAVE_ARG __BIND2_ARGS
using KeyboardLeaveFunc = std::function<void(void *, uint32_t serial)>;

#define KEYBOARD_KEY_ARG __BIND5_ARGS
using KeyboardKeyFunc = std::function<void(void *, uint32_t serial,
                                           uint32_t time,
                                           uint32_t key,
                                           KeyboardKeyState state)>;

#define KEYBOARD_MODIFIERS_ARG __BIND6_ARGS
using KeyboardModifiersFunc = std::function<void(void *, uint32_t serial,
                                                 uint32_t modsDepressed,
                                                 uint32_t modsLatched,
                                                 uint32_t modsLocked,
                                                 uint32_t group)>;

#define KEYBOARD_REPEAT_INFO_ARG __BIND3_ARGS
using KeyboardRepeatInfoFunc = std::function<void(void *, int32_t rate,
                                                  int32_t delay)>;

#define TOUCH_DOWN_ARG __BIND6_ARGS
using TouchDownFunc = std::function<void(void *, uint32_t serial,
                                         uint32_t time,
                                         int32_t id,
                                         double x, double y)>;

#define TOUCH_UP_ARG __BIND4_ARGS
using TouchUpFunc = std::function<void(void *, uint32_t serial,
                                       uint32_t time,
                                       int32_t id)>;

#define TOUCH_MOTION_ARG __BIND5_ARGS
using TouchMotionFunc = std::function<void(void *, uint32_t time,
                                           int32_t id,
                                           double x, double y)>;

#define TOUCH_FRAME_ARG __BIND1_ARGS
using TouchFrameFunc = std::function<void(void *)>;

#define TOUCH_CANCEL_ARG __BIND1_ARGS
using TouchCancelFunc = std::function<void(void *)>;

#define TOUCH_SHAPE_ARG __BIND4_ARGS
using TouchShapeFunc = std::function<void(void *, int32_t id,
                                          double major,
                                          double minor)>;

#define TOUCH_ORIENTATION_ARG __BIND3_ARGS
using TouchOrientationFunc = std::function<void(void *, int32_t id,
                                                double orientation)>;
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_INPUT_TYPE_H
