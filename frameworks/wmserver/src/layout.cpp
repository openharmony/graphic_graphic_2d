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

#include "layout.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

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

int32_t LayoutControllerUpdateStaticLayout(uint32_t type, const struct layout *layout)
{
    return OHOS::WMServer::LayoutController::GetInstance().UpdateStaticLayout(type, *layout);
}

int32_t LayoutControllerCalcWindowDefaultLayout(uint32_t type,
    uint32_t mode, uint32_t *zIndex, struct layout *outLayout)
{
    struct OHOS::WMServer::Layout layout;
    auto ret = OHOS::WMServer::LayoutController::GetInstance().CalcWindowDefaultLayout(type, mode, layout);
    if (zIndex != NULL) {
        *zIndex = layout.zIndex;
    }
    if (outLayout != NULL) {
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

int32_t LayoutController::UpdateStaticLayout(uint32_t type, const struct layout &layout)
{
    if (modeLayoutMap[WINDOW_MODE_UNSET].find(type) == modeLayoutMap[WINDOW_MODE_UNSET].end()) {
        return 1;
    }
    if (modeLayoutMap[WINDOW_MODE_UNSET][type].positionType != Layout::PositionType::STATIC) {
        return 2;
    }
    auto backup = modeLayoutMap[WINDOW_MODE_UNSET][type].layout;
    modeLayoutMap[WINDOW_MODE_UNSET][type].layout = layout;
    struct layout normal;
    if (!CalcNormalRect(normal)) {
        modeLayoutMap[WINDOW_MODE_UNSET][type].layout = backup;
        return 3;
    }
    return 0;
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
    outLayout.layout.x = rect.w * outLayout.layout.x / full;
    outLayout.layout.y = rect.h * outLayout.layout.y / full;
    outLayout.layout.w = rect.w * outLayout.layout.w / full;
    outLayout.layout.h = rect.h * outLayout.layout.h / full;
    if (outLayout.pTypeX == Layout::XPositionType::LFT) {
        outLayout.layout.x = rect.x;
    } else if (outLayout.pTypeX == Layout::XPositionType::MID) {
        outLayout.layout.x = rect.x + (rect.w - outLayout.layout.w) / half;
    } else if (outLayout.pTypeX == Layout::XPositionType::RGH) {
        outLayout.layout.x = rect.x + rect.w - outLayout.layout.w;
    } else {
        outLayout.layout.x += rect.x;
    }

    if (outLayout.pTypeY == Layout::YPositionType::TOP) {
        outLayout.layout.y = rect.y;
    } else if (outLayout.pTypeY == Layout::YPositionType::MID) {
        outLayout.layout.y = rect.y + (rect.h - outLayout.layout.h) / half;
    } else if (outLayout.pTypeY == Layout::YPositionType::BTM) {
        outLayout.layout.y = rect.y + rect.h - outLayout.layout.h;
    } else {
        outLayout.layout.y += rect.y;
    }
    return 0;
}

void LayoutController::RegisterAttributeProcessFunction(const char *attr,
                                                        AttributeProcessFunction func)
{
    attrProcessFuncs[attr] = func;
}

#define DEF_LYT_MACRO(wt, lt, ptx, pty, _x, _y, _w, _h) \
    modeLayoutMap[WINDOW_MODE_UNSET][WINDOW_TYPE_##wt] = { \
        .windowType = WINDOW_TYPE_##wt, \
        .windowTypeString = "WINDOW_TYPE_" #wt, \
        .zIndex = WINDOW_TYPE_##wt, \
        .positionType = Layout::PositionType::lt, \
        .pTypeX = Layout::XPositionType::ptx, \
        .pTypeY = Layout::YPositionType::pty, \
        .layout = { \
            .x = _x, \
            .y = _y, \
            .w = _w, \
            .h = _h, \
        }, \
    }

#define DEF_POS_LYT(lt, ptx, pty, w, h, wt) \
    DEF_LYT_MACRO(wt, lt, ptx, pty, 0, 0, w, h)

#define DEF_RCT_LYT(lt, x, y, w, h, wt) \
    DEF_LYT_MACRO(wt, lt, UNSET, UNSET, x, y, w, h)

void LayoutController::InitByDefaultValue()
{
    constexpr double full = 100.0; // 100%
    DEF_POS_LYT(RELATIVE, MID, MID, full, full, NORMAL);
    DEF_POS_LYT(STATIC, MID, TOP, full, 7.00, STATUS_BAR);
    DEF_POS_LYT(STATIC, MID, BTM, full, 7.00, NAVI_BAR);
    DEF_POS_LYT(FIXED, MID, MID, 80.0, 30.0, ALARM_SCREEN);
    DEF_POS_LYT(RELATIVE, MID, MID, full, full, LAUNCHER);
    DEF_POS_LYT(RELATIVE, MID, BTM, full, 33.3, INPUT_METHOD);
    DEF_POS_LYT(RELATIVE, MID, BTM, 90.0, 33.3, INPUT_METHOD_SELECTOR);
    DEF_POS_LYT(FIXED, MID, TOP, full, 50.0, NOTIFICATION_SHADE);
    DEF_RCT_LYT(FIXED, 2.5, 2.5, 95.0, 40.0, VOLUME_OVERLAY);
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

namespace {
bool DoubleEqual(double a, double b)
{
    return (a > b ? a - b : b - a) < 1e-6;
}

bool RectSubtract(struct layout &rect, struct layout other)
{
    if (other.w == 0 || other.h == 0) {
        return true;
    }
    if (DoubleEqual(other.w, rect.w) && DoubleEqual(other.x, rect.x)) {
        if (DoubleEqual(other.y, rect.y)) {
            rect.y += other.h;
            rect.h -= other.h;
            return true;
        } else if (DoubleEqual(other.y + other.h, rect.y + rect.h)) {
            rect.h -= other.h;
            return true;
        }
    }
    if (DoubleEqual(other.h, rect.h) && DoubleEqual(other.y, rect.y)) {
        if (DoubleEqual(other.x, rect.x)) {
            rect.x += other.w;
            rect.w -= other.w;
            return true;
        } else if (DoubleEqual(other.x + other.w, rect.x + rect.w)) {
            rect.w -= other.w;
            return true;
        }
    }

    LOGI("rect {%{public}lf %{public}lf %{public}lf %{public}lf}", rect.x, rect.y, rect.w, rect.h);
    LOGI("other {%{public}lf %{public}lf %{public}lf %{public}lf}", other.x, other.y, other.w, other.h);
    LOGE("RectSubtract failed");
    return false;
}
} // namespace

bool LayoutController::CalcNormalRect(struct layout &layout)
{
    struct layout rect = { 0, 0, (double)displayWidth, (double)displayHeight };
    struct Layout other;
    CalcWindowDefaultLayout(WINDOW_TYPE_STATUS_BAR, WINDOW_MODE_UNSET, other);
    if (!RectSubtract(rect, other.layout)) {
        LOGE("CalcNormalRect STATUS_BAR failed");
        return false;
    }
    CalcWindowDefaultLayout(WINDOW_TYPE_NAVI_BAR, WINDOW_MODE_UNSET, other);
    if (!RectSubtract(rect, other.layout)) {
        LOGE("CalcNormalRect NAVI_BAR failed");
        return false;
    }
    layout = rect;
    return true;
}

namespace {
bool IntegerParser(const std::string &value, int32_t &retval)
{
    std::regex numberRegex{"([0-9]+)"};
    std::smatch result;
    if (std::regex_match(value, result, numberRegex)) {
        std::stringstream ss;
        ss << result[1];
        ss >> retval;
        return true;
    }
    LOGE("invalid number (%{public}s)", value.c_str());
    return false;
}

bool NumberUnitParser(const std::string &value, int32_t total, double &retval)
{
    std::regex numberUnitRegex{"([0-9]+(\\.[0-9]+)?)(px|%)"};
    std::smatch result;
    if (std::regex_match(value, result, numberUnitRegex)) {
        std::stringstream ss;
        ss << result[1];
        ss >> retval;
        if (result[3] == "px") {
            constexpr double full = 100.0; // 100%
            retval = retval / total * full;
            return true;
        }
        if (result[3] == "%") {
            return true;
        }
    }
    LOGE("invalid number unit (%{public}s)", value.c_str());
    return false;
}

bool PositionTypeParser(const std::string &value, Layout::PositionType &retval)
{
    if (value == "relative") {
        retval = Layout::PositionType::RELATIVE;
        return true;
    } else if (value == "static") {
        retval = Layout::PositionType::STATIC;
        return true;
    } else if (value == "fixed") {
        retval = Layout::PositionType::FIXED;
        return true;
    }
    LOGE("invalid position type (%{public}s)", value.c_str());
    return false;
}

bool XPositionTypeParser(const std::string &value, Layout::XPositionType &retval)
{
    if (value == "left") {
        retval = Layout::XPositionType::LFT;
        return true;
    } else if (value == "middle") {
        retval = Layout::XPositionType::MID;
        return true;
    } else if (value == "right") {
        retval = Layout::XPositionType::RGH;
        return true;
    } else if (value == "unset") {
        retval = Layout::XPositionType::UNSET;
        return true;
    }
    LOGE("invalid x position type (%{public}s)", value.c_str());
    return false;
}

bool YPositionTypeParser(const std::string &value, Layout::YPositionType &retval)
{
    if (value == "top") {
        retval = Layout::YPositionType::TOP;
        return true;
    } else if (value == "middle") {
        retval = Layout::YPositionType::MID;
        return true;
    } else if (value == "bottom") {
        retval = Layout::YPositionType::BTM;
        return true;
    } else if (value == "unset") {
        retval = Layout::YPositionType::UNSET;
        return true;
    }
    LOGE("invalid y position type (%{public}s)", value.c_str());
    return false;
}

WindowMode WindowModeParser(const std::string &value)
{
    if (value == "unset") {
        return WINDOW_MODE_UNSET;
    } else if (value == "free") {
        return WINDOW_MODE_FREE;
    } else if (value == "full") {
        return WINDOW_MODE_FULL;
    }
    LOGE("invalid window mode (%{public}s)", value.c_str());
    return WINDOW_MODE_UNSET;
}

#define DEFINE_ATTRIBUTE_PROCESS_FUNCTION(attr, value, layout, displayWidth, displayHeight)   \
int32_t AttributeProcess##attr(const std::string &value,                                      \
                               struct Layout &layout,                                         \
                               int32_t displayWidth,                                          \
                               int32_t displayHeight);                                        \
__attribute__((constructor)) void RegisterAttributeProcess##attr()                            \
{                                                                                             \
    LayoutController::GetInstance().RegisterAttributeProcessFunction(#attr,                   \
                                                                     AttributeProcess##attr); \
}                                                                                             \
                                                                                              \
int32_t AttributeProcess##attr(const std::string &value,                                      \
                               struct Layout &layout,                                         \
                               int32_t displayWidth,                                          \
                               int32_t displayHeight)

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(zindex, value, layout, displayWidth, displayHeight)
{
    if (IntegerParser(value, layout.zIndex)) {
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(position, value, layout, displayWidth, displayHeight)
{
    if (PositionTypeParser(value, layout.positionType)) {
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(horizonalign, value, layout, displayWidth, displayHeight)
{
    if (XPositionTypeParser(value, layout.pTypeX)) {
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(verticalalign, value, layout, displayWidth, displayHeight)
{
    if (YPositionTypeParser(value, layout.pTypeY)) {
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(left, value, layout, displayWidth, displayHeight)
{
    if (NumberUnitParser(value, displayWidth, layout.layout.x)) {
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(top, value, layout, displayWidth, displayHeight)
{
    if (NumberUnitParser(value, displayHeight, layout.layout.y)) {
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(width, value, layout, displayWidth, displayHeight)
{
    if (NumberUnitParser(value, displayWidth, layout.layout.w)) {
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(height, value, layout, displayWidth, displayHeight)
{
    if (NumberUnitParser(value, displayHeight, layout.layout.h)) {
        return 0;
    }
    return 1;
}
} // namespace

void LayoutController::ParseSCSS(const fs::path &file)
{
    std::ifstream ifs{file};
    int ret = driver.parse(ifs);
    if (ret != 0) {
        LOGE("parse %{public}s failed", file.string().c_str());
        return;
    }
    LOGI("parse %{public}s success", file.string().c_str());

    for (const auto &[typeString, typeBlock] : driver.global.blocks) {
        LOGI("type: %{public}s", typeString.c_str());
        auto str = typeString;
        auto isTypeWindow = [&str](const auto &it) {
            return it.second.windowTypeString == str;
        };
        auto window = std::find_if(modeLayoutMap[WINDOW_MODE_UNSET].begin(),
            modeLayoutMap[WINDOW_MODE_UNSET].end(), isTypeWindow);
        if (window == modeLayoutMap[WINDOW_MODE_UNSET].end()) {
            continue;
        }

        ParseAttr(typeBlock, window->second);
        for (const auto &[modeString, modeBlock] : typeBlock.blocks) {
            LOGI("mode: %{public}s", modeString.c_str());
            WindowMode mode = WindowModeParser(modeString);
            auto modeWindow = std::find_if(modeLayoutMap[mode].begin(),
                modeLayoutMap[mode].end(), isTypeWindow);
            if (modeWindow == modeLayoutMap[mode].end()) {
                modeLayoutMap[mode][window->first] = window->second;
            }
            ParseAttr(modeBlock, modeLayoutMap[mode][window->first]);
        }
    }
}

void LayoutController::ParseAttr(const struct Driver::CSSBlock &block, struct Layout &layout)
{
    for (const auto &[attr, value] : block.declares) {
        LOGI("attr: %{public}s, value: %{public}s", attr.c_str(), value.c_str());
        std::string attribute = attr;
        if (attribute.find("-") != std::string::npos) {
            attribute.replace(attribute.find("-"), 1, "");
        }
        if (attribute.find("_") != std::string::npos) {
            attribute.replace(attribute.find("_"), 1, "");
        }
        auto it = attrProcessFuncs.find(attribute);
        if (it == attrProcessFuncs.end()) {
            LOGE("attr is invalid (%{public}s)", attribute.c_str());
        }

        auto ret = it->second(value, layout, displayWidth, displayHeight);
        if (ret != 0) {
            LOGI("value (%{public}s: %{public}s) return %{public}d",
                 attribute.c_str(), value.c_str(), ret);
        }
    }
}
} // namespace OHOS
