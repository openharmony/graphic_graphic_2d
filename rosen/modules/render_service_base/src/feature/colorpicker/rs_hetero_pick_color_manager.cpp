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

#include "feature/colorpicker/rs_hetero_pick_color_manager.h"
#ifdef MHC_ENABLE
#include "rs_mhc_manager.h"
#endif
#include "draw/surface.h"
#include "image/bitmap.h"

namespace OHOS {
namespace Rosen {

RSHeteroPickColorManager& RSHeteroPickColorManager::Instance()
{
    static RSHeteroPickColorManager instance;
    return instance;
}

bool RSHeteroPickColorManager::GetColor(const std::function<void(Drawing::ColorQuad&)>& updateColor,
    Drawing::Surface* surface, std::shared_ptr<Drawing::Image>& image)
{
    auto colorSurface = surface->MakeSurface(1, 1);
    auto outCanvas = colorSurface->GetCanvas();
    auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
    auto dstRect = Drawing::Rect(0, 0, outCanvas->GetWidth(), outCanvas->GetHeight());
    auto hpsStats = std::make_shared<Drawing::HpsStatisticsParameter>(srcRect, dstRect,
        Drawing::HpsStatisticsType::MEAN);
    std::vector<std::shared_ptr<Drawing::HpsEffectParameter>> hpsParams = { hpsStats };
    if (!outCanvas->DrawImageEffectHPS(*image, hpsParams)) {
        RS_LOGE("[HeteroPickColor]:DrawImageEffectHPS failed");
        return false;
    }
    auto imageSnapshot = colorSurface->GetImageSnapshot();
    if (!imageSnapshot) {
        RS_LOGE("[HeteroPickColor]:GetImageSnapshot failed");
        return false;
    }
    auto backendTexture = colorSurface->GetBackendTexture();

#ifdef MHC_ENABLE
    bool registerRes = RSMhcManager::Instance().RegisterFunc([updateColor, imageSnapshot, backendTexture] {
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::TOP_LEFT;
        Drawing::BitmapFormat info = Drawing::BitmapFormat{ imageSnapshot->GetColorType(),
            imageSnapshot->GetAlphaType() };
        std::shared_ptr<Drawing::Image> colorImage = std::make_shared<Drawing::Image>();
        std::shared_ptr<Drawing::GPUContext> gpuContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
        if (!colorImage->BuildFromTexture(
            *gpuContext,
            backendTexture.GetTextureInfo(),
            origin,
            info,
            nullptr)) {
            RS_LOGE("[HeteroPickColor]:BuildFromTexture failed");
            return;
        }
        Drawing::Bitmap dst;
        dst.Build(colorImage->GetImageInfo());
        if (!colorImage->ReadPixels(dst, 0, 0)) {
            RS_LOGE("[HeteroPickColor]:ReadPixels failed");
            return;
        }
        auto currentColor = dst.GetColor(0, 0);
        updateColor(currentColor);
    });
    return registerRes;
#else
    RS_LOGE("[HeteroPickColor]:MHC is disabled");
    return false;
#endif
}

} // namespace Rosen
} // namespace OHOS

