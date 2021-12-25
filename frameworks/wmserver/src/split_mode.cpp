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

#include "split_mode.h"

#include <map>

#include <scoped_bytrace.h>
#include <ianimation_service.h>
#include <ivi-layout-private.h>
#include <window_manager_type.h>
#include <wms-server-protocol.h>

#include "wmserver.h"
#include "layout_controller.h"

#include <trace.h>
DEFINE_LOG_LABEL("SplitMode");

using namespace OHOS;

int32_t g_x = 0;
int32_t g_y = 0;
bool g_reinit = false;
int32_t g_3selectAlign = 0;

constexpr double lineHeight = 0.1;
constexpr double lineHeightHalf = lineHeight / 2;
constexpr double exitingPosition = 0.2;

void GetSplitModeShowArea(int32_t &x, int32_t &y, int32_t &width, int32_t &height)
{
    struct layout layout = {};
    LayoutControllerCalcWindowDefaultLayout(WINDOW_TYPE_NORMAL, WINDOW_MODE_UNSET, NULL, &layout);
    x = layout.x;
    y = layout.y;
    width = layout.w;
    height = layout.h;
}

void ChangeSplitMode(struct WindowSurface *ws, SplitStatus status)
{
    ScopedBytrace trace(__func__);
    wms_send_split_mode_change(ws->controller->pWlResource, status);
    LOG_INFO("%d", status);
}

void ChangeWindowPosition(struct WindowSurface *ws, int32_t x, int32_t y)
{
    ScopedBytrace trace(__func__);
    SetWindowPosition(ws, x, y);
    LOG_INFO("%d move to (%d, %d)", ws->surfaceId, x, y);
}

void ChangeWindowSize(struct WindowSurface *ws, int32_t w, int32_t h)
{
    ScopedBytrace trace(__func__);
    if (w < 0) {
        w = 0;
    }
    if (h < 0) {
        h = 0;
    }
    wms_send_window_size_change(ws->controller->pWlResource, w, h);
    SetWindowSize(ws, w, h);
    LOG_INFO("%d resize to %dx%d", ws->surfaceId, w, h);
}

uint32_t GetSplitMode()
{
    struct WmsContext *ctx = GetWmsInstance();
    return ctx->splitMode;
}

void SetSplitMode(uint32_t type)
{
    ScopedBytrace trace(__func__);
    struct WmsContext *ctx = GetWmsInstance();
    ctx->splitMode = type;
    LOG_INFO("splitMode -> %d", ctx->splitMode);
}

struct WindowSurface *GetWindow(std::function<bool(struct WindowSurface *ws)> condition)
{
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *ws = NULL;
    wl_list_for_each(ws, &ctx->wlListWindow, link) {
        if (condition(ws)) {
            return ws;
        }
    }
    return nullptr;
}

void ForeachWindow(std::function<bool(struct WindowSurface *ws)> condition,
    std::function<void(struct WindowSurface *ws)> action)
{
    ScopedBytrace trace(__func__);
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *ws = NULL;
    wl_list_for_each(ws, &ctx->wlListWindow, link) {
        if (condition(ws)) {
            action(ws);
        }
    }
}

struct WindowSurface *GetSplitedWindow()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->isSplited;
    };
    return GetWindow(condition);
}

struct WindowSurface *GetLineWindow()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->type == WINDOW_TYPE_SPLIT_LINE;
    };
    return GetWindow(condition);
}

bool GetSplitedWindows(struct WindowSurface *&top, struct WindowSurface *&bottom)
{
    top = GetSplitedWindow();
    if (top == nullptr) {
        LOG_ERROR("no splitWindow");
        return false;
    }

    auto condition = [top](struct WindowSurface *ws) {
        return ws->isSplited && ws != top;
    };
    bottom = GetWindow(condition);
    if (bottom == nullptr) {
        LOG_ERROR("no two splitWindow");
        return false;
    }

    if (top->y > bottom->y) {
        auto tmp = top;
        top = bottom;
        bottom = tmp;
    }
    return true;
}

