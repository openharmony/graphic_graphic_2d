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

#include "layer_controller.h"

#include <map>
#include <queue>
#include <unistd.h>

#include <securec.h>
#include <wayland-client.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WindowManagerServer" };
} // namespace

constexpr int TOP_SURFACE_NUM = 1;
LayerController::LayerController() : mutex(), windowIdSets(), m_windowList(), surfaceList()
{
    {
        static bool init = false;
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);

        if (init) {
            WMLOG_I("Error: Second instance of Layer Controller detected");
        }
        init = true;
    }

    // retry to connect
    constexpr int32_t retryTimes = 60;
    struct wl_display *display;
    for (int32_t i = 0; i < retryTimes; i++) {
        display = wl_display_connect(nullptr);
        if (display != nullptr) {
            break;
        } else {
            WMLOG_W("create display failed! (%{public}d/%{public}d)", i + 1, retryTimes);
        }

        constexpr int32_t sleepTimeFactor = 50 * 1000;
        int32_t sleepTime = i * sleepTimeFactor;
        usleep(sleepTime);
    }

    if (display == nullptr) {
        WMLOG_E("create display failed!");
        exit(1);
    }

    auto callResult = ilm_initWithNativedisplay((t_ilm_nativedisplay)display);
    if (callResult != ILM_SUCCESS) {
        WMLOG_I("Error: ilm_initWithNativedisplay - %{public}s.", ILM_ERROR_STRING(callResult));
        exit(callResult);
    } else {
        WMLOG_I("ilm_initWithNativedisplay succes!");
    }

    initScreen();
    initlayer();
    RegisterNotification();
}

LayerController::~LayerController()
{
    cleanupIlm();
}

bool LayerController::cleanupIlm()
{
    // destroy the IVI LayerManagement Client
    ilmErrorTypes callResult = ilm_destroy();
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: ilm_destroy - %{public}s.", ILM_ERROR_STRING(callResult));
    }
    return true;
}

int32_t LayerController::CreateWindow(pid_t pid, const WindowConfig &config)
{
    std::lock_guard<std::mutex> lock(mutex);
    WMLOG_I("LayerController::CreateWindow start sub = %{public}d", config.subwindow);

    if (windowIdCount >= WINDOW_ID_NUM_MAX) {
        WMLOG_E("CreateWindow failed, window id count >= %{public}d", WINDOW_ID_NUM_MAX);
        return 0;
    }

    if (windowIdSets.find(pid) == windowIdSets.end()) {
        windowIdSets[pid] = std::bitset<WINDOW_ID_LIMIT>(0);
    }

    auto& windowIdSet = windowIdSets[pid];
    if (windowIdSet.all()) {
        WMLOG_E("CreateWindow failed, window count >= %{public}d", WINDOW_ID_LIMIT);
        return 0;
    }

    int wid = 0;
    for (int i = 0; i < WINDOW_ID_LIMIT; i++) {
        if (windowIdSet.test(i) == 0) {
            windowIdSet.set(i);
            wid = i;
            windowIdCount++;
            break;
        }
    }

    WindowInfo newInfo = {
        .windowId = pid * WINDOW_ID_LIMIT + wid,
        .parentID = config.parentid,
        .windowType = config.type,
        .isSubWindow = config.subwindow,
    };

    if (config.subwindow) {
        WMLOG_I("LayerController::CreateWindow start parentid = %{public}d", config.parentid);
        WindowInfo *parentInfo = WindowInfoFromId(config.parentid);
        if (parentInfo) {
            parentInfo->childIDList.push_back(newInfo.windowId);
        }
    }

    WMLOG_I("LayerController::CreateWindow start 111");
    m_windowList.push_back(newInfo);
    surfaceList.push_back(newInfo.windowId);

    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        WMLOG_E("WINDOWINFOLIST:WindID(%{public}d)", it->windowId);
        WMLOG_E("WINDOWINFOLIST:parentID(%{public}d)", it->parentID);
        WMLOG_E("WINDOWINFOLIST:isSubWindow(%{public}d)", it->isSubWindow);
        WMLOG_E("WINDOWINFOLIST:windowType(%{public}d)", it->windowType);
        for (auto jt = it->childIDList.begin(); jt != it->childIDList.end(); jt++) {
            WMLOG_E("WINDOWINFOLIST:childID(%{public}d)", *jt);
        }
    }
    WMLOG_I("LayerController::CreateWindow end %{public}d", newInfo.windowId);
    return newInfo.windowId;
}

