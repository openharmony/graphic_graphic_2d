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

#include <chrono>
#include <cstdint>

#include "feature/color_picker/rs_color_picker_thread.h"
#include "feature/color_picker/rs_color_picker_utils.h"
#include "feature/color_picker/rs_hetero_color_picker.h"

#include "common/rs_optional_trace.h"

namespace OHOS::Rosen {
namespace {
constexpr int TRACE_LEVEL_TWO = 2;

inline uint64_t NowMs()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

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

std::optional<Drawing::ColorQuad> ColorPickAltManager::GetColorPicked(
    RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId, const ColorPickerParam& params)
{
    if (params.strategy == ColorPickStrategyType::NONE) {
        return std::nullopt;
    }
    uint64_t currTime = NowMs();
    // Respect cooldown interval
    if (currTime < params.interval + lastUpdateTime_) {
        return std::nullopt;
    }
    lastUpdateTime_ = currTime;
    lightThreshold_.store(params.notifyThreshold.second, std::memory_order_relaxed);
    darkThreshold_.store(params.notifyThreshold.first, std::memory_order_relaxed);

    ScheduleColorPick(canvas, rect, nodeId);
    return std::nullopt;
}

void ColorPickAltManager::ScheduleColorPick(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId)
{
    if (IsNull(rect, "ColorPickAltManager::GetSnapshot rect nullptr!")) {
        return;
    }
    canvas.Save();
    canvas.ClipRect(*rect, Drawing::ClipOp::INTERSECT, false);
    Drawing::RectI snapshotIBounds = canvas.GetRoundInDeviceClipBounds();
    canvas.Restore();

    auto drawingSurface = canvas.GetSurface();
    if (IsNull(drawingSurface, "ColorPickAltManager::GetSnapshot surface nullptr!")) {
        return;
    }
    auto snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    if (IsNull(snapshot.get(), "ColorPickAltManager::GetSnapshot snapshot nullptr!")) {
        return;
    }

    auto ptr = std::static_pointer_cast<ColorPickAltManager>(shared_from_this());
    // accelerated color picker
    if (RSHeteroColorPicker::Instance().GetColor(
        [ptr, nodeId](Drawing::ColorQuad& newColor) { ptr->HandleColorUpdate(newColor, nodeId); }, canvas, snapshot)) {
        return;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = snapshot->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("ColorPickAltManager::ScheduleColorPick backendTexture invalid");
        return;
    }
    auto weakThis = weak_from_this();
    auto imageInfo = drawingSurface->GetImageInfo();
    auto colorSpace = imageInfo.GetColorSpace();
    Drawing::BitmapFormat bitmapFormat = { imageInfo.GetColorType(), imageInfo.GetAlphaType() };
    ColorPickerInfo* colorPickerInfo =
        new ColorPickerInfo(colorSpace, bitmapFormat, backendTexture, snapshot, nodeId, weakThis);

    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.finishedProc = [](void* context) { ColorPickerInfo::PickColor(context); };
    drawingFlushInfo.finishedContext = colorPickerInfo;
    drawingSurface->Flush(&drawingFlushInfo);
}

namespace {
enum class LuminanceZone { DARK, LIGHT, NEUTRAL, UNKNOWN };
LuminanceZone GetLuminanceZone(uint32_t luminance, uint32_t darkThreshold, uint32_t lightThreshold)
{
    if (luminance < darkThreshold) {
        return LuminanceZone::DARK;
    } else if (luminance > lightThreshold) {
        return LuminanceZone::LIGHT;
    } else if (luminance > RGBA_MAX) {
        return LuminanceZone::UNKNOWN;
    } else {
        return LuminanceZone::NEUTRAL;
    }
}
} // namespace

void ColorPickAltManager::HandleColorUpdate(Drawing::ColorQuad newColor, NodeId nodeId)
{
    const auto newLuminance = static_cast<uint32_t>(std::round(RSColorPickerUtils::CalculateLuminance(newColor)));
    const uint32_t prevLuminance = pickedLuminance_.exchange(newLuminance, std::memory_order_relaxed);
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "ColorPickAltManager::extracted background luminance = %u, prevLuminance = %u, nodeId = %lu", newLuminance,
        prevLuminance, nodeId);

    const uint32_t darkThreshold = darkThreshold_.load(std::memory_order_relaxed);
    const uint32_t lightThreshold = lightThreshold_.load(std::memory_order_relaxed);
    const auto prevZone = GetLuminanceZone(prevLuminance, darkThreshold, lightThreshold);
    const auto newZone = GetLuminanceZone(newLuminance, darkThreshold, lightThreshold);
    if (prevZone == newZone) {
        return;
    }
    RSColorPickerThread::Instance().NotifyClient(nodeId, std::clamp(static_cast<uint32_t>(newLuminance), 0u, 255u));
}

} // namespace OHOS::Rosen