bool To0Null()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->isSplited;
    };
    auto action = [](struct WindowSurface *ws) {
        ws->isSplited = false;
        int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
        GetSplitModeShowArea(defX, defY, defWidth, defHeight);
        ChangeWindowPosition(ws, defX, defY);
        ChangeWindowSize(ws, defWidth, defHeight);
        ChangeSplitMode(ws, SPLIT_STATUS_RETAIN);
        return false;
    };
    ForeachWindow(condition, action);

    auto lineWindow = GetLineWindow();
    if (lineWindow) {
        ChangeSplitMode(lineWindow, SPLIT_STATUS_DESTROY);
    }
    AddSurfaceDestroyListener(nullptr);
    return true;
}

bool To1Unenable()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->type == WINDOW_TYPE_NORMAL;
    };
    auto ws = GetWindow(condition);
    ws->isSplited = true;
    ChangeSplitMode(ws, SPLIT_STATUS_VAGUE);
    return true;
}

void OnSwitchTop(struct WindowSurface *ws)
{
    if (ws->type != WINDOW_TYPE_SPLIT_LINE) {
        return;
    }

    ws = GetSplitedWindow();
    if (ws == nullptr) {
        LOG_ERROR("no splitWindow");
        return;
    }

    struct WmsContext *ctx = GetWmsInstance();
    ctx->pLayoutInterface->surface_change_top(ws->layoutSurface);
    ctx->pLayoutInterface->commit_changes();
    AddSetWindowTopListener(nullptr);
}

void OnSurfaceDestroy(struct WindowSurface *ws)
{
    LOG_INFO("OnSurfaceDestroy: %d", ws->surfaceId);
    if (ws->isSplited == false) {
        return;
    }
    AddSurfaceDestroyListener(nullptr);
    SetSplitMode(SPLIT_MODE_NULL);
    LOG_INFO("exit split mode");

    auto lineWindow = GetLineWindow();
    if (lineWindow) {
        ChangeSplitMode(lineWindow, SPLIT_STATUS_DESTROY);
    }

    auto condition = [other = ws](struct WindowSurface *ws) {
        return ws->isSplited && ws != other;
    };
    auto ws2 = GetWindow(condition);
    if (ws2 == nullptr) {
        return;
    }

    ws2->isSplited = false;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetSplitModeShowArea(defX, defY, defWidth, defHeight);
    ChangeWindowPosition(ws2, defX, defY);
    ChangeWindowSize(ws2, defWidth, defHeight);
    ChangeSplitMode(ws2, SPLIT_STATUS_RETAIN);

    auto ctx = GetWmsInstance();
    ctx->pLayoutInterface->surface_change_top(ws->layoutSurface);
    ctx->pLayoutInterface->commit_changes();
}

bool To2Single(struct WindowSurface *ws)
{
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetSplitModeShowArea(defX, defY, defWidth, defHeight);

    // vectical-align: middle, height: 1 - lineHeight
    ChangeWindowPosition(ws, defX, defY + defHeight * lineHeightHalf);
    ChangeWindowSize(ws, defWidth, defHeight * (1 - lineHeight));
    ws->isSplited = true;
    ChangeSplitMode(ws, SPLIT_STATUS_VAGUE);
    return true;
}

bool To2Single0()
{
    AddSurfaceDestroyListener(OnSurfaceDestroy);
    auto condition = [](struct WindowSurface *ws) {
        return ws->type == WINDOW_TYPE_NORMAL;
    };
    struct WindowSurface *ws = GetWindow(condition);
    if (ws == nullptr) {
        LOG_ERROR("ws is nullptr, cannot found WINDOW_TYPE_NORMAL window");
        return false;
    }

    AddSetWindowTopListener(OnSwitchTop);
    if (IAnimationService::Get() == nullptr) {
        LOG_ERROR("IAnimationService::Get is nullptr");
        return false;
    }

    auto ret = IAnimationService::Get()->SplitModeCreateBackground();
    if (ret) {
        LOG_ERROR("SplitModeCreateBackground failed: %s", GSErrorStr(ret).c_str());
        if (ret == GSERROR_BINDER) {
            g_reinit = true;
        }
        return false;
    }

    return To2Single(ws);
}

