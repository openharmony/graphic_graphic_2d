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

#include "window_manager_controller_client.h"
#include "video_window.h"

#include <arpa/inet.h>
#include <cerrno>
#include <map>
#include <queue>
#include <string>
#include <sys/mman.h>
#include <timer.h>
#include <unistd.h>

#include <display_gralloc.h>
#include <drm_fourcc.h>
#include <ilm_client.h>
#include <ilm_common.h>
#include <ilm_control.h>
#include <ilm_input.h>
#include <key_event.h>
#include <mouse_event.h>

#include "window_manager_define.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WindowManagerClient" };
} // namespace

static FuncShotDone g_shotScreenDoneCb = nullptr;
static FuncShotDone g_shotSurfaceDoneCb = nullptr;
static uint32_t g_currentWinId = 0;
static uint32_t g_touchId[MAX_TOUCH_NUM] = {0};

constexpr uint32_t BTN_LEFT = 0x110;
constexpr uint32_t BTN_RIGHT = 0x111;
constexpr uint32_t BTN_MIDDLE = 0x112;
constexpr uint32_t BTN_FORWARD = 0x115;
constexpr uint32_t BTN_BACK = 0x116;

constexpr uint32_t LINUX_KEY_BACK = 158;

constexpr uint32_t BUTTON_SERIAL_BASE = 1;
constexpr int32_t BUTTON_PRESSED = 1;
constexpr int32_t KEY_PRESSED = 1;

constexpr int32_t SEND_OK = 0;
constexpr int32_t SEND_ERROR = -1;

constexpr int32_t BIND_INTERFACE_NUM_THREE = 3;
constexpr int32_t CURSOR_TYPE_NUM = 4;
constexpr int32_t STRIDE_NUM = 4;
constexpr int32_t MODIFY_OFFSET = 32;
constexpr int32_t ACTION_TYPE_ONE = 1;
constexpr int32_t ACTION_TYPE_TWO = 2;
constexpr int32_t ACTION_TYPE_THREE = 3;
constexpr int32_t ACTION_TYPE_FOUR = 4;
constexpr int32_t ACTION_TYPE_FIVE = 5;

static struct MouseProperty g_cachedMouseInfo;
static uint32_t g_occurredTime = 0;
static uint32_t g_buttonSerial = 0;

#define LOCK(mutexName) \
    std::lock_guard<std::mutex> lock(mutexName)

#define GET_WINDOWINFO(info, id, ret) \
    InnerWindowInfo* info = LayerControllerClient::GetInstance()->GetInnerWindowInfoFromId((uint32_t)id); \
    if (info == nullptr) { \
        WMLOGFE("id: %{public}d, window info is nullptr", id); \
        return ret; \
    }

#define GET_WINDOWINFO_VOID(info, id) \
    InnerWindowInfo* info = LayerControllerClient::GetInstance()->GetInnerWindowInfoFromId((uint32_t)id); \
    if (info == nullptr) { \
        WMLOGFE("id: %{public}d, window info is nullptr", id); \
        return; \
    }

#define GET_WINDOWINFO_INNER(info, id, ret) \
    InnerWindowInfo* info = GetInnerWindowInfoFromId((uint32_t)id); \
    if (info == nullptr) { \
        WMLOGFE("id: %{public}d, window info is nullptr", id); \
        return ret; \
    }

#define GET_WINDOWINFO_INNER_VOID(info, id) \
    InnerWindowInfo* info = GetInnerWindowInfoFromId((uint32_t)id); \
    if (info == nullptr) { \
        WMLOGFE("id: %{public}d, window info is nullptr", id); \
        return; \
    }

static int32_t GetDrmFmtByPixelFmt(int32_t pixelFmt)
{
    constexpr FormatTbl fmtTbl[] = {
        {PIXEL_FMT_RGB_565, DRM_FORMAT_RGB565},
        {PIXEL_FMT_RGBX_4444, DRM_FORMAT_RGBX4444},
        {PIXEL_FMT_RGBA_4444, DRM_FORMAT_RGBA4444},
        {PIXEL_FMT_RGBX_5551, DRM_FORMAT_RGBX5551},
        {PIXEL_FMT_RGBA_5551, DRM_FORMAT_RGBA5551},
        {PIXEL_FMT_RGBX_8888, DRM_FORMAT_RGBX8888},
        {PIXEL_FMT_RGBA_8888, DRM_FORMAT_RGBA8888},
        {PIXEL_FMT_RGB_888, DRM_FORMAT_RGB888},
        {PIXEL_FMT_BGR_565, DRM_FORMAT_BGR565},
        {PIXEL_FMT_BGRX_4444, DRM_FORMAT_BGRX4444},
        {PIXEL_FMT_BGRA_4444, DRM_FORMAT_BGRA4444},
        {PIXEL_FMT_BGRX_5551, DRM_FORMAT_BGRX5551},
        {PIXEL_FMT_BGRA_5551, DRM_FORMAT_BGRA5551},
        {PIXEL_FMT_BGRX_8888, DRM_FORMAT_BGRX8888},
        {PIXEL_FMT_BGRA_8888, DRM_FORMAT_BGRA8888},
        {PIXEL_FMT_YUV_422_I, DRM_FORMAT_YUV422},
        {PIXEL_FMT_YUYV_422_PKG, DRM_FORMAT_YUYV},
        {PIXEL_FMT_UYVY_422_PKG, DRM_FORMAT_UYVY},
        {PIXEL_FMT_YVYU_422_PKG, DRM_FORMAT_YVYU},
        {PIXEL_FMT_VYUY_422_PKG, DRM_FORMAT_VYUY},
    };

    int32_t drmFmt = DRM_FORMAT_RGBA8888;
    for (int i = 0; i < (int)(sizeof(fmtTbl) / sizeof(FormatTbl)); i++) {
        if (fmtTbl[i].pix_fmt == pixelFmt) {
            drmFmt = fmtTbl[i].drm_fmt;
        }
    }

    return drmFmt;
}

static int32_t CreateShmFile(int32_t size)
{
    static const char tempPath[] = "/weston-shared-XXXXXX";
    static const char path[] = "/dev/socket";
    size_t len = sizeof(path) + sizeof(tempPath) + 1;
    std::unique_ptr<char[]> name = std::make_unique<char[]>(len);
    auto ret = strcpy_s(name.get(), len, path);
    if (ret) {
        WMLOG_W("strcpy_s: %{public}s", strerror(ret));
    }

    ret = strcat_s(name.get(), len, tempPath);
    if (ret) {
        WMLOG_W("strcpy_s: %{public}s", strerror(ret));
    }

    int32_t fd = mkstemp(name.get());
    if (fd < 0) {
        WMLOG_E("CreateShmFile failed, mktemp failed with %{public}d", fd);
        return -1;
    }

    uint32_t flags = fcntl(fd, F_GETFD);
    fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
    unlink(name.get());

    ret = ftruncate(fd, size);
    if (ret < 0) {
        WMLOG_E("CreateShmFile failed, ftruncate failed with %{public}d", ret);
        close(fd);
        return -1;
    }

    return fd;
}

static void ShmFormat(void *data, struct wl_shm *wlshm, uint32_t format)
{
    WLContextStruct* wlcontext = (WLContextStruct*)data;
    wlcontext->formats |= (1 << format);
}

static int32_t CreateCursors(WLContextStruct& wlcontext)
{
    int32_t size = 32;
    static const std::string leftPtrs[] = {
        "left_ptr",
        "default",
        "top_left_arrow",
        "left-arrow"
    };

    wlcontext.cursor_theme = wl_cursor_theme_load(nullptr, size, wlcontext.wl_shm);
    if (wlcontext.cursor_theme == nullptr) {
        WMLOG_I("could not load default theme");
        return -1;
    }

    wlcontext.cursor = nullptr;
    for (int i = 0; i < sizeof(leftPtrs) / sizeof(*leftPtrs); i++) {
        wlcontext.cursor = wl_cursor_theme_get_cursor(wlcontext.cursor_theme, leftPtrs[i].c_str());
        if (wlcontext.cursor) {
            break;
        }
    }

    if (wlcontext.cursor == nullptr)  {
        WMLOG_I("could not load cursor");
        return -1;
    } else {
        return 0;
    }
}

