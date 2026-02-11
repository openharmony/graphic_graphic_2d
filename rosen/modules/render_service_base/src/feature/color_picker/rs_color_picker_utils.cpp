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

#include "feature/color_picker/rs_color_picker_utils.h"

#include "feature/color_picker/i_color_picker_manager.h"
#include "feature/color_picker/rs_color_picker_thread.h"
#include "feature/color_picker/rs_hetero_color_picker.h"

#include "common/rs_common_def.h"
#include "draw/surface.h"
#include "drawable/rs_property_drawable_utils.h"
#include "image/gpu_context.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"

#include "platform/ohos/backend/rs_vulkan_context.h"

#endif

namespace OHOS::Rosen::RSColorPickerUtils {
namespace {
constexpr float RED_LUMINANCE_COEFF = 0.299f;
constexpr float GREEN_LUMINANCE_COEFF = 0.587f;
constexpr float BLUE_LUMINANCE_COEFF = 0.114f;

bool ExecColorPick(const std::weak_ptr<IColorPickerManager>& weakManager, const ColorPickerInfo& colorPickerInfo)
{
    auto manager = weakManager.lock();
    if (!manager) {
        RS_LOGE("ColorPicker: manager is null");
        return false;
    }

    std::shared_ptr<Drawing::GPUContext> gpuCtx;
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    gpuCtx = RSColorPickerThread::Instance().GetShareGPUContext();
#endif
    if (!gpuCtx || !colorPickerInfo.oldImage_) {
        RS_LOGE("ColorPicker: invalid GPU context or image");
        return false;
    }

    auto image = std::make_shared<Drawing::Image>();
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    if (!image->BuildFromTexture(*gpuCtx, colorPickerInfo.backendTexture_.GetTextureInfo(), origin,
        colorPickerInfo.bitmapFormat_, colorPickerInfo.colorSpace_)) {
        RS_LOGE("ColorPicker: BuildFromTexture failed");
        return false;
    }

    Drawing::ColorQuad colorPicked;
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (RSPropertyDrawableUtils::PickColor(gpuCtx, image, colorPicked, colorPickerInfo.waitSemaphore_))
#else
    if (RSPropertyDrawableUtils::PickColor(gpuCtx, image, colorPicked))
#endif
    {
        manager->HandleColorUpdate(colorPicked);
        return true;
    }
    RS_LOGE("ColorPicker: PickColor failed");
    return false;
}
} // namespace

std::unique_ptr<ColorPickerInfo> CreateColorPickerInfo(Drawing::Surface* drawingSurface,
    const std::shared_ptr<Drawing::Image>& snapshot, std::weak_ptr<IColorPickerManager> manager)
{
    if (!drawingSurface || !snapshot) {
        return nullptr;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = snapshot->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        return nullptr;
    }

    auto imageInfo = drawingSurface->GetImageInfo();
    auto colorSpace = imageInfo.GetColorSpace();
    Drawing::BitmapFormat bitmapFormat = { imageInfo.GetColorType(), imageInfo.GetAlphaType() };
    return std::make_unique<ColorPickerInfo>(colorSpace, bitmapFormat, backendTexture, snapshot, manager);
}

float CalculateLuminance(Drawing::ColorQuad color)
{
    auto red = Drawing::Color::ColorQuadGetR(color);
    auto green = Drawing::Color::ColorQuadGetG(color);
    auto blue = Drawing::Color::ColorQuadGetB(color);
    return red * RED_LUMINANCE_COEFF + green * GREEN_LUMINANCE_COEFF + blue * BLUE_LUMINANCE_COEFF;
}

void ScheduleColorPickWithSemaphore(
    Drawing::Surface& surface, std::weak_ptr<IColorPickerManager> weakManager, std::unique_ptr<ColorPickerInfo> info)
{
    if (!info) {
        RS_LOGE("ScheduleColorPickWithSemaphore: ColorPickerInfo is null");
        return;
    }
    Drawing::FlushInfo flushInfo;
    flushInfo.backendSurfaceAccess = true;
    flushInfo.finishedContext = info.release();
    // Work in progress, still using finishedProc callback, semaphore not working
    flushInfo.finishedProc = [](void* ctx) {
        if (!ctx) {
            return;
        }
        RSColorPickerThread::Instance().PostTask([ctx]() {
            auto* colorInfo = reinterpret_cast<ColorPickerInfo*>(ctx);
            ExecColorPick(colorInfo->manager_, *colorInfo);
            delete colorInfo;
        });
    };
    surface.Flush(&flushInfo);
}

std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColorForPlaceholder(ColorPlaceholder placeholder)
{
    static const std::unordered_map<ColorPlaceholder, std::pair<Drawing::ColorQuad, Drawing::ColorQuad>>
        PLACEHOLDER_TO_COLOR {
#define COLOR_PLACEHOLDER_ENTRY(name, dark, light) \
            { ColorPlaceholder::name, { dark, light } },
#include "feature/color_picker/rs_color_placeholder_mapping_def.in"
#undef COLOR_PLACEHOLDER_ENTRY
        };
    if (auto it = PLACEHOLDER_TO_COLOR.find(placeholder); it != PLACEHOLDER_TO_COLOR.end()) {
        return it->second;
    }
    return std::make_pair(Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_WHITE);
}

Drawing::ColorQuad InterpolateColor(Drawing::ColorQuad start, Drawing::ColorQuad end, float fraction)
{
    if (ROSEN_NE(std::clamp(fraction, 0.f, 1.f), fraction)) {
        return end;
    }
    auto lerp = [](uint32_t a, uint32_t b, float t) {
        return static_cast<uint32_t>(static_cast<float>(a) + (static_cast<float>(b) - static_cast<float>(a)) * t);
    };
    auto a =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetA(start), Drawing::Color::ColorQuadGetA(end), fraction));
    auto r =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetR(start), Drawing::Color::ColorQuadGetR(end), fraction));
    auto g =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetG(start), Drawing::Color::ColorQuadGetG(end), fraction));
    auto b =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetB(start), Drawing::Color::ColorQuadGetB(end), fraction));
    return Drawing::Color::ColorQuadSetARGB(a, r, g, b);
}

