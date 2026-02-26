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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_UTILS_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_UTILS_H

#include <cstdint>
#include <functional>
#include <memory>

#include "draw/color.h"
#include "image/image.h"

#ifdef RS_ENABLE_VK
#include "vulkan/vulkan_core.h"
#endif

namespace OHOS::Rosen {

class IColorPickerManager;
class RSPaintFilterCanvas;

namespace RSColorPickerUtils {
/**
 * @brief Calculate the luminance of a color using standard coefficients.
 *
 * Uses the ITU-R BT.601 standard for luminance calculation:
 * Y = 0.299*R + 0.587*G + 0.114*B
 *
 * @param color The color to calculate luminance for
 * @return float The luminance value in range [0, 255]
 */
float CalculateLuminance(Drawing::ColorQuad color);

struct ColorPickerInfo {
    std::shared_ptr<Drawing::ColorSpace> colorSpace_;
    Drawing::BitmapFormat bitmapFormat_;
    Drawing::BackendTexture backendTexture_;
    std::shared_ptr<Drawing::Image> oldImage_;
    std::weak_ptr<IColorPickerManager> manager_;
#ifdef RS_ENABLE_VK
    VkSemaphore waitSemaphore_ = VK_NULL_HANDLE;

    void SetSemaphore(VkSemaphore semaphore) { waitSemaphore_ = semaphore; }
#endif

    ColorPickerInfo(std::shared_ptr<Drawing::ColorSpace> colorSpace, Drawing::BitmapFormat bitmapFormat,
        Drawing::BackendTexture backendTexture, std::shared_ptr<Drawing::Image> image,
        std::weak_ptr<IColorPickerManager> manager)
        : colorSpace_(colorSpace), bitmapFormat_(bitmapFormat), backendTexture_(backendTexture), oldImage_(image),
          manager_(manager)
    {}
};

/**
 * @brief Extract backend texture and create ColorPickerInfo from surface and snapshot.
 *
 * This helper function extracts the backend texture, color space, and bitmap format
 * from a drawing surface and snapshot, then creates a ColorPickerInfo struct.
 *
 * @param drawingSurface The drawing surface
 * @param snapshot The image snapshot
 * @param manager Weak pointer to the color picker manager
 * @return unique_ptr<ColorPickerInfo> if successful, nullptr if backend texture is invalid
 */
std::unique_ptr<ColorPickerInfo> CreateColorPickerInfo(Drawing::Surface* drawingSurface,
    const std::shared_ptr<Drawing::Image>& snapshot, std::weak_ptr<IColorPickerManager> manager);

/**
 * @brief Common implementation for extracting snapshot and scheduling color pick.
 *
 * This function handles the common logic for both RSColorPickerManager and ColorPickAltManager:
 * 1. Extract snapshot bounds from canvas using clip rect
 * 2. Get drawing surface from canvas
 * 3. Get image snapshot from surface
 * 4. Try hetero (accelerated) color picker first
 * 5. Fall back to standard color picker with semaphore if hetero fails
 *
 * @param canvas The paint filter canvas to extract from
 * @param rect The rect to extract color from (must not be null)
 * @param manager The color picker manager to handle color updates
 * @return true if scheduling succeeded, false on error
 */
bool ExtractSnapshotAndScheduleColorPick(
    RSPaintFilterCanvas& canvas,
    const Drawing::Rect* rect,
    const std::shared_ptr<IColorPickerManager>& manager);

/**
 * @brief Common implementation for scheduling color pick with semaphore.
 *
 * This function uses the Drawing::FlushInfo.backendSemaphore mechanism to ensure
 * GPU work completes before extracting the color. The ColorPickerInfo is passed
 * with a semaphore that is signaled when GPU work is complete, and a task is posted
 * to the color picker thread to extract the color and call HandleColorUpdate with the result.
 *
 * @param surface The drawing surface to flush
 * @param manager Weak pointer to the color picker manager
 * @param colorPickerInfo A struct containing color space, bitmap format, backend texture, and image
 */
void ScheduleColorPickWithSemaphore(Drawing::Surface& surface, std::weak_ptr<IColorPickerManager> manager,
    std::unique_ptr<ColorPickerInfo> colorPickerInfo);

/**
 * @brief Obtain the corresponding dark color and light color based on the placeholder
 *
 * @param placeholder The placeholder to Get color for
 * @return pair of dark color and light color
 */
std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColorForPlaceholder(ColorPlaceholder placeholder);

/**
 * @brief Interpolate the start color and end color to obtain the color
 *
 * @param start The start color
 * @param end The end color
 * @param fraction The interpolation ratio
 * @return Interpolated color
 */
Drawing::ColorQuad InterpolateColor(Drawing::ColorQuad start, Drawing::ColorQuad end, float fraction);


} // namespace RSColorPickerUtils
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_UTILS_H