namespace {
void SetFocus(uint32_t id, std::vector<uint32_t> windows, int flag)
{
    auto renderOrderAll = std::make_unique<t_ilm_surface[]>(windows.size());
    for (uint32_t i = 0; i < windows.size(); i++) {
        WMLOG_I("windows i=%{public}d id=%{public}d", i, windows[i]);
        renderOrderAll[i] = windows[i];
    }
    ilm_setInputFocus(renderOrderAll.get(), windows.size(), flag, ILM_FALSE);
    ilm_setInputFocus(&id, TOP_SURFACE_NUM, flag, ILM_TRUE);
}
}

void LayerController::ChangeWindowTop(uint32_t id)
{
    std::lock_guard<std::mutex> lock(mutex);
    WMLOG_I("LayerController::ChangeWindowTop id=%{public}d start", id);
    WindowInfo *topwininfo = WindowInfoFromId(id);
    if (topwininfo == nullptr) {
        WMLOG_I("LayerController::ChangeWindowTop id=%{public}d is NOT exit", id);
        return;
    }

    std::list<uint32_t> sameTypeWinList;
    sameTypeWinList.push_back(id);
    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        if (it->windowType == topwininfo->windowType && it->windowId != id) {
            sameTypeWinList.push_back(it->windowId);
        }
    }
    const int renderSize = sameTypeWinList.size();

    auto renderOrder = std::make_unique<t_ilm_surface[]>(renderSize);
    int i = 0;
    for (auto it = sameTypeWinList.begin(); it != sameTypeWinList.end(); it++, i++) {
        WMLOG_I("LayerController::ChangeWindowTop i=%{public}d id=%{public}d", i, *it);
        renderOrder[i] = *it;
    }

    ilm_surfaceSetVisibility((t_ilm_surface)id, ILM_TRUE);
    WMLOG_I("LayerController::ChangeWindowTop size=%{public}u ", renderSize);
    for (uint32_t i = 0; i < renderSize; i++) {
        WMLOG_I("LayerController::ChangeWindowTop renderOrder i=%{public}d id=%{public}d", i, renderOrder[i]);
    }

    int layerId = topwininfo->windowType * LAYER_ID_TYPE_OFSSET + LAYER_ID_APP_TYPE_BASE;
    WMLOG_I("LayerController::ChangeWindowTop layer ID =%{public}d", layerId);
    ilmErrorTypes callResult = ilm_layerSetRenderOrder(layerId, renderOrder.get(), renderSize);
    if (callResult != ILM_SUCCESS) {
        WMLOG_I("Error: ChangeWindowTop (id = %{public}u) - %{public}s", id, ILM_ERROR_STRING(callResult));
        return;
    }

    std::vector<uint32_t> allWinList;
    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        if (it->isSubWindow == false) {
            allWinList.push_back(it->windowId);
        }
    }
    if ((topwininfo->windowType == WINDOW_TYPE_STATUS_BAR) || (topwininfo->windowType == WINDOW_TYPE_NAVI_BAR)) {
        int flag = ILM_INPUT_DEVICE_POINTER | ILM_INPUT_DEVICE_TOUCH;
        SetFocus(id, allWinList, flag);
    } else {
        SetFocus(id, allWinList, ILM_INPUT_DEVICE_ALL);
    }
    ilm_commitChanges();
    WMLOG_I("LayerControllerClient::SwitchTop end");
}

void LayerController::initlayer()
{
    WMLOG_I("LayerControllerClient::initlayer() only one layer");

    ilmErrorTypes callResult;

    t_ilm_layer layerid = WINDOW_LAYER_DEFINE_NORMAL_ID;
    callResult = ilm_layerCreateWithDimension(&layerid, m_screenWidth, m_screenHeight);
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: initlayer normal - %{public}s.", ILM_ERROR_STRING(callResult));
        return;
    }
    WMLOG_I("initlayer normal success!");

    ilm_layerSetVisibility(layerid, ILM_TRUE);

    layerid = WINDOW_LAYER_DEFINE_STATUSBAR_ID;
    callResult = ilm_layerCreateWithDimension(&layerid, m_screenWidth, m_screenHeight);
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: initlayer status bar - %{public}s.", ILM_ERROR_STRING(callResult));
        return;
    }
    WMLOG_I("initlayer status bar success!");

    ilm_layerSetVisibility(layerid, ILM_TRUE);

    layerid = WINDOW_LAYER_DEFINE_NAVIBAR_ID;
    callResult = ilm_layerCreateWithDimension(&layerid, m_screenWidth, m_screenHeight);
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: initlayer navibar - %{public}s.", ILM_ERROR_STRING(callResult));
        return;
    }
    WMLOG_I("initlayer navibar succes!");

    ilm_layerSetVisibility(layerid, ILM_TRUE);

    layerid = WINDOW_LAYER_DEFINE_ALARM_ID;
    callResult = ilm_layerCreateWithDimension(&layerid, m_screenWidth, m_screenHeight);
    if (ILM_SUCCESS != callResult) {
        WMLOG_I("Error: initlayer alarm - %{public}s.", ILM_ERROR_STRING(callResult));
        return;
    }
    WMLOG_I("initlayer alarm succes!");

    ilm_layerSetVisibility(layerid, ILM_TRUE);

    static t_ilm_layer renderOrder[] = {
        WINDOW_LAYER_DEFINE_NORMAL_ID,
        WINDOW_LAYER_DEFINE_STATUSBAR_ID,
        WINDOW_LAYER_DEFINE_NAVIBAR_ID,
        WINDOW_LAYER_DEFINE_ALARM_ID
    };
    ilm_displaySetRenderOrder(0, renderOrder, sizeof(renderOrder) / sizeof(*renderOrder));
    ilm_commitChanges();
}