bool To2Single3()
{
    struct WindowSurface *ws = nullptr;
    ws = GetSplitedWindow();
    if (ws == nullptr) {
        LOG_ERROR("no split window");
        return false;
    }
    return To2Single(ws);
}

bool To3Select2()
{
    auto ws = GetSplitedWindow();
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetSplitModeShowArea(defX, defY, defWidth, defHeight);

    LOG_INFO("Select g_y: %d %p", g_y, ws);
    if (ws == nullptr) {
        LOG_ERROR("ws is nullptr");
        return false;
    }

    int32_t height = defHeight * (1 - lineHeight) / 0x2;
    if (g_y > defY + defHeight / 0x2) {
        // select bottom, ws move to top
        g_3selectAlign = 0;
        LOG_INFO("Select bottom");
        // vectical-align: top
        ChangeWindowPosition(ws, defX, defY);
        ChangeWindowSize(ws, defWidth, height);
        auto ctx = GetWmsInstance();
        ctx->pLayoutInterface->surface_change_top(ws->layoutSurface);
        ctx->pLayoutInterface->commit_changes();
    } else {
        // select bottom, ws move to bottom
        g_3selectAlign = 1;
        LOG_INFO("Select top");
        // vectical-align: bottom
        ChangeWindowPosition(ws, defX, defY + defHeight - height);
        ChangeWindowSize(ws, defWidth, height);
    }

    ChangeSplitMode(ws, SPLIT_STATUS_VAGUE);
    return true;
}

bool To3Select3()
{
    auto ws = GetSplitedWindow();
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetSplitModeShowArea(defX, defY, defWidth, defHeight);

    LOG_INFO("Select g_y: %d %p", g_y, ws);
    if (ws == nullptr) {
        LOG_ERROR("ws is nullptr");
        return false;
    }

    int32_t height = defHeight * (1 - lineHeight) / 0x2;
    if (g_y > defY + defHeight / 0x2 && g_3selectAlign != 0) {
        // select bottom, ws move to top
        g_3selectAlign = 0;
        LOG_INFO("Select bottom!");
        // vectical-align: top
        ChangeWindowPosition(ws, defX, defY);
        ChangeWindowSize(ws, defWidth, height);
    } else if (g_y <= defY + defHeight / 0x2 && g_3selectAlign != 1) {
        // select bottom, ws move to bottom
        g_3selectAlign = 1;
        LOG_INFO("Select top!");
        // vectical-align: bottom
        ChangeWindowPosition(ws, defX, defY + defHeight - height);
        ChangeWindowSize(ws, defWidth, height);
    } else {
        return true;
    }

    auto ctx = GetWmsInstance();
    ctx->pLayoutInterface->surface_change_top(ws->layoutSurface);
    ctx->pLayoutInterface->commit_changes();
    return true;
}