static int32_t SetPointerImage(const WLContextStruct& wlcontext)
{
    struct wl_cursor *cursor = nullptr;
    struct wl_cursor_image *image = nullptr;
    struct wl_buffer *buffer = nullptr;

    if (!wlcontext.wl_pointer || !wlcontext.cursor) {
        WMLOG_I("no wl_pointer or no wl_cursor");
        return -1;
    }

    cursor = wlcontext.cursor;
    image = cursor->images[0];
    buffer = wl_cursor_image_get_buffer(image);
    if (!buffer) {
        WMLOG_I("buffer for cursor not available");
        return -1;
    }

    wl_pointer_set_cursor(wlcontext.wl_pointer, 0,
                          wlcontext.wl_pointer_surface,
                          image->hotspot_x, image->hotspot_y);
    wl_surface_attach(wlcontext.wl_pointer_surface, buffer, 0, 0);
    wl_surface_damage(wlcontext.wl_pointer_surface, 0, 0,
                      image->width, image->height);
    wl_surface_commit(wlcontext.wl_pointer_surface);

    return 0;
}

static void PointerHandleEnter(void *data, struct wl_pointer *wlPointer, uint32_t serial,
                               struct wl_surface *wlSurface, wl_fixed_t sx, wl_fixed_t sy)
{
    int x = wl_fixed_to_int(sx);
    int y = wl_fixed_to_int(sy);
    WMLOG_I("PointerHandleEnter,x(%{public}d),y(%{public}d) serial(%{public}d)", x, y, serial);

    auto wlContext = reinterpret_cast<WLContextStruct*>(data);
    if (wlContext == nullptr) {
        WMLOG_I("%{public}s wlContext is nullptr", __func__);
        return;
    }

    SetPointerImage(*wlContext);

    InnerWindowInfo* wininfo = (InnerWindowInfo*)wl_surface_get_user_data(wlSurface);
    if (wininfo == nullptr) {
        WMLOG_I("%{public}s wininfo is nullptr", __func__);
        return;
    }
    g_currentWinId = wininfo->windowid;
    if (wininfo->pointerEnterCb) {
        wininfo->pointerEnterCb(x, y, (int)serial);
    }
}

static void PointerHandleLeave(void *data, struct wl_pointer *wlPointer, uint32_t serial,
                               struct wl_surface *wlSurface)
{
    WMLOG_I("PointerHandleLeave %{public}d", serial);
    InnerWindowInfo* wininfo = (InnerWindowInfo*)wl_surface_get_user_data(wlSurface);
    if (wininfo == nullptr) {
        WMLOG_I("%{public}s wininfo is nullptr", __func__);
        return;
    }
    g_currentWinId = wininfo->windowid;
    if (wininfo->pointerLeaveCb) {
        wininfo->pointerLeaveCb((int)serial);
    }
}

static int32_t SendCallbackForMouse(void *data)
{
    WMLOG_I("SendCallbackForMouse %{public}d %{public}f %{public}f %{public}d %{public}x",
            g_cachedMouseInfo.action, g_cachedMouseInfo.mmiPoint.GetX(), g_cachedMouseInfo.mmiPoint.GetY(),
            g_cachedMouseInfo.actionButton, g_cachedMouseInfo.pressedButtons);
    TouchEvent touchEvent;
    std::shared_ptr<MouseEvent> mouseEvent = std::make_shared<MouseEvent>();
    struct MultimodalProperty multimodal = {
        .highLevelEvent = 0,
        .uuid = "",
        .sourceType = MultimodalEvent::MOUSE,
        .occurredTime = g_occurredTime,
        .deviceId = "",
        .inputDeviceId = 0,
        .isHighLevelEvent = false,
    };
    struct ManipulationProperty manipulationProperty = {
        .startTime = g_occurredTime,
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
    mouseEvent->Initialize(multimodal, g_cachedMouseInfo);
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    touchEvent.SetMultimodalEvent(mouseEvent);
    GET_WINDOWINFO(wininfo, g_currentWinId, SEND_ERROR);
    if (wininfo->onTouchCb) {
        wininfo->onTouchCb(touchEvent);
    }
    return SEND_ERROR;
}

static void PointerHandleMotion(void *data, struct wl_pointer *wlPointer, uint32_t time,
                                wl_fixed_t sx, wl_fixed_t sy)
{
    int x = wl_fixed_to_int(sx);
    int y = wl_fixed_to_int(sy);

    if (static_cast<uint64_t>(g_cachedMouseInfo.actionButton) & MouseEvent::LEFT_BUTTON) {
        g_cachedMouseInfo.action = MouseEvent::MOVE;
    } else {
        g_cachedMouseInfo.action = MouseEvent::HOVER_MOVE;
    }

    MmiPoint point(x, y, 0);
    g_cachedMouseInfo.mmiPoint = point;
    g_occurredTime = time;
    WMLOG_I("PointerHandleMotion new,x(%{public}d),y(%{public}d) time(%{public}d)", x, y, time);
    SendCallbackForMouse(data);

    auto wlContext = reinterpret_cast<WLContextStruct*>(data);
    if (wlContext == nullptr) {
        WMLOG_I("%{public}s wlContext is nullptr", __func__);
        return;
    }

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    if (wininfo->pointerMotionCb) {
        wininfo->pointerMotionCb(x, y, (int)time);
    }
}

static void PointerHandleAxis(void *data, struct wl_pointer *wlPointer, uint32_t time,
                              uint32_t axis, wl_fixed_t value)
{
    g_cachedMouseInfo.action = MouseEvent::SCROLL;
    g_cachedMouseInfo.scrollingDelta = wl_fixed_to_int(value);
    g_cachedMouseInfo.scrollType = axis;
    g_occurredTime = time;
    WMLOG_I("PointerHandleAxis");
}

static uint32_t GetMouseButton(uint32_t button)
{
    switch (button) {
        case BTN_LEFT:
            return MouseEvent::LEFT_BUTTON;
        case BTN_RIGHT:
            return MouseEvent::RIGHT_BUTTON;
        case BTN_MIDDLE:
            return MouseEvent::MIDDLE_BUTTON;
        case BTN_FORWARD:
            return MouseEvent::FORWARD_BUTTON;
        case BTN_BACK:
            return MouseEvent::BACK_BUTTON;
        default:
            return MouseEvent::NONE_BUTTON;
    }
}

static void PointerHandleButton(void *data, struct wl_pointer *wlPointer, uint32_t serial,
                                uint32_t time, uint32_t button, uint32_t state)
{
    WMLOG_I("PointerHandleButton %{public}d %{public}d %{public}d %{public}d", serial, button, state, time);

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    if (wininfo->pointerButtonCb) {
        wininfo->pointerButtonCb((int)serial, (int)button, (int)state, (int)time);
    }

    if (serial <= g_buttonSerial && serial > BUTTON_SERIAL_BASE) {
        WMLOG_I("PointerHandleButton recv repeat button %{public}d %{public}d %{public}d %{public}d",
            serial, button, state, time);
        return;
    }

    uint32_t changeButton = GetMouseButton(button);
    uint32_t pressedButtons = g_cachedMouseInfo.pressedButtons;
    if (state == BUTTON_PRESSED) {
        pressedButtons |= changeButton;
    } else {
        pressedButtons ^= changeButton;
    }

    g_cachedMouseInfo.pressedButtons = pressedButtons;
    g_cachedMouseInfo.actionButton = changeButton;
    g_occurredTime = time;
    int result = SendCallbackForMouse(data);
    if (result == SEND_OK) {
        g_buttonSerial = serial;
    }
}

static void PointerHandleFrame(void *data, struct wl_pointer *wlPointer)
{
    WMLOG_I("PointerHandleFrame %{public}d %{public}f %{public}f %{public}d %{public}d",
            g_cachedMouseInfo.action, g_cachedMouseInfo.mmiPoint.GetX(), g_cachedMouseInfo.mmiPoint.GetY(),
            g_cachedMouseInfo.actionButton, g_cachedMouseInfo.pressedButtons);
    SendCallbackForMouse(data);
}

static void PointerHandleAxisSource(void *data, struct wl_pointer *wlPointer, uint32_t axisSource)
{
    WMLOG_I("PointerHandleAxisSource axisSource=%{public}d", axisSource);

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    if (wininfo->pointerAxisSourceCb) {
        wininfo->pointerAxisSourceCb((int)axisSource);
    }
}

static void PointerHandleAxisStop(void *data, struct wl_pointer *wlPointer, uint32_t time, uint32_t axis)
{
    WMLOG_I("PointerHandleAxisStop time=%{public}d,axis=%{public}d", time, axis);

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    if (wininfo->pointerAxisStopCb) {
        wininfo->pointerAxisStopCb((int)time, (int)axis);
    }
}

static void PointerHandleAxisDiscrete(void *data, struct wl_pointer *wlPointer, uint32_t axis, int32_t discrete)
{
    WMLOG_I("PointerHandleAxisDiscrete axis=%{public}d,discrete=%{public}d", axis, discrete);

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    if (wininfo->pointerAxisDiscreteCb) {
        wininfo->pointerAxisDiscreteCb((int)axis, (int)discrete);
    }
}

static void PointerHandleCapability(WLContextStruct &wlContext, uint32_t caps)
{
    bool havePointerCapability = !!(caps & WL_SEAT_CAPABILITY_POINTER);
    if (havePointerCapability == true && wlContext.wl_pointer == nullptr) {
        wlContext.wl_pointer = wl_seat_get_pointer(wlContext.wl_seat);

        static struct wl_pointer_listener pointerListener = {
            PointerHandleEnter,
            PointerHandleLeave,
            PointerHandleMotion,
            PointerHandleButton,
            PointerHandleAxis,
            PointerHandleFrame,
            PointerHandleAxisSource,
            PointerHandleAxisStop,
            PointerHandleAxisDiscrete
        };
        if (wlContext.wl_pointer) {
            wl_pointer_add_listener(wlContext.wl_pointer, &pointerListener, &wlContext);
        }

        CreateCursors(wlContext);
        wlContext.wl_pointer_surface = wl_compositor_create_surface(wlContext.wlCompositor);
    }

    if (havePointerCapability == false && wlContext.wl_pointer != nullptr) {
        wl_pointer_destroy(wlContext.wl_pointer);
        wlContext.wl_pointer = nullptr;

        if (wlContext.wl_pointer_surface) {
            wl_surface_destroy(wlContext.wl_pointer_surface);
            wlContext.wl_pointer_surface = nullptr;
        }

        if (wlContext.cursor_theme) {
            wl_cursor_theme_destroy(wlContext.cursor_theme);
        }
    }
}

static void KeyboardHandleKeyMap(void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size)
{
    WMLOG_I("KeyboardHandleKeyMap format=%{public}d", format);
    close(fd);
}

static void KeyboardHandleEnter(void *data, struct wl_keyboard *keyboard, uint32_t serial,
                                struct wl_surface *surface, struct wl_array *keys)
{
    InnerWindowInfo* wininfo = (InnerWindowInfo*)wl_surface_get_user_data(surface);
    if (wininfo == nullptr) {
        WMLOG_I("%{public}s wininfo is nullptr", __func__);
        return;
    }
    g_currentWinId = wininfo->windowid;
    WMLOG_D("KeyboardHandleEnter");
}

static void KeyboardHandleLeave(void *data, struct wl_keyboard *keyboard,
                                uint32_t serial, struct wl_surface *surface)
{
    WMLOG_D("KeyboardHandleLeave");
}

static void KeyboardHandleKey(void *data, struct wl_keyboard *keyboard,
                              uint32_t serial, uint32_t time,
                              uint32_t key, uint32_t state)
{
    WMLOG_D("KeyboardHandleKey s=%{public}u key=%{public}u state=%{public}u", serial, key, state);

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    if (wininfo->keyboardKeyCb == nullptr) {
        return;
    }

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
        .isPressed = (state == KEY_PRESSED),
        .keyCode = key,
        .keyDownDuration = 0,
    };

    static uint32_t keyDownTime = 0;
    if (state == KEY_PRESSED) {
        keyDownTime = time;
    } else {
        keyProperty.keyDownDuration = time - keyDownTime;
    }

    if (key == LINUX_KEY_BACK) {
        keyProperty.keyCode = KeyEvent::CODE_BACK;
    }

    KeyEvent event;
    event.Initialize(multiProperty, keyProperty);
    wininfo->keyboardKeyCb(event);
}

