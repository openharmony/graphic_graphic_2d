/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/color_picker/rs_color_picker_manager.h"

#include <chrono>
#include <memory>

#include "feature/color_picker/rs_color_picker_thread.h"
#include "feature/color_picker/rs_color_picker_utils.h"

#include "common/rs_optional_trace.h"

namespace OHOS::Rosen {
namespace {
constexpr float COLOR_PICKER_ANIMATE_DURATION = 133.0f;

inline uint64_t NowMs()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

inline Drawing::ColorQuad GetColorOrDefault(Drawing::ColorQuad colorValue, bool darkMode)
{
    return colorValue != Drawing::Color::COLOR_TRANSPARENT
               ? colorValue
               : (darkMode ? Drawing::Color::COLOR_WHITE : Drawing::Color::COLOR_BLACK);
}
} // namespace

std::optional<Drawing::ColorQuad> RSColorPickerManager::GetColorPick()
{
    uint64_t currTime = NowMs();
    const auto [prevColor, curColor] = GetColor();
    const float animFraction = static_cast<float>(currTime - animStartTime_) / COLOR_PICKER_ANIMATE_DURATION;
    // Continue animation if in progress
    if (animFraction <= 1.0f) {
        RSColorPickerThread::Instance().NotifyNodeDirty(nodeId_); // continue animation
    }
    return InterpolateColor(prevColor, curColor, animFraction);
}

void RSColorPickerManager::SetSystemDarkColorMode(bool isSystemDarkColorMode)
{
    isSystemDarkColorMode_.store(isSystemDarkColorMode, std::memory_order_relaxed);
}

void RSColorPickerManager::ScheduleColorPick(
    RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, const ColorPickerParam& params)
{
    auto ptr = std::static_pointer_cast<IColorPickerManager>(shared_from_this());
    RSColorPickerUtils::ExtractSnapshotAndScheduleColorPick(canvas, rect, ptr);
}

void RSColorPickerManager::HandleColorUpdate(Drawing::ColorQuad newColor)
{
    const bool darkMode = isSystemDarkColorMode_.load(std::memory_order_relaxed);
    Drawing::ColorQuad prevColor = GetColorOrDefault(prevColor_.load(std::memory_order_relaxed), darkMode);
    Drawing::ColorQuad curColor = GetColorOrDefault(colorPicked_.load(std::memory_order_relaxed), darkMode);
    RS_OPTIONAL_TRACE_NAME_FMT(
        "RSColorPickerManager::extracted isSystemDarkColorMode: %d background color = %x, prevColor = %x"
        ", nodeId = %lu",
        darkMode, newColor, prevColor, nodeId_);
    newColor = GetContrastColor(newColor, curColor == Drawing::Color::COLOR_BLACK);
    if (newColor == curColor) {
        return;
    }

    const uint64_t now = NowMs();
    float animFraction = static_cast<float>(now - animStartTime_) / COLOR_PICKER_ANIMATE_DURATION;
    animFraction = std::clamp(animFraction, 0.0f, 1.0f);
    prevColor_.store(InterpolateColor(prevColor, curColor, animFraction), std::memory_order_relaxed);
    colorPicked_.store(newColor, std::memory_order_relaxed);
    animStartTime_ = now;
    RSColorPickerThread::Instance().NotifyNodeDirty(nodeId_);
}

Drawing::ColorQuad RSColorPickerManager::InterpolateColor(
    Drawing::ColorQuad startColor, Drawing::ColorQuad endColor, float fraction)
{
    if (std::clamp(fraction, 0.0f, 1.0f) != fraction) {
        return endColor;
    }
    auto lerp = [](uint32_t a, uint32_t b, float t) {
        return static_cast<uint32_t>(static_cast<float>(a) + (static_cast<float>(b) - static_cast<float>(a)) * t);
    };
    float t = std::clamp(fraction, 0.0f, 1.0f);
    const uint32_t a0 = Drawing::Color::ColorQuadGetA(startColor);
    const uint32_t r0 = Drawing::Color::ColorQuadGetR(startColor);
    const uint32_t g0 = Drawing::Color::ColorQuadGetG(startColor);
    const uint32_t b0 = Drawing::Color::ColorQuadGetB(startColor);
    const uint32_t a1 = Drawing::Color::ColorQuadGetA(endColor);
    const uint32_t r1 = Drawing::Color::ColorQuadGetR(endColor);
    const uint32_t g1 = Drawing::Color::ColorQuadGetG(endColor);
    const uint32_t b1 = Drawing::Color::ColorQuadGetB(endColor);
    const uint32_t a = lerp(a0, a1, t);
    const uint32_t r = lerp(r0, r1, t);
    const uint32_t g = lerp(g0, g1, t);
    const uint32_t b = lerp(b0, b1, t);
    return Drawing::Color::ColorQuadSetARGB(a, r, g, b);
}

std::pair<Drawing::ColorQuad, Drawing::ColorQuad> RSColorPickerManager::GetColor() const
{
    const bool darkMode = isSystemDarkColorMode_.load(std::memory_order_relaxed);
    auto prevColor = GetColorOrDefault(prevColor_.load(std::memory_order_relaxed), darkMode);
    auto curColor = GetColorOrDefault(colorPicked_.load(std::memory_order_relaxed), darkMode);
    return { prevColor, curColor };
}

namespace {
constexpr float THRESHOLD_HIGH = 220.0f;
constexpr float THRESHOLD_LOW = 150.0f;
} // namespace

Drawing::ColorQuad RSColorPickerManager::GetContrastColor(Drawing::ColorQuad color, bool prevDark)
{
    float luminance = RSColorPickerUtils::CalculateLuminance(color);

    // Use hysteresis thresholds based on previous contrast color state
    const float threshold = prevDark ? THRESHOLD_LOW : THRESHOLD_HIGH;
    return luminance > threshold ? Drawing::Color::COLOR_BLACK : Drawing::Color::COLOR_WHITE;
}
} // namespace OHOS::Rosen
