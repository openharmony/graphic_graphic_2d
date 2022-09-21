/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "overdraw/rs_overdraw_controller.h"

#include <cstring>
#include <sstream>

#include "hilog/log.h"
#include "parameter.h"

#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
// param set debug.graphic.overdraw true/false
constexpr const char *switchText = "debug.graphic.overdraw";
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
constexpr const char *colorText = "debug.graphic.colors_overdraw";
constexpr const char *switchEnableText = "true";
} // namespace

RSOverdrawController &RSOverdrawController::GetInstance()
{
    static RSOverdrawController instance;
    return instance;
}

bool RSOverdrawController::IsEnabled() const
{
    return enabled_;
}

void RSOverdrawController::SetEnable(bool enable)
{
    enabled_ = enable;
}

const std::vector<uint32_t> &RSOverdrawController::GetColors() const
{
    return colors_;
}

void RSOverdrawController::SetColors(const std::vector<uint32_t> &colors)
{
    colors_ = colors;
}

RSOverdrawController::RSOverdrawController()
{
    char value[0x20];
    GetParameter(switchText, "false", value, sizeof(value));
    SwitchFunction(switchText, value, this);
    WatchParameter(switchText, SwitchFunction, this);
    WatchParameter(colorText, OnColorChange, this);
}

void RSOverdrawController::SwitchFunction(const char *key, const char *value, void *context)
{
    auto &that = *reinterpret_cast<RSOverdrawController *>(context);
    auto oldEnable = that.enabled_;
    if (strncmp(value, switchEnableText, strlen(switchEnableText)) == 0) {
        that.enabled_ = true;
        ROSEN_LOGI("%{public}s enable", key);
    } else {
        that.enabled_ = false;
        ROSEN_LOGI("%{public}s disable", key);
    }

    if (oldEnable != that.enabled_) {
        RSRenderThread::Instance().RequestNextVSync();
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

    auto oldColors = that.GetColors();
    if (ss.eof()) {
        that.SetColors(colors);
    }

    if (colors != oldColors) {
        RSRenderThread::Instance().RequestNextVSync();
    }
}
} // namespace Rosen
} // namespace OHOS