bool To4Confirm()
{
    if (IAnimationService::Get() == nullptr) {
        LOG_ERROR("IAnimationService::Get is nullptr");
        return false;
    }
    auto ret = IAnimationService::Get()->SplitModeCreateMiddleLine();
    if (ret) {
        LOG_ERROR("SplitModeCreateMiddleLine failed: %d", ret);
        if (ret == GSERROR_BINDER) {
            g_reinit = true;
        }
        return false;
    }

    auto condition = [](struct WindowSurface *ws) {
        return ws->type == WINDOW_TYPE_NORMAL && ws->isSplited == false;
    };
    auto win1 = GetWindow(condition);
    if (win1 == nullptr) {
        LOG_ERROR("win1 is nullptr");
        return false;
    }

    auto win2 = GetSplitedWindow();
    if (win2 == nullptr) {
        LOG_ERROR("win2 is nullptr");
        return false;
    }

    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetSplitModeShowArea(defX, defY, defWidth, defHeight);
    int32_t y1 = 0, y2 = 0, winHeight = defHeight * (1 - lineHeight) / 0x2;
    if (win2->y <= defY + defHeight / 0x2) {
        y1 = defY + defHeight - winHeight;
        y2 = defY;
    } else {
        y1 = defY;
        y2 = defY + defHeight - winHeight;
    }

    win1->isSplited = true;
    ChangeWindowPosition(win1, defX, y1);
    ChangeWindowSize(win1, defWidth, winHeight);
    ChangeWindowPosition(win2, defX, y2);
    ChangeWindowSize(win2, defWidth, winHeight);

    ChangeSplitMode(win1, SPLIT_STATUS_CLEAR);
    ChangeSplitMode(win2, SPLIT_STATUS_CLEAR);
    return true;
}

bool To5TouchDown()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->isSplited;
    };
    auto action = [](struct WindowSurface *ws) {
        ChangeSplitMode(ws, SPLIT_STATUS_VAGUE);
    };
    ForeachWindow(condition, action);
    return true;
}

bool To6TouchMove()
{
    struct WindowSurface *topWindow;
    struct WindowSurface *bottomWindow;
    if (!GetSplitedWindows(topWindow, bottomWindow)) {
        LOG_ERROR("no two splitWindow");
        return false;
    }

    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetSplitModeShowArea(defX, defY, defWidth, defHeight);

    LOG_INFO("g_y: %d", g_y);
    ChangeWindowSize(topWindow, defWidth, g_y - defY - defHeight * lineHeightHalf);
    ChangeSplitMode(topWindow, SPLIT_STATUS_VAGUE);

    ChangeWindowPosition(bottomWindow, defX, g_y + defHeight * lineHeightHalf);
    ChangeWindowSize(bottomWindow, defWidth, defY + defHeight - g_y - defHeight * lineHeightHalf);
    ChangeSplitMode(bottomWindow, SPLIT_STATUS_VAGUE);
    return true;
}

bool To7TouchUp5()
{
    struct WindowSurface *topWindow;
    struct WindowSurface *bottomWindow;
    if (!GetSplitedWindows(topWindow, bottomWindow)) {
        LOG_ERROR("no two splitWindow");
        return false;
    }

    ChangeSplitMode(topWindow, SPLIT_STATUS_CLEAR);
    ChangeSplitMode(bottomWindow, SPLIT_STATUS_CLEAR);
    return true;
}

bool To7TouchUp6()
{
    struct WindowSurface *topWindow;
    struct WindowSurface *bottomWindow;
    if (!GetSplitedWindows(topWindow, bottomWindow)) {
        LOG_ERROR("no two splitWindow");
        return false;
    }

    int32_t x = 0, y = 0, w = 0, h = 0;
    GetSplitModeShowArea(x, y, w, h);

    int32_t diff = g_y - y;
    LOG_INFO("diff: %d", diff);
    if (exitingPosition * h <= diff && diff <= h * (1 - exitingPosition)) {
        ChangeSplitMode(topWindow, SPLIT_STATUS_CLEAR);
        ChangeSplitMode(bottomWindow, SPLIT_STATUS_CLEAR);
        return true;
    }

    auto longWindow = topWindow;
    auto shortWindow = bottomWindow;
    if (bottomWindow->height > topWindow->height) {
        longWindow = bottomWindow;
        shortWindow = topWindow;
    }
    ChangeSplitMode(shortWindow, SPLIT_STATUS_DESTROY);
    ChangeSplitMode(GetLineWindow(), SPLIT_STATUS_DESTROY);

    longWindow->isSplited = false;
    ChangeWindowPosition(longWindow, x, y);
    ChangeWindowSize(longWindow, w, h);
    ChangeSplitMode(longWindow, SPLIT_STATUS_RETAIN);

    struct WmsContext *ctx = GetWmsInstance();
    SetSplitMode(SPLIT_MODE_NULL);
    LOG_INFO("exit split mode");
    ctx->pLayoutInterface->surface_change_top(longWindow->layoutSurface);
    ctx->pLayoutInterface->commit_changes();
    return true;
}

