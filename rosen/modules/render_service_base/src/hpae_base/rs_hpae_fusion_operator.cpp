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

#include "hpae_base/rs_base_fusion_operator.h"
#include "hpae_base/rs_hpae_log.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_maskcolor_shader_filter.h"

namespace OHOS::Rosen {

Drawing::Matrix BuildShaderMatrix(const Drawing::Rect &src, float greyScaleRatio, float stretchOffsetX,
    float stretchOffseeY, float stretchOffsetZ, float stretchOffsetW)
{
    Drawing::Matrix matrix;
    float scalW = (1.0 - stretchOffsetX - stretchOffsetZ) * greyScaleRatio;
    float scalH = (1.0 - stretchOffsetY - stretchOffsetW) * greyScaleRatio;

    matirx.PostScale(scaleW, scaleH);

    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(
        stretchOffsetX * greyScaleRatio * src.GetWidth(), stretchOffsetY * greyScaleRatio * src.GetHeight());
    matrix.PostConcat(translateMatrix);

    return matrix;
}

std::shared_ptr<Drawing::RuntimeEffect> greyShaderEffect_ = nullptr;
std::shared_ptr<Drawing::ShadeEffect> MakeGreyShader(
    float greyLow, float greyHeight, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform shader imageShader;
        uniform float coefficient1;
        uniform float coefficient2;

        float poww(float x, float y) {
            return (x < 0) ? -pow(-x, y) : pow(x, y);
        }

        float calculateT_y(float rgb) {
            if (rgb > 127.5) { rgb = 255 - rgb; }
            float b = 38.0;
            float c = 45.0;
            float d = 127.5;
            float A = 106.5;    // 3 * b - 3 * c d;
            float B = -93;      // 3 * (c - 2 * b);
            float C = 114;      // 3 * b;
            float p = 0.816240163988;        // (3 * A * C - pow(B, 2)) / (3 * pow(A, 2));
            float q = -rgb / 106.5 + 0.262253485943;        // -rgb/A - B * C/(3*pow(A, 2)) + 2 * pow(B, 3)/(27*pow(A,3))
            float s1 = -(q / 2.0);
            float s2 = sqrt(pow(s1, 2) + pow(p / 3, 3));
            return poww((s1 + s2), 1.0 / 3) + poww((s1 - s2), 1.0 / 3) - (8 / (3 * A));     
        }

        float calculateGreyAdjustY(float rgb) {
            float t_r = calculateT_y(rgb);
            return (rgb < 127.5) ? (rgb + coefficient * pow((1 - t_r), 3)) :
                (rgb - coefficient2 * pow((1 - t_r), 3));
        }

        half4 main(float2 coord) {
            vec3 color = vec3(imageShader.eval(coord).r, imageShader.eval(coord).g, imageShader.eval(coord).b);
            float Y = (0.229 * color.r + 0.587 * color.g + 0.114 * color.b) * 255;
            float U = (-0.147 * color.r - 0.289 * color.g + 0.436 * color.b) * 255;
            float V = (0.615 * color.r - 0.515 * color.g - 0.100 * color.b) * 255;
            Y = calculateGreyAdjustY(Y);
            color.r = (Y + 1.14 * V) / 255.0;
            color.g = (Y - 0.39 * U - 0.58 * V) / 255.0;
            color.b = (Y + 2.03 * U) / 255.0;

            return vec4(color, 1.0);
        }
    )";

    // parameter check: near zero
    constexpr static float EPS = 1e-5f;
    if (abs(geryLow) < EPS && abs(greyHigh) < EPS) {
        HPAE_LOGW("MakeGreyShader: grey value is invalid!");
        return imageShader;
    }

