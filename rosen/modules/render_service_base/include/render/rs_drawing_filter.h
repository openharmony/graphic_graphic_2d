/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_DRAWING_RS_DRAWING_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_DRAWING_RS_DRAWING_FILTER_H

#include "draw/brush.h"
#include "draw/canvas.h"
#include "effect/image_filter.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_kawase_blur.h"
#include "render/rs_filter.h"
#include "render/rs_shader_filter.h"

#include "common/rs_color.h"
#include "draw/color.h"
#include "effect/color_filter.h"
#include "effect/color_matrix.h"
#include "property/rs_color_picker_cache_task.h"


namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSDrawingFilter : public RSFilter {
public:
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter);
    RSDrawingFilter(std::shared_ptr<RSShaderFilter> shaderFilter);
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
        std::shared_ptr<RSShaderFilter> shaderFilter);
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
        std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters);
    ~RSDrawingFilter() override;

    Drawing::Brush GetBrush() const;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst);
    std::vector<std::shared_ptr<RSShaderFilter>> GetShaderFilters() const;
    void InsertShaderFilter(std::shared_ptr<RSShaderFilter> shaderFilter);
    std::shared_ptr<Drawing::ImageFilter> GetImageFilter() const;
    void SetImageFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter);
    void SetColorMode(int colorMode)
    {
        colorMode_ = colorMode;
    }
    int GetColorMode()
    {
        return colorMode_;
    }
    void SetMaskCOlor(Color maskColor)
    {
        maskColor_ = maskColor;
    }
    RSColor GetMaskCOlor()
    {
        return maskColor_;
    }
    void SetNeedMaskCOlor(bool needMaskColor)
    {
        needMaskColor_ = needMaskColor;
    }
    std::shared_ptr<RSShaderFilter> FindLinearGradientBlurShaderFilter();

    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter> other) const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<Drawing::ImageFilter> other) const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSShaderFilter> other) const;
    
    bool CanSkipFrame() const { return false; }
    bool CanSkipFrame(float radius) const;
    
    void CaclMaskColor(std::shared_ptr<Drawing::Image>& image);
    void PreProcess(std::shared_ptr<Drawing::Image>& image);
    void PostProcess(RSPaintFilterCanvas& canvas);
    const std::shared_ptr<RSColorPickerCacheTask>& GetColorPickerCacheTask() const;
    void ReleaseColorPickerFilter();
    
private:
    std::shared_ptr<Drawing::ImageFilter> imageFilter_ = nullptr;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters_;
    bool needMaskColor_ = false;
    int colorMode_;
    RSColor maskColor_ = RSColor();
    std::shared_ptr<RSColorPickerCacheTask> colorPickerTask_;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_DRAWING_RS_DRAWING_FILTER_H