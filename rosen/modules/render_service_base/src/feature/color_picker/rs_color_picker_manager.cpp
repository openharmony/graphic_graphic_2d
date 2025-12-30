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
#include "feature/color_picker/rs_hetero_color_picker.h"
#include "rs_trace.h"

#include "common/rs_color.h"
#include "draw/color.h"
#include "drawable/rs_property_drawable_utils.h"
#include "platform/common/rs_log.h"
#include "property/rs_color_picker_def.h"

namespace OHOS::Rosen {
namespace {
constexpr int64_t TASK_DELAY_TIME = 16;                 // 16ms
constexpr float COLOR_PICKER_ANIMATE_DURATION = 350.0f; // 350ms

inline uint64_t NowMs()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}
} // namespace

Drawing::ColorQuad RSColorPickerManager::GetColorPicked(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect,
    uint64_t nodeId, ColorPickStrategyType strategy, uint64_t interval)
{
    uint64_t currTime = NowMs();
    const auto [prevColor, curColor] = GetColor();
    const float animFraction = static_cast<float>(currTime - animStartTime_) / COLOR_PICKER_ANIMATE_DURATION;
    const auto res = InterpolateColor(prevColor, curColor, animFraction);
    RS_TRACE_NAME_FMT(
        "RSColorPickerManager::GetColorPicked: animFraction = %f, color = %x, prevColor = %x, interpolated color = %x",
        animFraction, curColor, prevColor, res);

    if (animFraction <= 1.0f) {
        RSColorPickerThread::Instance().NotifyNodeDirty(nodeId); // continue animation
    }

    if (strategy != ColorPickStrategyType::NONE && currTime >= interval + lastUpdateTime_) { // cooldown check
        ScheduleColorPick(canvas, rect, nodeId, strategy);
    }
    return res;
}

void RSColorPickerManager::ScheduleColorPick(
    RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId, ColorPickStrategyType strategy)
{
    if (rect == nullptr) {
        RS_LOGE("RSColorPickerManager::GetSnapshot rect invalid!");
        return;
    }

    canvas.Save();
    canvas.ClipRect(*rect, Drawing::ClipOp::INTERSECT, false);
    Drawing::RectI snapshotIBounds = canvas.GetRoundInDeviceClipBounds();
    canvas.Restore();

    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        RS_LOGE("RSColorPickerManager::GetSnapshot surface nullptr!");
        return;
    }
    auto snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    if (snapshot == nullptr) {
        RS_LOGE("RSColorPickerManager::GetSnapshot snapshot nullptr!");
        return;
    }

    auto ptr = std::static_pointer_cast<RSColorPickerManager>(shared_from_this());
    if (RSHeteroColorPicker::Instance().GetColor(
        [ptr, nodeId, strategy](
                Drawing::ColorQuad& newColor) { ptr->HandleColorUpdate(newColor, nodeId, strategy); },
            drawingSurface, snapshot)) {
        return; // accelerated color picker
    }

    auto colorPickTask = [snapshot, nodeId, strategy, weakThis = weak_from_this()]() {
        auto manager = weakThis.lock();
        if (!manager) {
            RS_LOGD("RSColorPickerThread manager not valid, return");
            return;
        }
        manager->PickColor(snapshot, nodeId, strategy);
    };
    RSColorPickerThread::Instance().PostTask(colorPickTask, TASK_DELAY_TIME);
}

void RSColorPickerManager::PickColor(
    const std::shared_ptr<Drawing::Image>& snapshot, uint64_t nodeId, ColorPickStrategyType strategy)
{
    Drawing::ColorQuad colorPicked;
    bool prevDark;
    {
        std::lock_guard<std::mutex> lock(colorMtx_);
        prevDark = (colorPicked_ == Drawing::Color::COLOR_BLACK);
    }
#if defined(RS_ENABLE_UNI_RENDER)
    auto gpuCtx = RSColorPickerThread::Instance().GetShareGPUContext();
#else
    auto gpuCtx = nullptr;
#endif
    if (RSPropertyDrawableUtils::PickColor(gpuCtx, snapshot, colorPicked)) {
        HandleColorUpdate(colorPicked, nodeId, strategy);
    } else {
        RS_LOGE("RSColorPickerThread colorPick failed");
    }
}

void RSColorPickerManager::HandleColorUpdate(
    Drawing::ColorQuad newColor, uint64_t nodeId, ColorPickStrategyType strategy)
{
    {
        std::lock_guard<std::mutex> lock(colorMtx_);
        if (strategy == ColorPickStrategyType::CONTRAST) {
            newColor = GetContrastColor(newColor, colorPicked_ == Drawing::Color::COLOR_BLACK);
        }
        if (newColor == colorPicked_) {
            return;
        }

        const uint64_t now = NowMs();
        float animFraction = static_cast<float>(now - animStartTime_) / COLOR_PICKER_ANIMATE_DURATION;
        animFraction = std::clamp(animFraction, 0.0f, 1.0f);
        prevColor_ = InterpolateColor(prevColor_, colorPicked_, animFraction);
        colorPicked_ = newColor;
        animStartTime_ = now;

        RS_TRACE_NAME_FMT(
            "RSColorPickerManager::notifyNodeDirty, prevColor = %x, newColor = %x", prevColor_, colorPicked_);
    }
    RSColorPickerThread::Instance().NotifyNodeDirty(nodeId);
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

inline std::pair<Drawing::ColorQuad, Drawing::ColorQuad> RSColorPickerManager::GetColor()
{
    std::lock_guard<std::mutex> lock(colorMtx_);
    return { prevColor_, colorPicked_ };
}

namespace {
constexpr float RED_LUMINANCE_COEFF = 0.299f;
constexpr float GREEN_LUMINANCE_COEFF = 0.587f;
constexpr float BLUE_LUMINANCE_COEFF = 0.114f;
constexpr float THRESHOLD_HIGH = 220.0f;
constexpr float THRESHOLD_LOW = 150.0f;
} // namespace

Drawing::ColorQuad RSColorPickerManager::GetContrastColor(Drawing::ColorQuad color, bool prevDark)
{
    auto red = Drawing::Color::ColorQuadGetR(color);
    auto green = Drawing::Color::ColorQuadGetG(color);
    auto blue = Drawing::Color::ColorQuadGetB(color);
    float luminance = red * RED_LUMINANCE_COEFF + green * GREEN_LUMINANCE_COEFF + blue * BLUE_LUMINANCE_COEFF;

    // Use hysteresis thresholds based on previous contrast color state
    const float threshold = prevDark ? THRESHOLD_LOW : THRESHOLD_HIGH;
    return luminance > threshold ? Drawing::Color::COLOR_BLACK : Drawing::Color::COLOR_WHITE;
}
} // namespace OHOS::Rosen
