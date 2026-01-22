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

#include "common/rs_optional_trace.h"
#include "feature/color_picker/rs_hetero_color_picker.h"

namespace OHOS::Rosen {
namespace {
constexpr int TRACE_LEVEL_TWO = 2;

inline uint64_t NowMs()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

template <typename T>
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
    notifyThreshold_ = params.notifyThreshold;

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
        [ptr, nodeId](Drawing::ColorQuad& newColor) { ptr->HandleColorUpdate(newColor, nodeId); }, drawingSurface,
            snapshot)) {
        return;
    }

    auto weakThis = weak_from_this();
    auto imageInfo = drawingSurface->GetImageInfo();
    auto colorSpace = imageInfo.GetColorSpace();
    Drawing::BitmapFormat bitmapFormat = { imageInfo.GetColorType(), imageInfo.GetAlphaType() };
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = snapshot->GetBackendTexture(false, &origin);
    ColorPickerInfo* colorPickerInfo = new ColorPickerInfo(colorSpace, bitmapFormat, backendTexture, nodeId, weakThis);

    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.finishedProc = [](void* context) { ColorPickerInfo::PickColor(context); };
    drawingFlushInfo.finishedContext = colorPickerInfo;
    drawingSurface->Flush(&drawingFlushInfo);
}

void ColorPickAltManager::HandleColorUpdate(Drawing::ColorQuad newColor, NodeId nodeId)
{
    Drawing::ColorQuad prevColor = pickedColor_.load(std::memory_order_relaxed);
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSColorPickerManager::extracted background color = %x, prevColor = %x, nodeId = %lu", newColor, prevColor,
        nodeId);

    const uint32_t threshold = notifyThreshold_.load(std::memory_order_relaxed);
    // Check if color change exceeds threshold
    if (notifyThreshold_ > 0) {
        uint8_t prevR = (prevColor >> 16) & 0xFF;
        uint8_t prevG = (prevColor >> 8) & 0xFF;
        uint8_t prevB = prevColor & 0xFF;
        uint8_t prevA = (prevColor >> 24) & 0xFF;

        uint8_t newR = (newColor >> 16) & 0xFF;
        uint8_t newG = (newColor >> 8) & 0xFF;
        uint8_t newB = newColor & 0xFF;
        uint8_t newA = (newColor >> 24) & 0xFF;

        uint32_t diffR = std::abs(static_cast<int>(newR) - static_cast<int>(prevR));
        uint32_t diffG = std::abs(static_cast<int>(newG) - static_cast<int>(prevG));
        uint32_t diffB = std::abs(static_cast<int>(newB) - static_cast<int>(prevB));
        uint32_t diffA = std::abs(static_cast<int>(newA) - static_cast<int>(prevA));
        // Only notify if any channel difference exceeds threshold
        if (diffR <= threshold && diffG <= threshold && diffB <= threshold && diffA <= threshold) {
            return;
        }
    }

    if (pickedColor_.exchange(newColor, std::memory_order_relaxed) != newColor) {
        RSColorPickerThread::Instance().NotifyClient(nodeId, newColor);
    }
}

} // namespace OHOS::Rosen
