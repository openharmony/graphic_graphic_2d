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

#ifndef FRAMEWORKS_WMSERVER_SRC_LAYOUT_CONTROLLER_H
#define FRAMEWORKS_WMSERVER_SRC_LAYOUT_CONTROLLER_H

#include "layout_header.h"

#ifdef __cplusplus
#include <filesystem>
#include <functional>
#include <map>
#include <string>

#include <window_manager_type.h>

namespace OHOS::WMServer {
class LayoutController {
public:
    static LayoutController &GetInstance();

    void Init(int32_t width, int32_t height);
    int32_t CalcWindowDefaultLayout(uint32_t type, uint32_t mode, struct Layout &outLayout);

private:
    LayoutController() = default;
    ~LayoutController() = default;

    void InitByDefaultValue();
    void InitByParseSCSS();
    bool CalcNormalRect(struct layout &layout);
    const std::string searchCSSDirectory = "/system/etc/wmlayout.d";

    void ParseSCSS(const std::filesystem::path &file);

    bool init = false;
    int32_t displayWidth = 0;
    int32_t displayHeight = 0;
    std::map<uint32_t, struct Layout> modeLayoutMap[WINDOW_MODE_MAX];
};
} // namespace OHOS::WMServer

// C interface
extern "C" {
#endif

// return errno, 0 is ok
void LayoutControllerInit(int32_t width, int32_t height);
int32_t LayoutControllerCalcWindowDefaultLayout(uint32_t type,
    uint32_t mode, uint32_t *zIndex, struct layout *outLayout);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORKS_WMSERVER_SRC_LAYOUT_CONTROLLER_H