bool LayerController::initScreen()
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

void LayerController::DestroyWindow(uint32_t windowId)
{
    std::lock_guard<std::mutex> lock(mutex);
    WMLOG_E("LayerController::DestroyWindow start winid=%{public}d", windowId);

    WindowInfo *wininfo = WindowInfoFromId(windowId);
    if (wininfo == nullptr) {
        WMLOG_I("LayerControllerClient::DestroyWindow windowId is not exsit");
        return;
    }

    if (wininfo->isSubWindow) {
        WindowInfo *parentwininfo = WindowInfoFromId(wininfo->parentID);
        if (parentwininfo != nullptr) {
            parentwininfo->childIDList.remove(windowId);
        }
    }

    std::queue<uint32_t> q;
    q.push(windowId);
    while (!q.empty()) {
        uint32_t id = q.front();
        q.pop();

        WindowInfo *info = WindowInfoFromId(id);
        if (info) {
            for (auto jt = info->childIDList.begin(); jt != info->childIDList.end(); jt++) {
                q.push(*jt);
            }
        }

        RemoveWindowInfoById(id);
    }

    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        WMLOG_E("WINDOWINFOLIST:WindID(%{public}d)", it->windowId);
        WMLOG_E("WINDOWINFOLIST:parentID(%{public}d)", it->parentID);
        WMLOG_E("WINDOWINFOLIST:isSubWindow(%{public}d)", it->isSubWindow);
        WMLOG_E("WINDOWINFOLIST:windowType(%{public}d)", it->windowType);
        for (auto jt = it->childIDList.begin(); jt != it->childIDList.end(); jt++) {
            WMLOG_E("WINDOWINFOLIST:childID(%{public}d)", *jt);
        }
    }
    WMLOG_E("LayerController::DestroyWindow Done winid=%{public}d", windowId);
}

LayerController::WindowInfo* LayerController::WindowInfoFromId(uint32_t windowId)
{
    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        if (it->windowId == windowId) {
            return &(*it);
        }
    }
    return nullptr;
}

void LayerController::RemoveWindowInfoById(uint32_t windowId)
{
    m_windowList.remove_if([windowId](WindowInfo const &info) { return info.windowId == windowId; });
    auto it = windowIdSets.find(windowId / WINDOW_ID_LIMIT);
    if (it != windowIdSets.end()) {
        it->second.reset(windowId % WINDOW_ID_LIMIT);
        windowIdCount--;
    }
}

void CallbackFunction(ilmObjectType object, t_ilm_uint id, t_ilm_bool created, void *userData)
{
    WMLOGFI("id: %{public}d, object: %{public}d, created: %{public}d", id, object, created);

    if (object == ILM_SURFACE && created == ILM_FALSE) {
        WMLOGFI("surface destroyed: %{public}d", id);
        LayerController::GetInstance()->DestroyWindow(id);
    }

    if (object == ILM_LAYER && created == ILM_TRUE) {
        struct ilmLayerProperties lp = { .sourceWidth = 0, .sourceHeight = 0 };
        ilm_getPropertiesOfLayer(id, &lp);
        WMLOG_I("callbackFunction windowid %{public}d x=%{public}d y=%{public}dWidth=%{public}d Height=%{public}d",
                id, 0, 0, lp.sourceWidth, lp.sourceHeight);
        ilm_layerSetDestinationRectangle(id, 0, 0, lp.sourceWidth, lp.sourceHeight);
        ilm_layerSetSourceRectangle(id, 0, 0, lp.sourceWidth, lp.sourceHeight);
        ilm_layerSetVisibility(id, ILM_TRUE);
        ilm_commitChanges();
    }
}

void LayerController::RegisterNotification()
{
    WMLOG_I("LayerController::RegisterNotification");
    ilmErrorTypes ret = ilm_registerNotification(CallbackFunction, nullptr);
    WMLOG_I("%{public}s: LayerController::RegisterNotification - %{public}s",
        (ret == ILM_SUCCESS) ? "Success" : "Error", ILM_ERROR_STRING(ret));
    return;
}
} // namespace OHOS
