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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_IMAGE_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_IMAGE_RENDER_MODIFIER_H

#include "modifier_ng/rs_render_modifier_ng.h"

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "external_window.h"
#include "surface_buffer.h"
#endif

#ifdef RS_ENABLE_VK
namespace OHOS::Rosen::NativeBufferUtils {
class VulkanCleanupHelper;
}
#endif

namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSBackgroundImageRenderModifier : public RSRenderModifier {
public:
    RSBackgroundImageRenderModifier() = default;
    ~RSBackgroundImageRenderModifier() override;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BACKGROUND_IMAGE;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    }

    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;

    void ResetProperties(RSProperties& properties) override;

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }

    bool OnApply(RSModifierContext& context) override;
    void OnSync() override;
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    static Drawing::ColorType GetColorTypeFromVKFormat(VkFormat vkFormat);
    std::shared_ptr<Drawing::Image> MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer);
    void ReleaseNativeWindowBuffer();
    void SetCompressedDataForASTC();
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
    pid_t tid_ = 0;
    uint32_t pixelMapId_ = 0;
    Drawing::BackendTexture backendTexture_ = {};
    NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper_ = nullptr;
#endif

    std::shared_ptr<RSImage> stagingBgImage_ = nullptr;
    Drawing::Rect stagingBoundsRect_;
    std::shared_ptr<RSImage> renderBgImage_ = nullptr;
    Drawing::Rect renderBoundsRect_;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_IMAGE_RENDER_MODIFIER_H