static void KeyboardHandleModifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial,
    uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
{
}

static void KeyboardHandleCapability(WLContextStruct &wlContext, uint32_t caps)
{
    bool haveKeyboardCapability = !!(caps & WL_SEAT_CAPABILITY_KEYBOARD);
    if (haveKeyboardCapability == true && wlContext.wlKeyboard == nullptr) {
        wlContext.wlKeyboard = wl_seat_get_keyboard(wlContext.wl_seat);

        static struct wl_keyboard_listener keyboardListener = {
            KeyboardHandleKeyMap,
            KeyboardHandleEnter,
            KeyboardHandleLeave,
            KeyboardHandleKey,
            KeyboardHandleModifiers,
        };

        if (wlContext.wlKeyboard) {
            wl_keyboard_add_listener(wlContext.wlKeyboard, &keyboardListener, &wlContext);
        }
    }

    if (haveKeyboardCapability == false && wlContext.wlKeyboard != nullptr) {
        wl_keyboard_destroy(wlContext.wlKeyboard);
        wlContext.wlKeyboard = nullptr;
    }
}

static void TouchHandleDown(void *data,
    struct wl_touch *wltouch, uint32_t serial, uint32_t time,
    struct wl_surface *surface, int32_t id, wl_fixed_t wlx, wl_fixed_t wly)
{
    int x = wl_fixed_to_int(wlx);
    int y = wl_fixed_to_int(wly);
    WMLOG_D("%{public}s, x=%{public}d y=%{public}d serial=%{public}d time=%{public}d",
        __func__, x, y, serial, time);

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

    InnerWindowInfo* wininfo = (InnerWindowInfo*)wl_surface_get_user_data(surface);
    if (wininfo == nullptr) {
        WMLOG_I("%{public}s wininfo is nullptr", __func__);
        return;
    }
    if (id < MAX_TOUCH_NUM) {
        g_touchId[id] = wininfo->windowid;
    }
    g_currentWinId = wininfo->windowid;
    if (wininfo->touchDownCb) {
        wininfo->touchDownCb(x, y, serial, time, id);
    }
}

static void ProcessActionEvent(ActionEventInfo& actionEvent, TouchProperty& touchProperty)
{
    if (actionEvent.isUp) {
        if (actionEventInfo.touchCount == 0) {
            touchProperty.action = ACTION_TYPE_TWO;
        } else {
            touchProperty.action = ACTION_TYPE_FIVE;
        }
        actionEvent.isUp = false;
    } else if (actionEvent.isDown) {
        if (actionEventInfo.touchCount == ACTION_TYPE_ONE) {
            touchProperty.action = ACTION_TYPE_ONE;
        } else {
            touchProperty.action = ACTION_TYPE_FOUR;
        }
        actionEvent.isDown = false;
    } else if (actionEvent.isMotion) {
        if (actionEventInfo.touchCount > 0) {
            touchProperty.action = ACTION_TYPE_THREE;
        }
        actionEvent.isMotion = false;
    }
}

static void TouchEventEncap(ActionEventInfo& actionEvent, TouchEvent& touchEvent, int32_t size)
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
    WMLOG_D("%{public}s, touchCount = %{public}d", __func__, actionEventInfo.touchCount);

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

static void TouchHandleUp(void *data,
    struct wl_touch *wltouch, uint32_t serial, uint32_t time, int32_t id)
{
    WMLOG_D("%{public}s, serial=%{public}d, time=%{public}d", __func__, serial, time);

    auto wlContext = reinterpret_cast<WLContextStruct*>(data);
    if (id < MAX_TOUCH_NUM) {
        actionEventInfo.touchCount--;
        actionEventInfo.isUp = true;
        actionEventInfo.touchEventInfos[id].isRefreshed = true;
        actionEventInfo.touchEventInfos[id].serial = serial;
        actionEventInfo.touchEventInfos[id].currentTime = time;
    }
    uint32_t winId = 0;
    if (id < MAX_TOUCH_NUM) {
        winId = g_touchId[id];
        g_touchId[id] = 0;
    }
    WMLOG_D("%{public}s, winId=%{public}d", __func__, winId);

    GET_WINDOWINFO_VOID(wininfo, winId);
    while (actionEventInfo.isUp || actionEventInfo.isDown || actionEventInfo.isMotion) {
        TouchEvent touchEvent;
        TouchEventEncap(actionEventInfo, touchEvent, MAX_TOUCH_NUM);
        if (wininfo->onTouchCb) {
            wininfo->onTouchCb(touchEvent);
        }
    }

    if (wlContext == nullptr) {
        WMLOG_I("%{public}s wlContext is nullptr", __func__);
        return;
    }
    if (wininfo->touchUpCb) {
        wininfo->touchUpCb(serial, time, id);
    }
}

static void TouchHandlEmotion(void *data,
    struct wl_touch *wltouch, uint32_t time, int32_t id, wl_fixed_t touchx, wl_fixed_t touchy)
{
    int x = wl_fixed_to_int(touchx);
    int y = wl_fixed_to_int(touchy);
    WMLOG_D("%{public}s, x=%{public}d, y=%{public}d, time=%{public}d", __func__, x, y, time);

    if (id < MAX_TOUCH_NUM) {
        actionEventInfo.isMotion = true;
        actionEventInfo.touchEventInfos[id].isRefreshed = true;
        actionEventInfo.touchEventInfos[id].currentTime = time;
        actionEventInfo.touchEventInfos[id].x = x;
        actionEventInfo.touchEventInfos[id].y = y;
    }

