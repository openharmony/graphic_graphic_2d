/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "platform/ohos/overdraw/rs_overdraw_controller.h"

#include <mutex>
#include <sstream>

#include "hilog/log.h"
#include "parameter.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
// param set debug.graphic.overdraw true/false
constexpr const char *SWITCH_TEXT = "debug.graphic.overdraw";
constexpr const char *SWITCH_ENABLE_TEXT = "true";
/* param set debug.graphic.colors_overdraw [color...]
 * For Example:
 *   param set debug.graphic.colors_overdraw "0 0x220000ff 0x2200ff00 0x22ff0000 0x44ff0000"
 *   means:
 *   - Drawn Once Region: 0x00000000 (transparent)
 *   - Drawn Twice Region: 0x220000ff (alpha: 13% blue)
 *   - Drawn 3 Times Region: 0x2200ff00 (alpha: 13% green)
 *   - Drawn 4 Times Region: 0x22ff0000 (alpha: 13% red)
 *   - Drawn >= 5 Times Region: 0x44ff0000 (alpha: 26% red)
 */
constexpr const char *COLOR_TEXT = "debug.graphic.colors_overdraw";
} // namespace

RSOverdrawController &RSOverdrawController::GetInstance()
{
    static RSOverdrawController instance;
    return instance;
}

void RSOverdrawController::SetDelegate(const std::shared_ptr<RSDelegate> &delegate)
{
    delegate_ = delegate;
}

bool RSOverdrawController::IsEnabled() const
{
    return enabled_;
}

void RSOverdrawController::SetEnable(bool enable)
{
    if (enabled_ == enable) {
        return ;
    }
    if (enable) {
        SwitchFunction(SWITCH_TEXT, SWITCH_ENABLE_TEXT, this);
    } else {
        SwitchFunction(SWITCH_TEXT, "", this);
    }
}

OverdrawColorArray RSOverdrawController::GetColorArray() const
{
    std::lock_guard lock(colorMutex_);
    return colorArray_;
}

std::map<int, Drawing::ColorQuad> RSOverdrawController::GetColorMap() const
{
    std::lock_guard lock(colorMutex_);
    return colorMap_;
}

RSOverdrawController::RSOverdrawController()
{
    char value[0x20];
    GetParameter(SWITCH_TEXT, "false", value, sizeof(value));
    SwitchFunction(SWITCH_TEXT, value, this);
    WatchParameter(SWITCH_TEXT, SwitchFunction, this);
    WatchParameter(COLOR_TEXT, OnColorChange, this);
}

void RSOverdrawController::SwitchFunction(const char *key, const char *value, void *context)
{
    auto &that = *reinterpret_cast<RSOverdrawController *>(context);
    auto oldEnable = that.enabled_;
    if (strncmp(value, SWITCH_ENABLE_TEXT, strlen(SWITCH_ENABLE_TEXT)) == 0) {
        that.enabled_ = true;
        ROSEN_LOGI("%{public}s enable", key);
    } else {
        that.enabled_ = false;
        ROSEN_LOGI("%{public}s disable", key);
    }

    if (oldEnable != that.enabled_ && that.delegate_ != nullptr) {
        that.delegate_->Repaint();
    }
}

void RSOverdrawController::OnColorChange(const char *key, const char *value, void *context)
{
    auto &that = *reinterpret_cast<RSOverdrawController *>(context);
    std::stringstream ss(value);
    std::vector<uint32_t> colors;
    uint32_t color;
    while (ss >> std::hex >> color) {
        colors.push_back(color);
    }

    if (ss.eof() && colors != that.colors_ && colors.size() > 0) {
        // array
        OverdrawColorArray colorArray = that.colorArray_;
        auto colorNumber = colorArray.size();
        for (size_t i = 0; i < colors.size() && i + 1 < colorNumber; i++) {
            colorArray[i + 1] = colors[i];
        }
        for (size_t i = colors.size(); i + 1 < colorNumber; i++) {
            colorArray[i + 1] = colors.back();
        }

        // map
        std::map<int, Drawing::ColorQuad> colorMap;
        for (size_t i = 0; i < colors.size(); i++) {
            colorMap[i + 1] = colors[i];
        }
        colorMap[0] = colors.back();

        {
            std::lock_guard lock(that.colorMutex_);
            that.colorArray_ = colorArray;
            that.colorMap_ = colorMap;
        }

        if (that.delegate_ != nullptr) {
            that.delegate_->Repaint();
        }
    }
}
} // namespace Rosen
} // namespace OHOS
