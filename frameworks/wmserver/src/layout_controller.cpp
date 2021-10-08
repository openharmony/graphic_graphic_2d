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

#include "layout_controller.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "layout_header.h"
#include "rects.h"
#include "weston.h"

namespace fs = std::filesystem;

#define LOG_LABEL "wms-wmlayout"

#define LOGD(fmt, ...) weston_log("%{public}s debug %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

#define LOGI(fmt, ...) weston_log("%{public}s info %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

#define LOGE(fmt, ...) weston_log("%{public}s error %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

// C interface
void LayoutControllerInit(int32_t width, int32_t height)
{
    OHOS::WMServer::LayoutController::GetInstance().Init(width, height);
}

int32_t LayoutControllerCalcWindowDefaultLayout(uint32_t type,
    uint32_t mode, uint32_t *zIndex, struct layout *outLayout)
{
    struct OHOS::WMServer::Layout layout = {};
    auto ret = OHOS::WMServer::LayoutController::GetInstance().CalcWindowDefaultLayout(type, mode, layout);
    if (zIndex != nullptr) {
        *zIndex = layout.zIndex;
    }
    if (outLayout != nullptr) {
        *outLayout = layout.layout;
    }
    return ret;
}

// C++ interface
namespace OHOS::WMServer {
LayoutController &LayoutController::GetInstance()
{
    static LayoutController instance;
    return instance;
}

void LayoutController::Init(int32_t width, int32_t height)
{
    displayWidth = width;
    displayHeight = height;
    if (!init) {
        InitByDefaultValue();
        InitByParseSCSS();
        init = true;
    }
}

int32_t LayoutController::CalcWindowDefaultLayout(uint32_t type, uint32_t mode, struct Layout &outLayout)
{
    auto it = modeLayoutMap[mode].find(type);
    if (it == modeLayoutMap[mode].end()) {
        if (mode == WINDOW_MODE_UNSET) {
            return 1;
        } else {
            it = modeLayoutMap[WINDOW_MODE_UNSET].find(type);
            if (it == modeLayoutMap[WINDOW_MODE_UNSET].end()) {
                return 1;
            }
        }
    }

    struct layout rect = { 0, 0, (double)displayWidth, (double)displayHeight };
    outLayout = it->second;
    if (outLayout.positionType == Layout::PositionType::RELATIVE) {
        CalcNormalRect(rect);
    }

    constexpr double full = 100.0; // 100%
    constexpr int32_t half = 2;
    outLayout.layout.x = floor(rect.w * outLayout.layout.x / full + 1e-6);
    outLayout.layout.y = floor(rect.h * outLayout.layout.y / full + 1e-6);
    outLayout.layout.w = floor(rect.w * outLayout.layout.w / full + 1e-6);
    outLayout.layout.h = floor(rect.h * outLayout.layout.h / full + 1e-6);
    if (outLayout.pTypeX == Layout::XPositionType::LFT) {
        outLayout.layout.x = floor(rect.x + 1e-6);
    } else if (outLayout.pTypeX == Layout::XPositionType::MID) {
        outLayout.layout.x = floor(rect.x + (rect.w - outLayout.layout.w) / half + 1e-6);
    } else if (outLayout.pTypeX == Layout::XPositionType::RGH) {
        outLayout.layout.x = floor(rect.x + rect.w - outLayout.layout.w + 1e-6);
    } else {
        outLayout.layout.x += floor(rect.x + 1e-6);
    }

    if (outLayout.pTypeY == Layout::YPositionType::TOP) {
        outLayout.layout.y = floor(rect.y + 1e-6);
    } else if (outLayout.pTypeY == Layout::YPositionType::MID) {
        outLayout.layout.y = floor(rect.y + (rect.h - outLayout.layout.h) / half + 1e-6);
    } else if (outLayout.pTypeY == Layout::YPositionType::BTM) {
        outLayout.layout.y = floor(rect.y + rect.h - outLayout.layout.h + 1e-6);
    } else {
        outLayout.layout.y += floor(rect.y + 1e-6);
    }
    return 0;
}

#define DEF_LYT_MACRO(wt, lt, ptx, pty, _x, _y, _w, _h) \
    modeLayoutMap[WINDOW_MODE_UNSET][WINDOW_TYPE_##wt] = { \
        .zIndex = WINDOW_TYPE_##wt, \
        .positionType = Layout::PositionType::lt, \
        .pTypeX = Layout::XPositionType::ptx, \
        .pTypeY = Layout::YPositionType::pty, \
        .layout = { .x = _x, .y = _y, .w = _w, .h = _h, }, \
    }

#define DEF_POS_LYT(lt, ptx, pty, w, h, wt) \
    DEF_LYT_MACRO(wt, lt, ptx, pty, 0, 0, w, h)

#define DEF_RCT_LYT(lt, x, y, w, h, wt) \
    DEF_LYT_MACRO(wt, lt, UNSET, UNSET, x, y, w, h)

void LayoutController::InitByDefaultValue()
{
    constexpr double full = 100.0; // 100%
    DEF_POS_LYT(RELATIVE, MID, MID, full, full, NORMAL);
    modeLayoutMap[WINDOW_MODE_FREE][WINDOW_TYPE_NORMAL] = {
        .zIndex = static_cast<int32_t>(51.0 + 1e-6),
        .positionType = Layout::PositionType::FIXED,
        .pTypeX = Layout::XPositionType::MID,
        .pTypeY = Layout::YPositionType::MID,
        .layout = {
            .x = 0,
            .y = 0,
            .w = full,
            .h = full,
        },
    };
    DEF_POS_LYT(STATIC, MID, TOP, full, 7.0, STATUS_BAR);
    DEF_POS_LYT(STATIC, MID, BTM, full, 7.0, NAVI_BAR);
    DEF_POS_LYT(FIXED, MID, MID, 80.0, 30.0, ALARM_SCREEN);
    DEF_POS_LYT(FIXED, MID, MID, full, full, SYSTEM_UI);
    DEF_POS_LYT(RELATIVE, MID, MID, full, full, LAUNCHER);
    DEF_POS_LYT(FIXED, MID, MID, full, full, VIDEO);
    DEF_POS_LYT(RELATIVE, MID, BTM, full, 33.3, INPUT_METHOD);
    DEF_POS_LYT(RELATIVE, MID, BTM, 90.0, 33.3, INPUT_METHOD_SELECTOR);
    DEF_RCT_LYT(FIXED, 2.5, 2.5, 95.0, 40.0, VOLUME_OVERLAY);
    DEF_POS_LYT(FIXED, MID, TOP, full, 50.0, NOTIFICATION_SHADE);
    DEF_RCT_LYT(RELATIVE, 7.5, 7.5, 85.0, 50.0, FLOAT);
}

void LayoutController::InitByParseSCSS()
{
    fs::path dir{searchCSSDirectory};
    if (!fs::exists(dir)) {
        LOGE("%{public}s not exist dir", searchCSSDirectory.c_str());
        return;
    }

    fs::directory_entry entry{dir};
    if (entry.status().type() != fs::file_type::directory) {
        LOGE("%{public}s is not dir", searchCSSDirectory.c_str());
        return;
    }

    fs::directory_iterator files{dir};
    for (const auto &file : files) {
        LOGI("found file: %{public}s", file.path().string().c_str());
        if (file.is_regular_file() && file.path().extension().string() == ".scss") {
            ParseSCSS(file.path());
        }
    }
}

bool LayoutController::CalcNormalRect(struct layout &layout)
{
    Rects totalRects{0, 0, displayWidth, displayHeight};
    for (const auto &[type, layout] : modeLayoutMap[WINDOW_MODE_UNSET]) {
        if (layout.positionType == Layout::PositionType::STATIC) {
            struct Layout nowLayout = {};
            CalcWindowDefaultLayout(type, WINDOW_MODE_UNSET, nowLayout);
            Rects rect{
                static_cast<int32_t>(nowLayout.layout.x + 1e-6),
                static_cast<int32_t>(nowLayout.layout.y + 1e-6),
                static_cast<int32_t>(nowLayout.layout.w + 1e-6),
                static_cast<int32_t>(nowLayout.layout.h + 1e-6),
            };
            totalRects -= rect;
        }
    }
    if (totalRects.GetSize() == 1) {
        layout.x = totalRects.GetX(0);
        layout.y = totalRects.GetY(0);
        layout.w = totalRects.GetW(0);
        layout.h = totalRects.GetH(0);
        return true;
    }
    LOGE("CalcNormalRect failed, size: %{public}d", totalRects.GetSize());
    for (int32_t i = 0; i < totalRects.GetSize(); i++) {
        LOGE("  +[%{public}d] = (%{public}d, %{public}d) %{public}dx%{public}d", i,
             totalRects.GetX(i), totalRects.GetY(i), totalRects.GetW(i), totalRects.GetH(i));
    }
    return false;
}

void LayoutController::ParseSCSS(const fs::path &file)
{
    std::ifstream ifs{file};
    int32_t size = 0;
    ifs >> size;

    int32_t mode = 0;
    int32_t type = 0;
    struct Layout layout = {};
    for (size_t i = 0; i < size; i++) {
        ifs >> mode >> type >> layout;
        if (layout.isZIndexSetting) {
            modeLayoutMap[mode][type].zIndex = layout.zIndex;
        }
        if (layout.isPositionTypeSetting) {
            modeLayoutMap[mode][type].positionType = layout.positionType;
        }
        if (layout.isPositionXTypeSetting) {
            modeLayoutMap[mode][type].pTypeX = layout.pTypeX;
        }
        if (layout.isPositionYTypeSetting) {
            modeLayoutMap[mode][type].pTypeY = layout.pTypeY;
        }
        if (layout.isLayoutSetting) {
            modeLayoutMap[mode][type].layout = layout.layout;
        }
    }
}
} // namespace OHOS::WMServer
