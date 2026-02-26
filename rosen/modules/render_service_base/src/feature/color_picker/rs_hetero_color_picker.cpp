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

#include "feature/color_picker/rs_hetero_color_picker.h"
#ifdef MHC_ENABLE
#include "rs_mhc_manager.h"
#endif
#ifdef RS_ENABLE_VK
#ifndef ROSEN_ARKUI_X
#include "platform/ohos/backend/rs_vulkan_context.h"
#else
#include "rs_vulkan_context.h"
#endif
#endif
#include "draw/surface.h"
#include "image/bitmap.h"

namespace OHOS {
namespace Rosen {

RSHeteroColorPicker& RSHeteroColorPicker::Instance()
{
    static RSHeteroColorPicker instance;
    return instance;
}

namespace {
#if defined(MHC_ENABLE) && defined(RS_ENABLE_VK)
bool RegisterMhcUpdate(const std::function<void(Drawing::ColorQuad&)>& updateColor,
    const std::shared_ptr<Drawing::Image>& imageSnapshot, const Drawing::BackendTexture& backendTexture)
{
    bool registerRes = RSMhcManager::Instance().RegisterFunc([updateColor, imageSnapshot, backendTexture] {
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::TOP_LEFT;
        Drawing::BitmapFormat info =
            Drawing::BitmapFormat { imageSnapshot->GetColorType(), imageSnapshot->GetAlphaType() };
        std::shared_ptr<Drawing::Image> colorImage = std::make_shared<Drawing::Image>();
        std::shared_ptr<Drawing::GPUContext> gpuContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
        if (!gpuContext) {
            RS_LOGE("[HeteroColorPicker]:CreateDrawingContext failed");
            return;
        }
        if (!colorImage->BuildFromTexture(*gpuContext, backendTexture.GetTextureInfo(), origin, info, nullptr)) {
            RS_LOGE("[HeteroColorPicker]:BuildFromTexture failed");
            return;
        }
        Drawing::Bitmap dst;
        dst.Build(colorImage->GetImageInfo());
        if (!colorImage->ReadPixels(dst, 0, 0)) {
            RS_LOGE("[HeteroColorPicker]:ReadPixels failed");
            return;
        }
        auto currentColor = dst.GetColor(0, 0);
        updateColor(currentColor);
    });
    return registerRes;
}
#endif
} // namespace

bool RSHeteroColorPicker::GetColor(const std::function<void(Drawing::ColorQuad&)>& updateColor,
    RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Image>& image)
{
    auto surface = canvas.GetSurface();
    if (!ValidateInputs(updateColor, surface, image)) {
        return false;
    }

    auto colorSurface = surface->MakeSurface(1, 1);
    if (!colorSurface) {
        RS_LOGE("[HeteroColorPicker]:MakeSurface failed");
        return false;
    }

    auto outCanvas = colorSurface->GetCanvas();
    if (!outCanvas) {
        RS_LOGE("[HeteroColorPicker]:GetCanvas failed");
        return false;
    }

    auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
    auto dstRect = Drawing::Rect(0, 0, outCanvas->GetWidth(), outCanvas->GetHeight());
    auto hpsStats =
        std::make_shared<Drawing::HpsStatisticsParameter>(srcRect, dstRect, Drawing::HpsStatisticsType::MEAN);
    std::vector<std::shared_ptr<Drawing::HpsEffectParameter>> hpsParams = { hpsStats };
    if (!outCanvas->DrawImageEffectHPS(*image, hpsParams)) {
        RS_LOGE("[HeteroColorPicker]:DrawImageEffectHPS failed");
        return false;
    }

    auto imageSnapshot = colorSurface->GetImageSnapshot();
    if (!imageSnapshot) {
        RS_LOGE("[HeteroColorPicker]:GetImageSnapshot failed");
        return false;
    }
    auto backendTexture = colorSurface->GetBackendTexture();
    if (!backendTexture.IsValid()) {
        RS_LOGE("[HeteroColorPicker]:GetBackendTexture failed");
        return false;
    }

#if defined(MHC_ENABLE) && defined(RS_ENABLE_VK)
    return RegisterMhcUpdate(updateColor, imageSnapshot, backendTexture);
#else
    RS_LOGE("[HeteroColorPicker]:MHC is disabled");
    return false;
#endif
}

bool RSHeteroColorPicker::ValidateInputs(const std::function<void(Drawing::ColorQuad&)>& updateColor,
    Drawing::Surface* surface, const std::shared_ptr<Drawing::Image>& image)
{
    if (!static_cast<bool>(updateColor)) {
        RS_LOGE("[HeteroColorPicker]:Invalid input updateColor: %d", static_cast<bool>(updateColor));
        return false;
    }
    if (surface == nullptr) {
        RS_LOGE("[HeteroColorPicker]:Invalid input surface");
        return false;
    }
    if (!static_cast<bool>(image)) {
        RS_LOGE("[HeteroColorPicker]:Invalid input image: %d", static_cast<bool>(image));
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
