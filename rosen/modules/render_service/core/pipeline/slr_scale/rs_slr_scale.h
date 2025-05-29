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

#ifndef RS_SLR_SCALE_H
#define RS_SLR_SCALE_H

#include "params/rs_display_render_params.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
const float SLR_SCALE_THR_HIGH = 0.79f;
class RSSLRScaleFunction {
public:
    RSSLRScaleFunction(float dstWidth, float dstHeight, float srcWidth, float srcHeight);
    ~RSSLRScaleFunction() = default;
    void CheckOrRefreshScreen(
        float dstWidth, float dstHeight, float srcWidth, float srcHeight);
    void CanvasScale(RSPaintFilterCanvas& canvas);
    void ProcessCacheImage(RSPaintFilterCanvas& canvas, Drawing::Image& cacheImageProcessed);
    void ProcessOffscreenImage(RSPaintFilterCanvas& canvas, Drawing::Image& offscreenImage);

    bool GetIsSLRCopy() const
    {
        return isSLRCopy_;
    }

    float GetScaleNum() const
    {
        return scaleNum_;
    }

    uint32_t GetSaveCount() const
    {
        return saveCount_;
    }

    Drawing::Matrix GetScaleMatrix() const
    {
        return scaleMatrix_;
    }
    
    int GetKernelSize() const
    {
        return kernelSize_;
    }

    void CheckOrRefreshColorSpace(GraphicColorGamut colorGamut)
    {
        if (colorGamut_ == colorGamut) {
            return;
        }
        RefreshColorSpace(colorGamut);
    }

private:
    void RefreshScreenData();
    void RefreshColorSpace(GraphicColorGamut colorGamut);
    std::shared_ptr<Drawing::ShaderEffect> GetSLRShaderEffect(float coeff, int dstWidth);
    std::shared_ptr<Rosen::Drawing::Image> ProcessSLRImage(RSPaintFilterCanvas& canvas,
        Drawing::Image& cacheImageProcessed);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> SLRImageShaderBuilder(
        const Drawing::Image& image);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> LaplaceShaderBuilder(const Drawing::Image& image);
    static std::shared_ptr<Drawing::RuntimeEffect> MakeSLRShaderEffect();
    static std::shared_ptr<Drawing::RuntimeEffect> MakeLaplaceShaderEffect();
    bool isSLRCopy_ = true;
    float mirrorWidth_;
    float mirrorHeight_;
    float srcWidth_;
    float srcHeight_;
    float dstWidth_ = 1.0f;
    float dstHeight_ = 1.0f;
    float scaleNum_ = 1.0f;
    float alpha_ = 0.15f;
    uint32_t saveCount_ = 0;
    int kernelSize_ = 2;
    Drawing::Matrix scaleMatrix_;
    GraphicColorGamut colorGamut_;
    std::shared_ptr<Drawing::ColorSpace> imageColorSpace_;
    std::shared_ptr<Drawing::ShaderEffect> widthEffect_;
    std::shared_ptr<Drawing::ShaderEffect> heightEffect_;

    static std::shared_ptr<Drawing::RuntimeEffect> slrFilterShader_;
    static std::shared_ptr<Drawing::RuntimeEffect> laplaceFilterShader_;
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> slrShaderBuilder_;
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> laplaceShaderBuilder_;
};
}
}
#endif // RS_SLR_SCALE_H