    auto wlContext = reinterpret_cast<WLContextStruct*>(data);
    if (wlContext == nullptr) {
        WMLOG_I("%{public}s wlContext is nullptr", __func__);
        return;
    }
    uint32_t winId = 0;
    if (id < MAX_TOUCH_NUM) {
        winId = g_touchId[id];
    }
    WMLOG_D("%{public}s, winId=%{public}d", __func__, winId);

    GET_WINDOWINFO_VOID(wininfo, winId);
    if (wininfo->touchEmotionCb) {
        wininfo->touchEmotionCb(x, y, time, id);
    }
}

static void TouchHandleFrame(void *data, struct wl_touch *wltouch)
{
    WMLOG_D("%{public}s", __func__);

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    while (actionEventInfo.isUp || actionEventInfo.isDown || actionEventInfo.isMotion) {
        TouchEvent touchEvent;
        TouchEventEncap(actionEventInfo, touchEvent, MAX_TOUCH_NUM);
        if (wininfo->onTouchCb) {
            wininfo->onTouchCb(touchEvent);
        }
    }
    if (wininfo->touchFrameCb) {
        wininfo->touchFrameCb();
    }
}

static void TouchHandleCancel(void *data, struct wl_touch *wltouch)
{
    WMLOG_D("TouchHandleCancel");

    GET_WINDOWINFO_VOID(wininfo, g_currentWinId);
    if (wininfo->touchCancelCb) {
        wininfo->touchCancelCb();
    }
}

static void TouchHandleShape(void *data,
    struct wl_touch *wltouch, int32_t id, wl_fixed_t major, wl_fixed_t minor)
{
    int majorout = wl_fixed_to_int(major);
    int minorout = wl_fixed_to_int(minor);
    WMLOG_D("TouchHandleShape major=%{public}d minorout=%{public}d", majorout, minorout);

    if (id < MAX_TOUCH_NUM) {
        actionEventInfo.touchEventInfos[id].touchPressure = (majorout + minorout) / 2.0f;
    }

    uint32_t winId = 0;
    if (id < MAX_TOUCH_NUM) {
        winId = g_touchId[id];
    }

    GET_WINDOWINFO_VOID(wininfo, winId);
    if (wininfo->touchShapeCb) {
        wininfo->touchShapeCb(majorout, minorout);
    }
}

static void TouchHandleOrientation(void *data,
    struct wl_touch *wltouch, int32_t id, wl_fixed_t orientation)
{
    int orientationout = wl_fixed_to_int(orientation);
    WMLOG_D("TouchHandleOrientation orientationout=%{public}d", orientationout);
    uint32_t winId = 0;
    if (id < MAX_TOUCH_NUM) {
        winId = g_touchId[id];
    }

    GET_WINDOWINFO_VOID(wininfo, winId);
    if (wininfo->touchOrientationCb) {
        wininfo->touchOrientationCb(id, orientationout);
    }
}

static void TouchHandleCapability(WLContextStruct &wlContext, uint32_t caps)
{
    bool haveTouchCapability = !!(caps & WL_SEAT_CAPABILITY_TOUCH);
    if (haveTouchCapability == true && wlContext.wlTouch == nullptr) {
        wlContext.wlTouch = wl_seat_get_touch(wlContext.wl_seat);

        static const struct wl_touch_listener touchListener = {
            TouchHandleDown,
            TouchHandleUp,
            TouchHandlEmotion,
            TouchHandleFrame,
            TouchHandleCancel,
            TouchHandleShape,
            TouchHandleOrientation,
        };
        if (wlContext.wlTouch) {
            wl_touch_add_listener(wlContext.wlTouch, &touchListener, &wlContext);
        }
    }

    if (haveTouchCapability == false && wlContext.wlTouch != nullptr) {
        wl_touch_destroy(wlContext.wlTouch);
        wlContext.wlTouch = nullptr;
    }
}

static void SeatHandleCapabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
    WMLOG_I("SeatHandleCapabilities");

    auto wlContext = reinterpret_cast<WLContextStruct*>(data);
    if (wlContext == nullptr) {
        WMLOG_I("%{public}s wlContext is nullptr", __func__);
        return;
    }

    PointerHandleCapability(*wlContext, caps);
    KeyboardHandleCapability(*wlContext, caps);
    TouchHandleCapability(*wlContext, caps);
}

static void SeatName(void *data, struct wl_seat *wlSeat, const char *name)
{
}

extern "C"
{
void LayerControllerClient::registry_handle_global(void* data,
    struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
    WMLOG_I("LayerControllerClient::registry_handle_global--%{public}s", interface);

    auto wlContext = reinterpret_cast<WLContextStruct*>(data);
    if (wlContext == nullptr) {
        WMLOG_I("%{public}s wlContext is nullptr", __func__);
        return;
    }

    if (strcmp(interface, "wl_compositor") == 0) {
        wlContext->wlCompositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
        return;
    }

    if (strcmp(interface, "ivi_application") == 0) {
        auto ret = wl_registry_bind(registry, id, &ivi_application_interface, 1);
        wlContext->iviApp = (struct ivi_application*)ret;
        return;
    }

    if (strcmp(interface, "zwp_linux_dmabuf_v1") == 0) {
        auto ret = wl_registry_bind(registry, id, &zwp_linux_dmabuf_v1_interface, BIND_INTERFACE_NUM_THREE);
        wlContext->dmabuf = (struct zwp_linux_dmabuf_v1*)ret;
        return;
    }

    if (strcmp(interface, "wl_seat") == 0) {
        WMLOG_I("wlContext->enable_cursor=%{public}d", wlContext->enable_cursor);

        if (wlContext->enable_cursor) {
            wlContext->wl_seat = (struct wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 1);

            if (wlContext->wl_seat) {
                static struct wl_seat_listener seatListener = { SeatHandleCapabilities, SeatName };
                wl_seat_add_listener(wlContext->wl_seat, &seatListener, wlContext);
            }
        }
    }

    if (strcmp(interface, "wl_shm") == 0) {
        wlContext->wl_shm = (struct wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);

        if (wlContext->wl_shm) {
            static struct wl_shm_listener shmListenter = {ShmFormat};
            wl_shm_add_listener(wlContext->wl_shm, &shmListenter, wlContext);
        }
        return;
    }

    if (strcmp(interface, "wl_subcompositor") == 0) {
        auto ret = wl_registry_bind(registry, id, &wl_subcompositor_interface, 1);
        wlContext->wlSubCompositor = (struct wl_subcompositor*)ret;
        return;
    }

    if (strcmp(interface, "wp_viewport") == 0) {
        auto ret = wl_registry_bind(registry, id, &wp_viewporter_interface, 1);
        wlContext->viewporter = (struct wp_viewporter*)ret;
        return;
    }
}

void LayerControllerClient::registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}
} // extern "C"

sptr<LayerControllerClient> LayerControllerClient::instance = nullptr;
sptr<LayerControllerClient> LayerControllerClient::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new LayerControllerClient();
        }
    }

    return instance;
}

LayerControllerClient::LayerControllerClient() : m_screenWidth(0), m_screenHeight(0)
{
    WMLOG_I("DEBUG LayerControllerClient");
    static bool isInit = false;
    if (isInit) {
        WMLOG_I("Error: Second instance of Layer Controller detected");
    }
    isInit = true;

    initIlm();
    initScreen();
    pthread_t tid1;
    pthread_create(&tid1, NULL, LayerControllerClient::thread_display_dispatch, this);
}

LayerControllerClient::~LayerControllerClient()
{
    WMLOG_I("DEBUG ~LayerControllerClient");
    ilm_commitChanges();
    cleanupIlm();
}