bool Ignore()
{
    return true;
}

/* split mode state machine
 * /-\       /-*       /-*
 * |7| ----> |0| <---> |1|
 * \-/       \-/       \-/
 *    ^
 *  ^  \      ^
 *  |   \     |
 *  |    \    v
 *  |    |
 *  |    |   /-*       /-*
 *  |    |   |2| <---> |3|
 *  |     \  \-/       \-/
 *  |      \
 *  |       \           |
 *  |        \          |
 *  |         \         v
 *            v
 * /-*       /-\       /-\
 * |6| <---- |5| <---- |4|
 * \-/       \-/       \-/
 */
bool(* stateMachine[SPLIT_MODE_MAX][SPLIT_MODE_MAX])() = {
/* From 0NULL         1UNENABLE     2SINGLE       3SELECT       4CONFIRM      5TOUCHDOWN    6TOUCHMOVE    7TOUCHUP   */
    { Ignore,       To0Null,      To0Null,      nullptr,      nullptr,      nullptr,      nullptr,      To0Null      },
    { To1Unenable,  Ignore,       nullptr,      nullptr,      nullptr,      nullptr,      nullptr,      nullptr      },
    { To2Single0,   nullptr,      Ignore,       To2Single3,   nullptr,      nullptr,      nullptr,      nullptr      },
    { nullptr,      nullptr,      To3Select2,   To3Select3,   nullptr,      nullptr,      nullptr,      nullptr      },
    { nullptr,      nullptr,      To4Confirm,   To4Confirm,   nullptr,      nullptr,      nullptr,      nullptr      },
    { nullptr,      nullptr,      nullptr,      nullptr,      To5TouchDown, nullptr,      nullptr,      To5TouchDown },
    { nullptr,      nullptr,      nullptr,      nullptr,      nullptr,      To6TouchMove, To6TouchMove, nullptr      },
    { nullptr,      nullptr,      nullptr,      nullptr,      nullptr,      To7TouchUp5,  To7TouchUp6,  nullptr      },
};

void ControllerSetSplitMode(struct wl_client *client,
                            struct wl_resource *resource,
                            uint32_t type, int32_t x, int32_t y)
{
    ScopedBytrace trace(__func__);
    LOG_SCOPE();
    LOG_INFO("type: %d", type);
    if (!(type >= 0 && type < SPLIT_MODE_MAX)) {
        LOG_ERROR("invalid");
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    if (IAnimationService::Init(g_reinit) == GSERROR_OK) {
        g_reinit = false;
    }

    auto currentType = GetSplitMode();
    LOG_INFO("from %u to %u", currentType, type);
    auto func = stateMachine[type][currentType];
    if (func == nullptr) {
        LOG_ERROR("cannot");
        wms_send_reply_error(resource, WMS_ERROR_INVALID_OPERATION);
        return;
    }

    if (type == SPLIT_MODE_SELECT || type == SPLIT_MODE_DIVIDER_TOUCH_MOVE) {
        g_x = x;
        g_y = y;
    }

    LOG_ENTERS("stateMachine");
    ScopedBytrace trace2("stateMachine");
    auto ret = func();
    trace2.End();
    LOG_EXITS("stateMachine");
    if (ret) {
        LOG_INFO("success");
        SetSplitMode(type);
        wms_send_reply_error(resource, WMS_ERROR_OK);
    } else {
        LOG_ERROR("failure");
        wms_send_reply_error(resource, WMS_ERROR_API_FAILED);
    }
    return;
}
