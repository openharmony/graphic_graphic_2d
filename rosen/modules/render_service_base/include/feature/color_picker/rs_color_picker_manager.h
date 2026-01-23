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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H

#include <atomic>
#include <memory>
#include <mutex>

#include "feature/color_picker/rs_color_picker_thread.h"
#include "i_color_picker_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Image;
}
class RSColorPickerManager : public std::enable_shared_from_this<RSColorPickerManager>, public IColorPickerManager {
public:
    RSColorPickerManager() = default;
    ~RSColorPickerManager() noexcept = default;
    /**
     * @brief Return previously picked color and conditionally schedule a new color pick task on the current canvas.
     * @param strategy Only CONTRAST is currently supported.
     */
    std::optional<Drawing::ColorQuad> GetColorPicked(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect,
        uint64_t nodeId, const ColorPickerParam& params) override;

private:
    struct ColorPickerInfo {
        std::shared_ptr<Drawing::ColorSpace> colorSpace_;
        Drawing::BitmapFormat bitmapFormat_;
        Drawing::BackendTexture backendTexture_;
        std::shared_ptr<Drawing::Image> oldImage_;
        NodeId nodeId_;
        std::weak_ptr<RSColorPickerManager> manager_;
        ColorPickStrategyType strategy_;

        ColorPickerInfo(std::shared_ptr<Drawing::ColorSpace> colorSpace, Drawing::BitmapFormat bitmapFormat,
            Drawing::BackendTexture backendTexture, std::shared_ptr<Drawing::Image> image, NodeId nodeId,
            std::weak_ptr<RSColorPickerManager> manager, ColorPickStrategyType strategy)
            : colorSpace_(colorSpace), bitmapFormat_(bitmapFormat), backendTexture_(backendTexture), oldImage_(image),
              nodeId_(nodeId), manager_(manager), strategy_(strategy) {}

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
                if (gpuCtx == nullptr || info->oldImage_ == nullptr) {
                    RS_LOGE("ColorPickerInfo::PickColor param invalid");
                    return;
                }
                auto image = std::make_shared<Drawing::Image>();
                Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
                if (!image->BuildFromTexture(*gpuCtx, info->backendTexture_.GetTextureInfo(), origin,
                    info->bitmapFormat_, info->colorSpace_)) {
                    RS_LOGE("ColorPickerInfo::PickColor BuildFromTexture failed");
                    return;
                }
                manager->PickColor(image, info->nodeId_, info->strategy_);
                image = nullptr;
                info->oldImage_ = nullptr;
            };
            RSColorPickerThread::Instance().PostTask(task, 0);
        }
    };
    void ScheduleColorPick(
        RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId, ColorPickStrategyType strategy);
    void PickColor(const std::shared_ptr<Drawing::Image>& snapshot, uint64_t nodeId, ColorPickStrategyType strategy);
    void HandleColorUpdate(Drawing::ColorQuad newColor, uint64_t nodeId, ColorPickStrategyType strategy);

    inline std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColor();

    static Drawing::ColorQuad InterpolateColor(
        Drawing::ColorQuad startColor, Drawing::ColorQuad endColor, float fraction);

    /**
     * @brief Get a contrasting black or white color with hysteresis.
     *
     * @param color input to contrast with, usually the average color of the background.
     */
    static Drawing::ColorQuad GetContrastColor(Drawing::ColorQuad color);

    std::mutex colorMtx_;
    Drawing::ColorQuad colorPicked_ = Drawing::Color::COLOR_BLACK;
    Drawing::ColorQuad prevColor_ = Drawing::Color::COLOR_BLACK;

    std::atomic<uint64_t> animStartTime_ = 0;
    uint64_t lastUpdateTime_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H
