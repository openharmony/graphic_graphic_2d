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
#include "feature/color_picker/rs_hetero_color_picker.h"
#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"

namespace OHOS::Rosen {
namespace {
constexpr int32_t TRACE_LEVEL_TWO = 2;
constexpr float COLOR_PICKER_ANIMATE_DURATION = 350.0f; // 350ms

inline uint64_t NowMs()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
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
    // Cooldown check now done in RSColorPickerDrawable::Prepare()
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
        [ptr, strategy = params.strategy](
                Drawing::ColorQuad& newColor) { ptr->HandleColorUpdate(newColor, strategy); },
            canvas, snapshot)) {
        return; // accelerated color picker
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = snapshot->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("RSColorPickerManager::ScheduleColorPick backendTexture invalid");
        return;
    }
    auto weakThis = weak_from_this();
    auto imageInfo = drawingSurface->GetImageInfo();
    auto colorSpace = imageInfo.GetColorSpace();
    Drawing::BitmapFormat bitmapFormat = { imageInfo.GetColorType(), imageInfo.GetAlphaType() };
    ColorPickerInfo* colorPickerInfo =
        new ColorPickerInfo(colorSpace, bitmapFormat, backendTexture, snapshot, nodeId_, weakThis, params.strategy);

    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.finishedProc = [](void* context) { ColorPickerInfo::PickColor(context); };
    drawingFlushInfo.finishedContext = colorPickerInfo;
    drawingSurface->Flush(&drawingFlushInfo);
}

void RSColorPickerManager::PickColor(
    const std::shared_ptr<Drawing::Image>& snapshot, ColorPickStrategyType strategy)
{
    Drawing::ColorQuad colorPicked;
#if defined(RS_ENABLE_UNI_RENDER)
    auto gpuCtx = RSColorPickerThread::Instance().GetShareGPUContext();
#else
    auto gpuCtx = nullptr;
#endif
    if (RSPropertyDrawableUtils::PickColor(gpuCtx, snapshot, colorPicked)) {
        HandleColorUpdate(colorPicked, strategy);
    } else {
        RS_LOGE("RSColorPickerThread colorPick failed");
    }
}

void RSColorPickerManager::HandleColorUpdate(
    Drawing::ColorQuad newColor, ColorPickStrategyType strategy)
{
    const bool isSystemDarkColorMode = isSystemDarkColorMode_.load(std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(colorMtx_);
        Drawing::ColorQuad prevColor = prevColor_.has_value() ? prevColor_.value() :
            (isSystemDarkColorMode ? Drawing::Color::COLOR_WHITE : Drawing::Color::COLOR_BLACK);
        Drawing::ColorQuad curColor = colorPicked_.has_value() ? colorPicked_.value() :
            (isSystemDarkColorMode ? Drawing::Color::COLOR_WHITE : Drawing::Color::COLOR_BLACK);
        RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
            "RSColorPickerManager::extracted isSystemDarkColorMode: %d background color = %x, prevColor = %x"
            ", nodeId = %lu", isSystemDarkColorMode, newColor, prevColor, nodeId_);
        if (strategy == ColorPickStrategyType::CONTRAST) {
            newColor = GetContrastColor(newColor, curColor == Drawing::Color::COLOR_BLACK);
        }
        if (colorPicked_.has_value() && newColor == colorPicked_.value()) {
            return;
        }

        const uint64_t now = NowMs();
        float animFraction = static_cast<float>(now - animStartTime_) / COLOR_PICKER_ANIMATE_DURATION;
        animFraction = std::clamp(animFraction, 0.0f, 1.0f);
        prevColor_ = InterpolateColor(prevColor, curColor, animFraction);
        colorPicked_ = newColor;
        animStartTime_ = now;
    }
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

inline std::pair<Drawing::ColorQuad, Drawing::ColorQuad> RSColorPickerManager::GetColor()
{
    const bool isSystemDarkColorMode = isSystemDarkColorMode_.load(std::memory_order_relaxed);
    std::lock_guard<std::mutex> lock(colorMtx_);
    Drawing::ColorQuad prevColor = prevColor_.has_value() ? prevColor_.value() :
        (isSystemDarkColorMode ? Drawing::Color::COLOR_WHITE : Drawing::Color::COLOR_BLACK);
    Drawing::ColorQuad curColor = colorPicked_.has_value() ? colorPicked_.value() :
        (isSystemDarkColorMode ? Drawing::Color::COLOR_WHITE : Drawing::Color::COLOR_BLACK);
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
