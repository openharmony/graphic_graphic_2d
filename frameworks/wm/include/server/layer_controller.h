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

#ifndef FRAMEWORKS_WM_INCLUDE_SERVER_LAYER_CONTROLLER_H
#define FRAMEWORKS_WM_INCLUDE_SERVER_LAYER_CONTROLLER_H

#include <bitset>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <ilm_client.h>
#include <ilm_common.h>
#include <ilm_control.h>
#include <ilm_input.h>
#include <ilm_platform.h>
#include <ilm_types.h>
#include <window_manager_common.h>

#include "window_manager_define.h"

namespace OHOS {
class LayerController {
public:
    static LayerController* GetInstance()
    {
        static LayerController layerctrl;
        return &layerctrl;
    }
    int32_t CreateWindow(pid_t pid, const WindowConfig &config);
    void DestroyWindow(uint32_t windowId);
    void ChangeWindowTop(uint32_t windowId);
    void ChangeWindowBottom(uint32_t windowId);

protected:
    bool cleanupIlm();
    void setSurfaceVisible(uint32_t surfaceId);
    void setLayerVisible(uint32_t id);

private:
    LayerController();
    virtual ~LayerController();

    struct WindowInfo {
        uint32_t windowId;
        uint32_t parentID;
        int windowType;
        bool isSubWindow;
        std::list<uint32_t> childIDList;
        bool operator == (const WindowInfo& other) const
        {
            return windowId == other.windowId;
        }
    };

    WindowInfo* WindowInfoFromId(uint32_t windowId);
    void RemoveWindowInfoById(uint32_t windowId);
    void RegisterNotification();
    bool initScreen();
    void initlayer();

    std::mutex mutex;

    uint32_t windowIdCount = 0;
    std::map<int, std::bitset<WINDOW_ID_LIMIT>> windowIdSets;

    std::list<WindowInfo> m_windowList;
    std::vector<uint32_t> surfaceList;
    int32_t m_screenWidth = 0;
    int32_t m_screenHeight = 0;
    uint32_t m_currentLayer = 0;
    uint32_t m_screenId;
};
}

#endif // FRAMEWORKS_WM_INCLUDE_SERVER_LAYER_CONTROLLER_H
