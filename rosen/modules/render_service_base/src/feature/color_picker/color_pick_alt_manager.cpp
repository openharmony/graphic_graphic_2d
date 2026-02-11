/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "feature/color_picker/color_pick_alt_manager.h"

#include <cstdint>
#include <memory>

#include "feature/color_picker/rs_color_picker_thread.h"
#include "feature/color_picker/rs_color_picker_utils.h"

#include "common/rs_optional_trace.h"

namespace OHOS::Rosen {
namespace {
constexpr int TRACE_LEVEL_TWO = 2;

template<typename T>
[[nodiscard]] inline bool IsNull(T* p, const char* msg)
{
    if (!p) {
        RS_LOGE("%s", msg);
        return true;
    }
    return false;
}
} // namespace

std::optional<Drawing::ColorQuad> ColorPickAltManager::GetColorPick()
{
    return std::nullopt;
}

void ColorPickAltManager::ScheduleColorPick(
    RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, const ColorPickerParam& params)
{
    if (params.strategy == ColorPickStrategyType::NONE) {
        return;
    }
    lightThreshold_.store(params.notifyThreshold.second, std::memory_order_relaxed);
    darkThreshold_.store(params.notifyThreshold.first, std::memory_order_relaxed);

    if (params.rect) {
        rect = &params.rect.value();
    }

    auto ptr = std::static_pointer_cast<IColorPickerManager>(shared_from_this());
    RSColorPickerUtils::ExtractSnapshotAndScheduleColorPick(canvas, rect, ptr);
}

namespace {
enum class LuminanceZone { DARK, LIGHT, NEUTRAL, UNKNOWN };
LuminanceZone GetLuminanceZone(uint32_t luminance, uint32_t darkThreshold, uint32_t lightThreshold)
{
    if (luminance > RGBA_MAX) {
        return LuminanceZone::UNKNOWN;
    }
    if (luminance < darkThreshold) {
        return LuminanceZone::DARK;
    }
    if (luminance > lightThreshold) {
        return LuminanceZone::LIGHT;
    }
    return LuminanceZone::NEUTRAL;
}
} // namespace

void ColorPickAltManager::HandleColorUpdate(Drawing::ColorQuad newColor)
{
    const auto newLuminance = static_cast<uint32_t>(std::round(RSColorPickerUtils::CalculateLuminance(newColor)));
    const uint32_t prevLuminance = pickedLuminance_.exchange(newLuminance, std::memory_order_relaxed);
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "ColorPickAltManager::extracted background luminance = %u, prevLuminance = %u, nodeId = %lu", newLuminance,
        prevLuminance, nodeId_);

    const uint32_t darkThreshold = darkThreshold_.load(std::memory_order_relaxed);
    const uint32_t lightThreshold = lightThreshold_.load(std::memory_order_relaxed);
    const auto prevZone = GetLuminanceZone(prevLuminance, darkThreshold, lightThreshold);
    const auto newZone = GetLuminanceZone(newLuminance, darkThreshold, lightThreshold);
    if (prevZone == newZone) {
        return;
    }
    RSColorPickerThread::Instance().NotifyClient(nodeId_, std::clamp(static_cast<uint32_t>(newLuminance), 0u, 255u));
}

} // namespace OHOS::Rosen