bool LayerControllerClient::initIlm()
{
    // initializes the IVI LayerManagement Client
    (void)memset_s(&wlContextStruct_, sizeof(wlContextStruct_), 0, sizeof(wlContextStruct_));

    // retry to connect
    constexpr int32_t retryTimes = 60;
    for (int32_t i = 0; i < retryTimes; i++) {
        wlContextStruct_.wlDisplay = wl_display_connect(nullptr);
        if (wlContextStruct_.wlDisplay != nullptr) {
            break;
        } else {
            WMLOG_W("create display failed! (%{public}d/%{public}d)", i + 1, retryTimes);
        }

        constexpr int32_t sleepTimeFactor = 50 * 1000;
        int32_t sleepTime = i * sleepTimeFactor;
        usleep(sleepTime);
    }

    if (wlContextStruct_.wlDisplay == nullptr) {
        WMLOG_E("create display failed!");
        exit(1);
    }

    char *option = getenv("IVI_CLIENT_ENABLE_CURSOR");
    char *end = nullptr;
    if (option) {
        wlContextStruct_.enable_cursor = (uint8_t)strtol(option, &end, 0);
    } else {
        wlContextStruct_.enable_cursor = 1;
    }
    WMLOG_I("wlContextStruct_.enable_cursor = %{public}d", wlContextStruct_.enable_cursor);

    ilmErrorTypes callResult = ilm_initWithNativedisplay((t_ilm_nativedisplay)wlContextStruct_.wlDisplay);
    if (callResult != ILM_SUCCESS) {
        WMLOG_I("Error: ilm_initWithNativedisplay - %{public}s.", ILM_ERROR_STRING(callResult));
        exit(callResult);
        return false;
    } else {
        WMLOG_I("ilm_initWithNativedisplay succes!");
    }
    wlContextStruct_.wlRegistry = wl_display_get_registry(wlContextStruct_.wlDisplay);

    static const struct wl_registry_listener registry_listener = {
        LayerControllerClient::registry_handle_global,
        LayerControllerClient::registry_handle_global_remove
    };
    wl_registry_add_listener(wlContextStruct_.wlRegistry, &registry_listener, &wlContextStruct_);
    wl_display_roundtrip(wlContextStruct_.wlDisplay);
    wl_display_roundtrip(wlContextStruct_.wlDisplay);
    return true;
}

bool LayerControllerClient::cleanupIlm()
{
    // destroy the IVI LayerManagement Client
    ilmErrorTypes callResult = ilm_destroy();
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: ilm_destroy - %{public}s.", ILM_ERROR_STRING(callResult));
    }

    return true;
}

bool LayerControllerClient::initScreen()
{
    // Get screens and sizes
    ilmErrorTypes callResult = ILM_FAILED;
    struct ilmScreenProperties screenProperties;
    t_ilm_uint*  screenIds = NULL;
    t_ilm_uint   numberOfScreens  = 0;

    callResult = ilm_getScreenIDs(&numberOfScreens, &screenIds);
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: initScreen() ilm_getScreenIDs - %{public}s.", ILM_ERROR_STRING(callResult));
        return false;
    } else {
        WMLOG_I("Debug: ilm_getScreenIDs - %{public}s. number of screens = %{public}u", 
                ILM_ERROR_STRING(callResult), numberOfScreens);
        for (uint i = 0; i < numberOfScreens; i++) {
            WMLOG_I("Debug: Screen ID[%{public}u] = %{public}d", i, screenIds[i]);
        }
        m_screenId = 0;
    }

    free(screenIds);

    callResult = ilm_getPropertiesOfScreen(m_screenId, &screenProperties);
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: initScreen() ilm_getPropertiesOfScreen - %{public}s. Exiting.",
                ILM_ERROR_STRING(callResult));
        return false;
    }

    free(screenProperties.layerIds);

    m_screenWidth  = screenProperties.screenWidth;
    m_screenHeight = screenProperties.screenHeight;

    WMLOG_I("Info: initScreen() - screen size = %{public}u x %{public}u", m_screenWidth, m_screenHeight);

    return true;
}

[[ maybe_unused ]]
static void ShotScreenDoneCb(void* buffer, uint32_t size, uint32_t width, uint32_t height, uint32_t format)
{
    WMLOG_I("ShotScreenDoneCb size=%{public}d width=%{public}d height=%{public}d format=%{public}d",
            size, width, height, format);

    ImageInfo imageinfo;

    imageinfo.header.height = (uint16_t)height;
    imageinfo.header.width = (uint16_t)width;
    imageinfo.header.colorMode = PIXEL_FMT_BGRA_8888;
    imageinfo.data = (uint8_t*)buffer;
    imageinfo.dataSize = size;
    if (g_shotScreenDoneCb != nullptr) {
        g_shotScreenDoneCb(imageinfo);
    }
}

[[ maybe_unused ]]
static void ShotSurfaceDoneCb(void* buffer, uint32_t size, uint32_t width, uint32_t height, uint32_t format)
{
    WMLOG_I("ShotSurfaceDoneCb size=%{public}d width=%{public}d height=%{public}d format=%{public}d",
            size, width, height, format);

    ImageInfo imageinfo;

    imageinfo.header.height = (uint16_t)height;
    imageinfo.header.width = (uint16_t)width;
    imageinfo.header.colorMode = PIXEL_FMT_RGBA_8888;
    imageinfo.data = (uint8_t*)buffer;
    imageinfo.dataSize = size;
    if (g_shotSurfaceDoneCb != nullptr) {
        g_shotSurfaceDoneCb(imageinfo);
    }
}

void LayerControllerClient::ChangeWindowType(int32_t id, WindowType type)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::ChangeWindowType id=%{public}d,type=%{public}d", id, type);

    GET_WINDOWINFO_INNER_VOID(wininfo, id);

    if (type == wininfo->windowconfig.type) {
        WMLOG_E("LayerControllerClient::ChangeWindowType window type is no need change");
        return;
    }

    int32_t layerid = WINDOW_LAYER_DEFINE_NORMAL_ID;
    layerid += wininfo->windowconfig.type * LAYER_ID_TYPE_OFSSET;
    ilm_layerRemoveSurface(layerid, id);
    ilm_commitChanges();

    layerid = WINDOW_LAYER_DEFINE_NORMAL_ID;
    layerid += wininfo->windowconfig.type * LAYER_ID_TYPE_OFSSET;
    wininfo->windowconfig.type = type;
    ilm_layerAddSurface(layerid, id);
    ilm_commitChanges();
}

void LayerControllerClient::StartShotScreen(FuncShotDone doneCb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::StartShotScreen");
    g_shotScreenDoneCb = doneCb;
    ilm_takeScreenshot_buffer((t_ilm_uint)m_screenId, ShotScreenDoneCb);
}

void LayerControllerClient::StartShotWindow(int winID, FuncShotDone doneCb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::StartShotWindow id=%{public}d", winID);
    g_shotSurfaceDoneCb = doneCb;
    ilm_takeSurfaceScreenshot_buffer((t_ilm_uint)winID, ShotSurfaceDoneCb);
}

static void CreateSucceeded(void *data, struct zwp_linux_buffer_params_v1 *params, struct wl_buffer *newBuffer)
{
    BufferInfo* bufferInfo = reinterpret_cast<BufferInfo*>(data);
    if (bufferInfo == nullptr) {
        WMLOG_E("%{public}s data is null!!!", __func__);
        return;
    }

    WMLOG_I("wl_buffer create success");
    bufferInfo->Async([newBuffer](BufferInfo* bufferInfo) { bufferInfo->wlBuffer_ = newBuffer; });
}

static void CreateFailed(void *data, struct zwp_linux_buffer_params_v1 *params)
{
    BufferInfo* bufferInfo = reinterpret_cast<BufferInfo*>(data);
    if (bufferInfo == nullptr) {
        WMLOG_E("%{public}s data is null!!!", __func__);
        return;
    }

    WMLOG_I("wl_buffer create failed");
    bufferInfo->Async([](BufferInfo* bufferInfo) { bufferInfo->wlBuffer_ = nullptr; });
}

static struct wl_buffer *CreateZwpLinuxBuffer(struct zwp_linux_dmabuf_v1 *dmabuf,
    BufferInfo* bufferInfo, wl_display *display, uint64_t modifier)
{
    struct zwp_linux_buffer_params_v1 *params = zwp_linux_dmabuf_v1_create_params(dmabuf);
    if (params == nullptr) {
        return nullptr;
    }

    sptr<SurfaceBuffer> buffer = bufferInfo->buffer_.promote();
    if (buffer == nullptr) {
        return nullptr;
    }
    zwp_linux_buffer_params_v1_add(params, buffer->GetFileDescriptor(), 0, 0,
        buffer->GetWidth() * STRIDE_NUM, modifier >> MODIFY_OFFSET, modifier & 0xffffffff);

    static struct zwp_linux_buffer_params_v1_listener paramsListener = {
        CreateSucceeded,
        CreateFailed
    };
    zwp_linux_buffer_params_v1_add_listener(params, &paramsListener, bufferInfo);

    bufferInfo->AsyncInit();
    constexpr uint32_t flags = ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_Y_INVERT;
    zwp_linux_buffer_params_v1_create(params, buffer->GetWidth(), buffer->GetHeight(),
        GetDrmFmtByPixelFmt(buffer->GetFormat()), flags);
    WMLOG_I("LayerControllerClient::CreateWlBuffer create zwp linux buffer W=%{public}d H=%{public}d",
            buffer->GetWidth(), buffer->GetHeight());

    wl_display_flush(display);
    bufferInfo->Await();

    zwp_linux_buffer_params_v1_destroy(params);
    return bufferInfo->wlBuffer_;
}