    if (greyShaderEffect_ == nullptr) {
        greyShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (greyShaderEffect_ == nullptr) {
            HPAE_LOGE("MakeGreyShader: blurEffect create failed");
            return nullptr;
        }
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(greyShaderEffect_);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("coefficient1", greyLow);
    builder->SetUniform("coefficient2", greyHeight);
    return builder->MakeShader(nullptr, false);
}

HaePixel RSHpaeFunsionOperator::GetHpaePixel(const std::shared_ptr<RSDrawingFilter> &filter)
{
    HaePixel haePixel;
    if (filter) {
        auto maskColorShadeFilter = std::static_pointer_cast<RSMaskColorShaderFilter>(
            filter->GetShaderFilterWithType(RSShaderFilter::MASK_COLOR));
        if (maskColorShadeFilter) {
            RSColor maskColors = maskColorShaderFilter->GetMaskColor();
            haePixel.a = static_cast<uint64_t>(maskColors.GetAlpha()) * 4;
            haePixel.r = static_cast<uint64_t>(maskColors.GetRed()) * 4;
            haePixel.g = static_cast<uint64_t>(maskColors.GetGreen()) * 4;
            haePixel.b = static_cast<uint64_t>(maskColors.GetBlue()) * 4;
        }
    }

    return haePixel;
}

// 对清晰背景image进行灰阶和边缘像素拓展处理
// 结果写到aae blur的1/4输入buffer上
int RSHpaeFunsionOperator::ProcessGreyAndStretch(const Drawing::RectI& clipBounds,
    const std::shared_ptr<Drawing::Image> &image, const HpaeBufferInfo &targetBuffer,
    const std::shared_ptr<RSDrawingFilter> &filter, const Drawing::RectI &src)
{
    // 通过GPU将结果绘制到该canva上
    auto targetCanvas = targetBuffer.canvas;
    if (!image || !targetCanvas) {
        HPAE_LOGE("ProcessGreyAndStretch: input param is null: image: %p, canvas: %p", image.get(), targetCanvas.get());
        return -1;
    }

    float greyScaleRatio = 1.0 * targetCanvas->GetWidth() / src.GetWidth();
    auto pixelStretch = RSHpaeBaseData::GetInstance().GetPixelStretch();
    constexpr static float EPS = 1e-5f;
    // The pixel stretch is fuzed only when the stretch factors are negative
    if (pixelStretch.x_ > EPS || pixelStretch.y_ > EPS || pixelStretch.z_ > EPS || pixelStretch.w_ > EPS) {
        pixelStretch = Vector4f(0.f, 0.f, 0.f, 0.f);
    }
    auto shaderMatrix = BuildShaderMatrix(src,
        greyScaleRatio,
        std::abs(pixelStretch.x_) / image->GetWidth(),
        std::abs(pixelStretch.y_) / image->GetHeight(),
        std::abs(pixelStretch.z_) / image->GetWidth(),
        std::abs(pixelStretch.w_) / image->GetHeight());

    auto tileMode = static_cast<Drawing::TileMode>(RSHpaeBaseData::GetInstance().GetTileMode());
    HPAE_TRACE_NAME_FMT("RSHpaeFilterCacheManager::ProcessGreyAndStretch. offset:[%f, %f, %f], tileMode:%d, image:[%dx%d]",
        pixelStretch.x_, pixelStretch.y_, pixelStretch.z_, pixelStretch.w_, tileMode,
        image->GetWidth(), image->GetHeight());
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, tileMode, tileMode, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), shaderMatrix);
    auto greyCoef = RSHpaeBaseData::GetInstance().GetGreyCoef();
    auto shader = MakeGreyShader(greyCoef.x_, greyCoef.y_, imageShader);
    if (shader == nullptr) {
        HPAE_LOGE("ProcessGreyAndStretch: shader is null!");
        return -1;
    }

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    Drawing::AutoCanvasRestore acr(targetCanvas, true);
    targetCanvas->ResetMatrix();
    // targetCanvas->Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    targetCanvas->DrawBackground(brush);

    return 0;
}

void RSHpaeFusionOperator::GetColorMatrixCoef(const std::shared_ptr<RSDrawingFilter>& filter,
    float (&colorMatrix)[HAE_COLOR_MATRIX_COEF_COUNT])
{
    float brightnessForHpae = RSHpaeBaseData::GetInstance().GetBrightness();
    float saturationForHpae = RSHpaeBaseData::GetInstance().GetSaturation();
    float normalizedDegree = brightnessForHpae - 1.0;
    Drawing::ColorMatrix cm;
    cm.SetSaturation(saturationForHpae);
    cm.GetArray(colorMatrix);
    colorMatrix[Drawing::ColorMatrix::TRANS_FOR_R] += normalizedDegree;
    colorMatrix[Drawing::ColorMatrix::TRANS_FOR_G] += normalizedDegree;
    colorMatrix[Drawing::ColorMatrix::TRANS_FOR_B] += normalizedDegree;
}

Drawing::Matrix RSHpaeFusionOperator::GetShaderTransform(const Drawing::Rect& blurRect, float scaleW, float scaleH)
{
    Drawing::Matrix martrix;
    matrix.SetScale(scaleW, scaleH);
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(blurRect.GetLeft(), blurRect.GetTop());
    matrix.PostConcat(translateMatrix);
    return matrix;
}

} // OHOS::Rosen