bool ExtractSnapshotAndScheduleColorPick(
    RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, const std::shared_ptr<IColorPickerManager>& manager)
{
    if (!rect) {
        RS_LOGE("ExtractSnapshotAndScheduleColorPick: rect is null");
        return false;
    }

    canvas.Save();
    canvas.ClipRect(*rect, Drawing::ClipOp::INTERSECT, false);
    Drawing::RectI snapshotIBounds = canvas.GetRoundInDeviceClipBounds();
    canvas.Restore();

    auto drawingSurface = canvas.GetSurface();
    if (!drawingSurface) {
        RS_LOGE("ExtractSnapshotAndScheduleColorPick: surface is null");
        return false;
    }

    auto snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    if (!snapshot) {
        RS_LOGE("ExtractSnapshotAndScheduleColorPick: snapshot is null");
        return false;
    }

    // Try accelerated (hetero) color picker first
    if (RSHeteroColorPicker::Instance().GetColor(
        [manager](Drawing::ColorQuad& newColor) { manager->HandleColorUpdate(newColor); }, canvas, snapshot)) {
        return true;
    }

    // Fall back to standard color picker with semaphore
    auto weakManager = std::weak_ptr<IColorPickerManager>(manager);
    auto colorPickerInfo = CreateColorPickerInfo(drawingSurface, snapshot, weakManager);
    ScheduleColorPickWithSemaphore(*drawingSurface, weakManager, std::move(colorPickerInfo));
    return true;
}

} // namespace OHOS::Rosen::RSColorPickerUtils