namespace {
std::mutex g_surfaceWptrMutex;  // protect surface's wptr
} // namespace
void LayerControllerClient::BufferRelease(void *data, struct wl_buffer *wlBuffer)
{
    std::lock_guard<std::mutex> lock(g_surfaceWptrMutex);
    BufferInfo* bufferInfo = reinterpret_cast<BufferInfo*>(data);
    if (bufferInfo == nullptr) {
        WMLOG_E("%{public}s data is null!!!", __func__);
        return;
    }

    sptr<Surface> surface = bufferInfo->surface_.promote();
    if (surface == nullptr) {
        WMLOG_E("%{public}s surface is nullptr!!!", __func__);
        return;
    }

    sptr<SurfaceBuffer> buffer = bufferInfo->buffer_.promote();
    if (buffer == nullptr) {
        WMLOG_E("%{public}s buffer is nullptr!!!", __func__);
        return;
    }

    surface->ReleaseBuffer(buffer, -1);
    WMLOG_I("Success: BufferRelease.create Success.");
}

void LayerControllerClient::CreateWlBuffer(sptr<Surface>& surface, uint32_t windowId)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::CreateWlBuffer id=%{public}d", windowId);

    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = surface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        WMLOG_I("LayerControllerClient::CreateWlBuffer AcquireBuffer failed");
        return;
    }

    BufferInfo* bufferInfo = GetBufferInfo(buffer);
    bufferInfo->buffer_ = buffer;
    bufferInfo->surface_ = surface;

    GET_WINDOWINFO_INNER_VOID(windowInfo, windowId);

    if (bufferInfo->wlBuffer_ == nullptr) {
        uint64_t modifier = 0;
        CreateZwpLinuxBuffer(wlContextStruct_.dmabuf, bufferInfo, windowInfo->wlDisplay, modifier);
        if (bufferInfo->wlBuffer_) {
            static struct wl_buffer_listener bufferListener = {BufferRelease};
            wl_buffer_add_listener(bufferInfo->wlBuffer_, &bufferListener, bufferInfo);
        }
    }

    if (bufferInfo->wlBuffer_) {
        wl_surface_attach(windowInfo->wlSurface, bufferInfo->wlBuffer_, 0, 0);
        wl_surface_damage(windowInfo->wlSurface, damage.x, damage.y, damage.w, damage.h);
        wl_surface_commit(windowInfo->wlSurface);
        wl_display_flush(windowInfo->wlDisplay);
    }
    WMLOG_I("LayerControllerClient::CreateWlBuffer end");
}

void* LayerControllerClient::thread_display_dispatch(void* param)
{
    WMLOG_I("LayerControllerClient::thread_display_dispatch start");

    LayerControllerClient* ptr = static_cast<LayerControllerClient*>(param);
    while (true) {
        int32_t ret = wl_display_dispatch(ptr->wlContextStruct_.wlDisplay);
        if (ret == -1) {
            WMLOG_I("LayerControllerClient::thread_display_dispatch error, errno: %{public}d", errno);
            break;
        }
    }

    return nullptr;
}

bool LayerControllerClient::CreateSurface(int32_t id)
{
    WMLOG_I("CreateSurface (windowid = %{public}d) start", id);

    GET_WINDOWINFO(windowInfo, id, false);
    windowInfo->wlSurface = wl_compositor_create_surface(wlContextStruct_.wlCompositor);
    if (windowInfo->wlSurface == nullptr) {
        WMLOG_I("Error: CreateSurface wl_compositor_create_surface failed");
        return false;
    }

    WMLOG_I("CreateSurface wl_compositor_create_surface success");

    if (wlContextStruct_.iviApp) {
        windowInfo->iviSurface = ivi_application_surface_create(
            wlContextStruct_.iviApp, id, windowInfo->wlSurface);
        if (windowInfo->iviSurface == nullptr) {
            return false;
        }

        WMLOG_I("Success: CreateSurface ivi_application_surface_create(id = %{public}d) success", id);
    }

    windowInfo->surface = Surface::CreateSurfaceAsConsumer();
    if (windowInfo->surface) {
        windowInfo->listener = new SurfaceListener(windowInfo->surface, id);
        if (windowInfo->listener == nullptr) {
            WMLOG_E("windowInfo->listener new failed");
        }
        windowInfo->surface->RegisterConsumerListener(windowInfo->listener);

        int width = LayerControllerClient::GetInstance()->GetMaxWidth();
        int height = LayerControllerClient::GetInstance()->GetMaxHeight();
        windowInfo->surface->SetDefaultWidthAndHeight(width, height);
    }
    wl_surface_set_user_data(windowInfo->wlSurface, windowInfo);
    WMLOG_I("CreateSurface (windowid = %{public}d) end", id);
    return true;
}

InnerWindowInfo* LayerControllerClient::CreateWindow(int32_t id, WindowConfig& config)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::CreateWindow start");

    InnerWindowInfo newInfo = {
        .wlDisplay = wlContextStruct_.wlDisplay,
        .windowconfig = config,
        .windowid = id,
        .layerid = config.type * LAYER_ID_TYPE_OFSSET + LAYER_ID_APP_TYPE_BASE,
        .width = config.width,
        .height = config.height,
        .pos_x = config.pos_x,
        .pos_y = config.pos_y,
    };

    {
        LOCK(windowListMutex);
        m_windowList.push_back(newInfo);
    }
    wm_register_notification(config);
    CreateSurface(id);
    return GetInnerWindowInfoFromId(id);
}

namespace {
struct wl_buffer* CreateShmBuffer(struct wl_shm &wlShm, InnerWindowInfo &info)
{
    int32_t stride = info.width * STRIDE_NUM;
    int32_t size = stride * info.height;
    int32_t fd = CreateShmFile(size);
    if (fd < 0) {
        WMLOG_I("CreateSubWindow createwlSubSuface failed(fd)");
        return nullptr;
    }

    info.shm_data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (info.shm_data == nullptr) {
        WMLOG_I("CreateSubWindow createwlSubSuface failed(mmap)");
        close(fd);
        return nullptr;
    }

    auto pool = wl_shm_create_pool(&wlShm, fd, size);
    wl_buffer *buffer = nullptr;
    if (pool) {
        buffer = wl_shm_pool_create_buffer(pool, 0, info.width, info.height, stride, WL_SHM_FORMAT_XRGB8888);
        wl_shm_pool_destroy(pool);
    }

    if (buffer == nullptr) {
        WMLOG_I("CreateSubWindow createwlSubSuface failed(wl_buffer)");
    }

    close(fd);
    return buffer;
}

void GetSubInnerWindowInfo(InnerWindowInfo &info, const WLContextStruct wlContextStruct,
    const int subid, const int parentid, const WindowConfig &config)
{
    info.wlDisplay = wlContextStruct.wlDisplay;
    info.windowconfig = config;
    info.windowid = subid;
    info.parentid = parentid;
    info.subwidow = true;
    info.width = config.width;
    info.height = config.height;
    info.pos_x = config.pos_x;
    info.pos_y = config.pos_y;
    info.voLayerId = -1U;
}

bool CreateVideoSubWindow(InnerWindowInfo &newSubInfo, struct wl_shm &shm)
{
    uint32_t voLayerId = -1U;
    sptr<Surface> surface = nullptr;
    int32_t ret = VideoWindow::CreateLayer(newSubInfo, voLayerId, surface);
    if (ret == 0) {
        newSubInfo.voLayerId = voLayerId;
        newSubInfo.surface = surface;

        newSubInfo.wl_buffer = CreateShmBuffer(shm, newSubInfo);
        if (newSubInfo.wl_buffer == nullptr) {
            WMLOG_I("CreateVideoSubWindow CreateShmBuffer failed");
            return false;
        }

        if (newSubInfo.windowconfig.type == WINDOW_TYPE_VIDEO) {
            wl_surface_set_surface_type(newSubInfo.wlSurface, WL_SURFACE_TYPE_VIDEO);
        }

        newSubInfo.p_cb = wl_surface_frame(newSubInfo.wlSurface);
        wl_surface_attach(newSubInfo.wlSurface, newSubInfo.wl_buffer, 0, 0);
        wl_surface_damage(newSubInfo.wlSurface, 0, 0, newSubInfo.width, newSubInfo.height);
        wl_surface_commit(newSubInfo.wlSurface);
        return true;
    }

    WMLOG_I("CreateVideoSubWindow VideoWindow::CreateLayer failed");
    return false;
}
}

