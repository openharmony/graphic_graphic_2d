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

#include "feature/colorpicker/rs_color_picker_manager.h"

#include <chrono>
#include <memory>

#include "feature/colorpicker/rs_color_picker_thread.h"
#include "feature/colorpicker/rs_hetero_pick_color_manager.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "drawable/rs_property_drawable_utils.h"

namespace OHOS::Rosen {
constexpr int64_t TASK_DELAY_TIME = 16; // 16ms
Drawing::ColorQuad RSColorPickerManager::GetColorPicked(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect,
    uint64_t nodeId, ColorPickStrategyType strategy, uint64_t interval)
{
    uint64_t currTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    if (lastUpdateTime_ != 0 && currTime < interval + lastUpdateTime_) {
        lastUpdateTime_ = lastUpdateTime_ == 0 ? currTime : lastUpdateTime_;
        return colorPicked_;
    }

    if (rect == nullptr) {
        RS_LOGE("RSColorPickerManager::GetColorPicked rect invalid!");
        return colorPicked_;
    }

    canvas.Save();
    canvas.ClipRect(*rect, Drawing::ClipOp::INTERSECT, false);
    Drawing::RectI snapshotIBounds = canvas.GetRoundInDeviceClipBounds();
    canvas.Restore();

    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        RS_LOGE("RSColorPickerManager::GetColorPicked surface nullptr!");
        return colorPicked_;
    }
    RS_TRACE_NAME_FMT("RSColorPickerManager::GetColorPicked surface wh: [%d, %d], snapshotIBounds: %s",
        drawingSurface->Width(), drawingSurface->Height(), snapshotIBounds.ToString().c_str());
    auto snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    if (snapshot == nullptr) {
        RS_LOGE("RSColorPickerManager::GetColorPicked snapshot nullptr!");
        return colorPicked_;
    }
    lastUpdateTime_ = currTime;

    auto ptr = std::static_pointer_cast<RSColorPickerManager>(shared_from_this());
    auto updateColor = [ptr, nodeId](Drawing::ColorQuad& newColor) {
        ptr->colorPicked_ = newColor;
        RSColorPickerThread::Instance().NotifyNodeDirty(nodeId);
    };
    if (!RSHeteroPickColorManager::Instance().GetColor(updateColor, drawingSurface, snapshot)) {
        auto colorPickTask = [snapshot, nodeId, strategy, weakThis = weak_from_this()]() {
            auto manager = weakThis.lock();
            if (!manager) {
                RS_LOGD("RSColorPickerThread manager not valid, return");
                return;
            }
            Drawing::ColorQuad colorPicked;
    #if defined(RS_ENABLE_UNI_RENDER)
            if (RSPropertyDrawableUtils::PickColor(RSColorPickerThread::Instance().GetShareGPUContext(), snapshot,
    #else
            if (RSPropertyDrawableUtils::PickColor(nullptr, snapshot,
    #endif
                colorPicked, strategy)) {
                if (manager->colorPicked_.exchange(colorPicked) != colorPicked) {
                    RSColorPickerThread::Instance().NotifyNodeDirty(nodeId);
                }
                RS_LOGD("RSColorPickerThread colorPicked_:0x%{public}" PRIX64, static_cast<uint64_t>(colorPicked));
                RS_TRACE_NAME_FMT("RSColorPickerThread colorPicked_:%d", colorPicked);
            } else {
                RS_LOGE("RSColorPickerThread colorPick failed");
            }
        };
        RSColorPickerThread::Instance().PostTask(colorPickTask, TASK_DELAY_TIME);
    }

    return colorPicked_;
}
} // OHOS::Rosen
