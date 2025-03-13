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

#include "draw/canvas.h"
#include "ge_render_fuzzer.h"
#include "get_object.h"
#include "ge_render.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

// fuzz src & dst for DrawImageEffect
bool GERenderFuzzTest001(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    static constexpr int32_t radius = 10;
    visualEffect->SetParam("KAWASE_BLUR_RADIUS", radius);
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;

    float fLeft = GETest::GetPlainData<float>();
    float fTop = GETest::GetPlainData<float>();
    float fWidth = GETest::GetPlainData<float>();
    float fHeight = GETest::GetPlainData<float>();
    Drawing::Rect src{fLeft, fTop, fWidth, fHeight};
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();

    geRender->DrawImageEffect(canvas, *veContainer, image, src, dst, sampling);
    return true;
}

// fuzz src & dst for ApplyImageEffect
std::shared_ptr<Drawing::Image> GERenderFuzzTest002(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;
    
    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;

    float fLeft = GETest::GetPlainData<float>();
    float fTop = GETest::GetPlainData<float>();
    float fWidth = GETest::GetPlainData<float>();
    float fHeight = GETest::GetPlainData<float>();
    Drawing::Rect src{fLeft, fTop, fWidth, fHeight};
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();

    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, image, src, dst, sampling);
    return resImg;
}

std::shared_ptr<Drawing::Image> GERenderFuzzTest003(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    FuzzedDataProvider fdp(data, size);

    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::shared_ptr<Drawing::Image> image = nullptr;
    Drawing::Rect src{0.0, 0.0, 100.0, 100.0};
    Drawing::Rect dst{0.0, 0.0, 100.0, 100.0};
    Drawing::SamplingOptions sampling;
    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, image, src, dst, sampling);
    return resImg;
}

std::shared_ptr<Drawing::Image> GERenderFuzzTest004(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    auto geRender = std::make_shared<GERender>();
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto visualEffectGrey = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    auto visualEffectAIBar = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_AI_BAR);
    auto visualEffectLinear = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);

    visualEffectGrey->SetParam("GREY_COEF_1", 0.0f);
    visualEffectGrey->SetParam("GREY_COEF_2", 0.0f);

    Drawing::Matrix matrix;
    std::vector<std::pair<float, float>> fractionStops = {{0.0, 0.0}, {1.0, 1.0}};
    visualEffectLinear->SetParam("BLURRADIUS", 0.0f);
    visualEffectLinear->SetParam("GEOWIDTH", 0.0f);
    visualEffectLinear->SetParam("GEOHEIGHT", 0.0f);
    visualEffectLinear->SetParam("TRANX", 0.0f);
    visualEffectLinear->SetParam("TRANY", 0.0f);
    visualEffectLinear->SetParam("CANVASMAT", matrix);
    visualEffectLinear->SetParam("FRACTIONSTOPS", fractionStops);
    visualEffectLinear->SetParam("DIRECTION", 0);
    visualEffectLinear->SetParam("ISOFFSCREEN", true);

    veContainer->AddToChainedFilter(visualEffectGrey);
    veContainer->AddToChainedFilter(visualEffectAIBar);
    veContainer->AddToChainedFilter(visualEffectLinear);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src = GETest::GetPlainData<Drawing::Rect>();
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();
    Drawing::SamplingOptions sampling;
    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, image, src, dst, sampling);
    return resImg;
}

std::shared_ptr<Drawing::Image> GERenderFuzzTest005(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    auto geRender = std::make_shared<GERender>();
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto visualEffectMagnifier = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MAGNIFIER);

    visualEffectMagnifier->SetParam("FACTOR", 0.0f);
    visualEffectMagnifier->SetParam("WIDTH", 0.0f);
    visualEffectMagnifier->SetParam("HEIGHT", 0.0f);
    visualEffectMagnifier->SetParam("CORNERRADIUS", 0.0f);
    visualEffectMagnifier->SetParam("BORDERWIDTH", 0.0f);
    visualEffectMagnifier->SetParam("SHADOWOFFSETX", 0.0f);
    visualEffectMagnifier->SetParam("SHADOWOFFSETY", 0.0f);
    visualEffectMagnifier->SetParam("SHADOWSIZE", 0.0f);
    visualEffectMagnifier->SetParam("SHADOWSTRENGTH", 0.0f);
    visualEffectMagnifier->SetParam("GRADIENTMASKCOLOR1", 0x00000000);
    visualEffectMagnifier->SetParam("GRADIENTMASKCOLOR2", 0x00000000);
    visualEffectMagnifier->SetParam("OUTERCONTOURCOLOR1", 0x00000000);
    visualEffectMagnifier->SetParam("OUTERCONTOURCOLOR2", 0x00000000);
    visualEffectMagnifier->SetParam("ROTATEDEGREE", 0);

    veContainer->AddToChainedFilter(visualEffectMagnifier);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src = GETest::GetPlainData<Drawing::Rect>();
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();
    Drawing::SamplingOptions sampling;

    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, image, src, dst, sampling);
    return resImg;
}

std::shared_ptr<Drawing::Image> GERenderFuzzTest006(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    auto geRender = std::make_shared<GERender>();
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto visualEffectWaterRipple = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_WATER_RIPPLE);
    auto visualEffectMesa = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR);

    visualEffectWaterRipple->SetParam("PROGRESS", 0.0f);
    visualEffectWaterRipple->SetParam("WAVE_NUM", 0);
    visualEffectWaterRipple->SetParam("RIPPLE_CENTER_X", 0.0f);
    visualEffectWaterRipple->SetParam("RIPPLE_CENTER_Y", 0.0f);
    visualEffectWaterRipple->SetParam("RIPPLE_MODE", 0);

    visualEffectMesa->SetParam("MESA_BLUR_RADIUS", 0);
    visualEffectMesa->SetParam("MESA_BLUR_GREY_COEF_1", 0.0f);
    visualEffectMesa->SetParam("MESA_BLUR_GREY_COEF_2", 0.0f);
    visualEffectMesa->SetParam("OFFSET_X", 0.f);
    visualEffectMesa->SetParam("OFFSET_Y", 0.f);
    visualEffectMesa->SetParam("OFFSET_Z", 0.f);
    visualEffectMesa->SetParam("OFFSET_W", 0.f);
    visualEffectMesa->SetParam("TILE_MODE", 0);
    visualEffectMesa->SetParam("WIDTH", 0.f);
    visualEffectMesa->SetParam("HEIGHT", 0.f);

    veContainer->AddToChainedFilter(visualEffectWaterRipple);
    veContainer->AddToChainedFilter(visualEffectMesa);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src = GETest::GetPlainData<Drawing::Rect>();
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();
    Drawing::SamplingOptions sampling;

    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, image, src, dst, sampling);
    return resImg;
}

}  // namespace GraphicsEffectEngine
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GraphicsEffectEngine::GERenderFuzzTest001(data, size);
    OHOS::GraphicsEffectEngine::GERenderFuzzTest002(data, size);
    OHOS::GraphicsEffectEngine::GERenderFuzzTest003(data, size);
    OHOS::GraphicsEffectEngine::GERenderFuzzTest004(data, size);
    OHOS::GraphicsEffectEngine::GERenderFuzzTest005(data, size);
    OHOS::GraphicsEffectEngine::GERenderFuzzTest006(data, size);
    return 0;
}