InnerWindowInfo* LayerControllerClient::CreateSubWindow(int32_t subid, int32_t parentid, WindowConfig& config)
{
    LOCK(mutex);
    WMLOG_I("%{public}s start parentID is %{public}d, subid is %{public}d",
        "LayerControllerClient::CreateSubWindow", parentid, subid);

    GET_WINDOWINFO(parentInnerWindowInfo, parentid, nullptr);
    InnerWindowInfo newSubInfo = {};
    GetSubInnerWindowInfo(newSubInfo, wlContextStruct_, subid, parentid, config);

    newSubInfo.wlSurface = wl_compositor_create_surface(wlContextStruct_.wlCompositor);
    if (newSubInfo.wlSurface == nullptr) {
        WMLOG_I("CreateSubWindow createwlSubSuface (subid = %{public}d) failed", subid);
        return nullptr;
    }
    WMLOG_I("CreateSubWindow createwlSubSuface (subid = %{public}d) success", subid);

    newSubInfo.wlSubSurface = wl_subcompositor_get_subsurface(wlContextStruct_.wlSubCompositor,
        newSubInfo.wlSurface, parentInnerWindowInfo->wlSurface);
    if (newSubInfo.wlSubSurface) {
        wl_subsurface_set_position(newSubInfo.wlSubSurface, newSubInfo.pos_x, newSubInfo.pos_y);
        wl_subsurface_set_desync(newSubInfo.wlSubSurface);
    }

    if (newSubInfo.windowconfig.type != WINDOW_TYPE_VIDEO) {
        newSubInfo.surface = Surface::CreateSurfaceAsConsumer();
        if (newSubInfo.surface) {
            newSubInfo.listener = new SurfaceListener(newSubInfo.surface, subid);
            if (newSubInfo.listener == nullptr) {
                WMLOG_E("newSubInfo.listener new failed");
            }
            newSubInfo.surface->RegisterConsumerListener(newSubInfo.listener);
        }
    } else {
        if (wlContextStruct_.wl_shm == nullptr) {
            WMLOG_I("CreateSubWindow createwlSubSuface wl_shm is nullptr");
            return nullptr;
        }

        if (!CreateVideoSubWindow(newSubInfo, *wlContextStruct_.wl_shm)) {
            return nullptr;
        }
    }

    {
        LOCK(windowListMutex);
        m_windowList.push_back(newSubInfo);
    }

    parentInnerWindowInfo->childIDList.push_back(subid);
    WMLOG_E("LayerControllerClient::CreateSubWindow END");
    return GetInnerWindowInfoFromId(subid);
}

void LayerControllerClient::RegistPointerButtonCb(int id, funcPointerButton cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerButtonCb = cb;
}

void LayerControllerClient::RegistPointerEnterCb(int id, funcPointerEnter cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerEnterCb = cb;
}

void LayerControllerClient::RegistPointerLeaveCb(int id, funcPointerLeave cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerLeaveCb = cb;
}

void LayerControllerClient::RegistPointerMotionCb(int id, funcPointerMotion cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerMotionCb = cb;
}

void LayerControllerClient::RegistPointerAxisCb(int id, funcPointerAxis cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerAxisCb = cb;
}

void LayerControllerClient::RegistPointerAxisStopCb(int id, funcPointerAxisStop cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerAxisStopCb = cb;
}

void LayerControllerClient::RegistPointerAxisSourceCb(int id, funcPointerAxisSource cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerAxisSourceCb = cb;
}

void LayerControllerClient::RegistPointerAxisDiscreteCb(int id, funcPointerAxisDiscrete cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->pointerAxisDiscreteCb = cb;
}

void LayerControllerClient::RegistTouchUpCb(int id, funcTouchUp cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->touchUpCb = cb;
}

void LayerControllerClient::RegistTouchDownCb(int id, funcTouchDown cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->touchDownCb = cb;
}

void LayerControllerClient::RegistTouchEmotionCb(int id, funcTouchEmotion cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->touchEmotionCb = cb;
}

void LayerControllerClient::RegistTouchFrameCb(int id, funcTouchFrame cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->touchFrameCb = cb;
}

void LayerControllerClient::RegistTouchCancelCb(int id, funcTouchCancel cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->touchCancelCb = cb;
}

void LayerControllerClient::RegistTouchShapeCb(int id, funcTouchShape cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->touchShapeCb = cb;
}

void LayerControllerClient::RegistTouchOrientationCb(int id, funcTouchOrientation cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    GET_WINDOWINFO_VOID(windowInfo, id);
    windowInfo->touchOrientationCb = cb;
}

void LayerControllerClient::RegistOnTouchCb(int id, funcOnTouch cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    if (cb) {
        WMLOG_I("LayerControllerClient::RegistOnTouchCb OK");
        GET_WINDOWINFO_VOID(windowInfo, id);
        windowInfo->onTouchCb = cb;
    }
}

void LayerControllerClient::RegistOnKeyCb(int id, funcOnKey cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::RegistOnKeyCb start");
    if (cb) {
        WMLOG_I("LayerControllerClient::RegistOnKeyCb OK");
        GET_WINDOWINFO_VOID(windowInfo, id);
        windowInfo->keyboardKeyCb = cb;
    }
}

void LayerControllerClient::RegistWindowInfoChangeCb(int id, funcWindowInfoChange cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s begin",  __func__);
    if (cb) {
        WMLOG_I("LayerControllerClient::RegistWindowInfoChangeCb OK");
        GET_WINDOWINFO_VOID(windowInfo, id);
        windowInfo->windowInfoChangeCb = cb;
    }
}

void LayerControllerClient::RegistOnWindowCreateCb(int32_t id, void(* cb)(uint32_t pid))
{
    LOCK(mutex);
    GET_WINDOWINFO_VOID(info, id);
    info->onWindowCreateCb = cb;
}

void LayerControllerClient::SendWindowCreate(uint32_t pid)
{
    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        if (it->onWindowCreateCb) {
            it->onWindowCreateCb(pid);
        }
    }
}

void LayerControllerClient::SetSubSurfaceSize(int32_t id, int32_t width, int32_t height)
{
    WMLOG_E("%{public}s start id(%{public}d) width(%{public}d) height(%{public}d)",
        "LayerControllerClient::SetSubSurfaceSize", id, width, height);

    GET_WINDOWINFO_VOID(windowInfo, id);
    if (windowInfo->subwidow == true && windowInfo->viewport) {
        windowInfo->viewport = wp_viewporter_get_viewport(wlContextStruct_.viewporter, windowInfo->wlSurface);
        if (windowInfo->viewport) {
            wp_viewport_set_source(windowInfo->viewport, windowInfo->pos_x, windowInfo->pos_y, width, height);
        }
        wl_surface_commit(windowInfo->wlSurface);
    }
}

void LayerControllerClient::DestroyWindow(int32_t id)
{
    LOCK(mutex);

    std::queue<uint32_t> q;
    q.push(id);
    while (!q.empty()) {
        uint32_t id = q.front();
        q.pop();

        InnerWindowInfo *info = GetInnerWindowInfoFromId(id);
        if (info) {
            for (auto jt = info->childIDList.begin(); jt != info->childIDList.end(); jt++) {
                q.push(*jt);
            }
        }

        RemoveInnerWindowInfo(id);
    }

    CleanBufferInfo();
    ilm_commitChanges();
    return;
}

void LayerControllerClient::Move(int32_t id, int32_t x, int32_t y)
{
    LOCK(mutex);

    GET_WINDOWINFO_VOID(wininfo, id);
    wininfo->pos_x = x;
    wininfo->pos_y = y;
    wininfo->windowconfig.pos_x = x;
    wininfo->windowconfig.pos_y = y;

    if (wininfo->subwidow) {
        wl_subsurface_set_position(wininfo->wlSubSurface, x, y);
    } else {
        WMLOG_I("LayerControllerClient::Move id(%{public}d) x(%{public}d) y(%{public}d)", id, x, y);
        ilmErrorTypes callResult = ilm_surfaceSetDestinationRectangle(id, x, y, wininfo->width, wininfo->height);
        if (callResult != ILM_SUCCESS) {
            WMLOG_I("Error: LayerControllerClient::Move ilm_layerSetDestinationRectangle - %{public}s. Exiting.",
                    ILM_ERROR_STRING(callResult));
            return;
        }

        ilm_commitChanges();
    }

    struct WindowInfo info = {
        .width = wininfo->width,
        .height = wininfo->height,
        .pos_x = wininfo->pos_x,
        .pos_y = wininfo->pos_y
    };

    if (wininfo->windowInfoChangeCb) {
        wininfo->windowInfoChangeCb(info);
    }
}

