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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_COLOR_PICK_ALT_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_COLOR_PICK_ALT_MANAGER_H

#include <atomic>
#include <memory>

#include "drawable/rs_property_drawable_utils.h"
#include "feature/color_picker/rs_color_picker_thread.h"
#include "i_color_picker_manager.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Image;
}
class ColorPickAltManager : public std::enable_shared_from_this<ColorPickAltManager>, public IColorPickerManager {
public:
    ColorPickAltManager() = default;
    ~ColorPickAltManager() noexcept = default;
    std::optional<Drawing::ColorQuad> GetColorPicked(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect,
        uint64_t nodeId, const ColorPickerParam& params) override;

private:
    struct ColorPickerInfo {
        std::shared_ptr<Drawing::ColorSpace> colorSpace_;
        Drawing::BitmapFormat bitmapFormat_;
        Drawing::BackendTexture backendTexture_;
        NodeId nodeId_;
        std::weak_ptr<ColorPickAltManager> manager_;

        ColorPickerInfo(std::shared_ptr<Drawing::ColorSpace> colorSpace, Drawing::BitmapFormat bitmapFormat,
            Drawing::BackendTexture backendTexture, NodeId nodeId, std::weak_ptr<ColorPickAltManager> manager)
            : colorSpace_(colorSpace), bitmapFormat_(bitmapFormat), backendTexture_(backendTexture), nodeId_(nodeId),
              manager_(manager) {}

        static void PickColor(void* context)
        {
            ColorPickerInfo* colorPickerInfo = static_cast<ColorPickerInfo*>(context);
            if (colorPickerInfo == nullptr) {
                RS_LOGE("ColorPickerInfo::PickColor context nullptr");
                return;
            }
            std::shared_ptr<ColorPickerInfo> info(colorPickerInfo);
            auto task = [info]() {
                auto manager = info->manager_.lock();
                if (manager == nullptr) {
                    RS_LOGE("ColorPickerInfo::PickColor manager nullptr");
                    return;
                }
#if defined(RS_ENABLE_UNI_RENDER)
                auto gpuCtx = RSColorPickerThread::Instance().GetShareGPUContext();

#else
                std::shared_ptr<Drawing::GPUContext> gpuCtx = nullptr;
#endif
                if (gpuCtx == nullptr) {
                    RS_LOGE("ColorPickerInfo::PickColor GPUContext nullptr");
                    return;
                }
                auto image = std::make_shared<Drawing::Image>();
                Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
                if (!image->BuildFromTexture(*gpuCtx, info->backendTexture_.GetTextureInfo(), origin,
                    info->bitmapFormat_, info->colorSpace_)) {
                    RS_LOGE("ColorPickerInfo::PickColor BuildFromTexture failed");
                    return;
                }
                Drawing::ColorQuad colorPicked;
                if (RSPropertyDrawableUtils::PickColor(gpuCtx, image, colorPicked)) {
                    manager->HandleColorUpdate(colorPicked, info->nodeId_);
                } else {
                    RS_LOGE("ColorPickAltManager: PickColor failed");
                }
            };
            RSColorPickerThread::Instance().PostTask(task, 0);
        }
    };

    void ScheduleColorPick(
        RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId);
    void HandleColorUpdate(Drawing::ColorQuad newColor, uint64_t nodeId);

    std::atomic<Drawing::ColorQuad> pickedColor_ = Drawing::Color::COLOR_BLACK;
    uint64_t lastUpdateTime_ = 0;
    std::atomic<uint32_t> notifyThreshold_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_COLOR_PICK_ALT_MANAGER_H