void LayerControllerClient::ReSize(int32_t id, int32_t width, int32_t height)
{
    WMLOG_I("LayerControllerClient::ReSize id(%{public}d) width(%{public}d) width(%{public}d)", id, width, height);
    LOCK(mutex);

    GET_WINDOWINFO_VOID(wininfo, id);
    wininfo->windowconfig.width = width;
    wininfo->windowconfig.height = height;
    wininfo->width = width;
    wininfo->height = height;

    if (wininfo->subwidow) {
        SetSubSurfaceSize(id, width, height);
    } else {
        ilmErrorTypes callResult = ilm_surfaceSetDestinationRectangle(id,
            wininfo->pos_x, wininfo->pos_y, width, height);
        if (ILM_SUCCESS != callResult) {
            WMLOG_I("Error: LayerControllerClient::Move ilm_surfaceSetDestinationRectangle- %{public}s. Exiting.",
                    ILM_ERROR_STRING(callResult));
            return;
        }

        ilm_commitChanges();
    }

    struct WindowInfo info = {
        .width = wininfo->width,
        .height = wininfo->height,
        .pos_x = wininfo->pos_x,
        .pos_y = wininfo->pos_y
    };

    if (wininfo->windowInfoChangeCb) {
        wininfo->windowInfoChangeCb(info);
    }
}

void LayerControllerClient::Show(int32_t id)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::Show id=%{public}d", id);
    t_ilm_surface surfaceid = (t_ilm_surface)id;
    ilm_surfaceSetVisibility(surfaceid, ILM_TRUE);
    ilm_commitChanges();
    return;
}

void LayerControllerClient::Hide(int32_t id)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::Hide id=%{public}d", id);
    t_ilm_surface surfaceid = (t_ilm_surface)id;
    ilm_surfaceSetVisibility(surfaceid, ILM_FALSE);
    ilm_commitChanges();
    return;
}

void LayerControllerClient::Rotate(int32_t id, int32_t type)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::Rotate id=%{public}d type=%{public}d start", id, type);
    GET_WINDOWINFO_VOID(wininfo, id);
    wl_surface_set_buffer_transform(wininfo->wlSurface, type);
    wl_surface_commit(wininfo->wlSurface);
}

int32_t LayerControllerClient::GetMaxWidth()
{
    WMLOG_I("LayerControllerClient::GetMaxWidth m_screenWidth=%{public}d", m_screenWidth);
    return m_screenWidth;
}

int32_t LayerControllerClient::GetMaxHeight()
{
    WMLOG_I("LayerControllerClient::GetMaxHeight m_screenHeight=%{public}d", m_screenHeight);
    return m_screenHeight;
}

namespace {
void ConfigIlmSurface(t_ilm_uint id)
{
    GET_WINDOWINFO_VOID(info, id);
    auto& config = info->windowconfig;
    WMLOGFI("id: %{public}d, layer: %{public}d (%{public}d, %{public}d), %{public}d x %{public}d",
            id, info->layerid, config.pos_x, config.pos_y, config.width, config.height);

    ilm_surfaceSetDestinationRectangle(id, config.pos_x, config.pos_y, config.width, config.height);
    ilm_surfaceSetSourceRectangle(id, 0, 0, config.width, config.height);
    ilm_surfaceSetVisibility(id, ILM_TRUE);
    ilm_layerAddSurface(info->layerid, id);
    ilm_commitChanges();
}

void SurfaceNotificationListener(t_ilm_uint id, struct ilmSurfaceProperties* sp, t_ilm_notification_mask m)
{
    WMLOGFI("id: %{public}d, mask: %{public}x", id, m);

    if ((unsigned)m & ILM_NOTIFICATION_CONFIGURED) {
        ConfigIlmSurface(id);
    }
}

void NotificationCallback(ilmObjectType object, t_ilm_uint id, t_ilm_bool created, void *userData)
{
    LayerControllerClient::GetInstance()->SendWindowCreate(id / WINDOW_ID_LIMIT);

    if (object == ILM_SURFACE && id / WINDOW_ID_LIMIT != getpid()) {
        return;
    }

    WMLOGFI("id: %{public}d, object: %{public}d, created: %{public}d", id, object, created);
    if (object == ILM_SURFACE && created == ILM_TRUE) {
        ilm_surfaceAddNotification(id, SurfaceNotificationListener);
        ilm_commitChanges();

        struct ilmSurfaceProperties sp = { .origSourceWidth = 0, .origSourceHeight = 0 };
        ilm_getPropertiesOfSurface(id, &sp);
        if (sp.origSourceWidth != 0 && sp.origSourceHeight != 0) {
            ConfigIlmSurface(id);
        }
    }
}
} // namespace

void LayerControllerClient::wm_register_notification(WindowConfig& config)
{
    ilmErrorTypes callResult = ilm_registerNotification(NotificationCallback, nullptr);
    if (callResult != ILM_SUCCESS) {
        WMLOG_E("Error: LayerControllerClient::wm_register_notification - %{public}s",
                ILM_ERROR_STRING(callResult));
        return;
    }
    WMLOG_I("Success: LayerControllerClient::wm_register_notification");
    return;
}

void ProcessWindowInfo(InnerWindowInfo& info)
{
    int layerid = WINDOW_LAYER_DEFINE_NORMAL_ID;
    layerid += info.windowconfig.type * LAYER_ID_TYPE_OFSSET;
    if (info.subwidow) {
        if (info.viewport) {
            wp_viewport_destroy(info.viewport);
        }

        if (info.wlSubSurface) {
            wl_subsurface_destroy(info.wlSubSurface);
        }

        if (info.windowconfig.type == WINDOW_TYPE_VIDEO) {
            if (info.wl_buffer) {
                wl_buffer_destroy(info.wl_buffer);
            }

            if (info.shm_data) {
                munmap(info.shm_data, info.width * STRIDE_NUM * info.height);
            }

            VideoWindow::DestroyLayer(info.voLayerId);
        }
    } else {
        ilm_layerRemoveSurface(layerid, info.windowid);
        ilm_commitChanges();

        if (info.iviSurface) {
            ivi_surface_destroy(info.iviSurface);
        }
    }

    if (info.wlSurface) {
        wl_surface_destroy(info.wlSurface);
    }

    if (info.surface) {
        std::lock_guard<std::mutex> lock(g_surfaceWptrMutex);
        info.surface->CleanCache();
        info.surface = nullptr;
    }
}

void LayerControllerClient::RemoveInnerWindowInfo(uint32_t id)
{
    LOCK(windowListMutex);

    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        if (it->windowid == id) {
            ProcessWindowInfo(*it);
            m_windowList.erase(it);
            it--;
            break;
        }
    }
}

InnerWindowInfo* LayerControllerClient::GetInnerWindowInfoFromId(uint32_t windowid)
{
    LOCK(windowListMutex);
    for (auto& info : m_windowList) {
        if (info.windowid == windowid) {
            return &info;
        }
    }

    return nullptr;
}

BufferInfo* LayerControllerClient::GetBufferInfo(sptr<SurfaceBuffer>& buffer)
{
    CleanBufferInfo();
    for (auto& info : bufferInfos_) {
        if (info.buffer_ == buffer) {
            return &info;
        }
    }

    bufferInfos_.emplace_back();
    return &bufferInfos_.back();
}

void LayerControllerClient::CleanBufferInfo()
{
    for (auto it = bufferInfos_.begin(); it != bufferInfos_.end(); it++) {
        sptr<SurfaceBuffer> buffer;
        if (it->buffer_ != nullptr) {
            buffer = it->buffer_.promote();
        }

        if (it->buffer_ == nullptr || buffer == nullptr || buffer->GetVirAddr() == nullptr) {
            bufferInfos_.erase(it);
            it--;
        }
    }
}

SurfaceListener::SurfaceListener(sptr<Surface>& surface, uint32_t windowid)
{
    WMLOG_I("DEBUG SurfaceListener");
    surface_ = surface;
    windowid_ = windowid;
}

SurfaceListener::~SurfaceListener()
{
    WMLOG_I("DEBUG ~SurfaceListener");
    surface_ = nullptr;
}

void SurfaceListener::OnBufferAvailable()
{
    WMLOG_I("OnBufferAvailable");
    auto surface = surface_.promote();
    if (surface) {
        LayerControllerClient::GetInstance()->CreateWlBuffer(surface, windowid_);
    } else {
        WMLOG_E("surface.promote failed");
    }
}

BufferInfo::BufferInfo()
{
    wlBuffer_ = nullptr;
}

BufferInfo::~BufferInfo()
{
    if (wlBuffer_) {
        WMLOG_I("wl_buffer destroy");
        wl_buffer_destroy(wlBuffer_);
    }
}

void BufferInfo::AsyncInit()
{
    created_ = false;
}

void BufferInfo::Await()
{
    std::unique_lock<std::mutex> lck(createdMutex_);
    while (!created_) {
        createdVariable_.wait(lck);
    }
}

void BufferInfo::Async(std::function<void(BufferInfo*)> mutexFunc)
{
    std::unique_lock<std::mutex> lck(createdMutex_);
    mutexFunc(this);
    created_ = true;
    createdVariable_.notify_all();
}
}
