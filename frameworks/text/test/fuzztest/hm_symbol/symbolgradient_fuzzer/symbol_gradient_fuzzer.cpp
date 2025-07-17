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

#include <cstddef>
#include <fuzzer/FuzzedDataProvider.h>
#include "rosen_text/symbol_gradient.h"
#include "symbol_gradient_fuzzer.h"

namespace OHOS {
namespace Rosen {
void SymbolGradientFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    // Simulation input color data
    uint32_t color1 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color2 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color3 = fdp.ConsumeIntegral<uint32_t>();
    float p1 = fdp.ConsumeFloatingPoint<float>();
    float p2 = fdp.ConsumeFloatingPoint<float>();
    float p3 = fdp.ConsumeFloatingPoint<float>();
    Drawing::TileMode tileMode = static_cast<Drawing::TileMode>(fdp.ConsumeIntegral<uint8_t>());
    SymbolGradient gradient = SymbolGradient();
    std::vector<Drawing::ColorQuad> colors = {color1, color2, color3};
    std::vector<float> positions = {p1, p2, p3};
    Drawing::Point offset = Drawing::Point(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    gradient.CreateGradientShader(offset);
    gradient.CreateGradientBrush();
    gradient.CreateGradientPen();
    gradient.SetColors(colors);
    gradient.SetPositions(positions);
    gradient.SetTileMode(tileMode);

    float left = fdp.ConsumeFloatingPoint<float>();
    float right = fdp.ConsumeFloatingPoint<float>();
    float top = fdp.ConsumeFloatingPoint<float>();
    float bottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect bounds = Drawing::Rect(left, top, right, bottom);
    gradient.Make(bounds);
    gradient.CreateGradientBrush();
    gradient.CreateGradientPen();
    gradient.CreateGradientShader(offset);

    SymbolGradient gradient2 = SymbolGradient();
    gradient.GetGradientType();
    gradient2.SetColors(gradient.GetColors());
    gradient2.SetPositions(gradient.GetPositions());
    gradient2.SetTileMode(gradient.GetTileMode());
}

void SymbolLineGradientFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    // Simulation input color data
    uint32_t color1 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color2 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color3 = fdp.ConsumeIntegral<uint32_t>();
    float p1 = fdp.ConsumeFloatingPoint<float>();
    float p2 = fdp.ConsumeFloatingPoint<float>();
    float p3 = fdp.ConsumeFloatingPoint<float>();
    Drawing::TileMode tileMode = static_cast<Drawing::TileMode>(fdp.ConsumeIntegral<uint8_t>());
    float angle = fdp.ConsumeFloatingPoint<float>();
    std::vector<Drawing::ColorQuad> colors = {color1, color2, color3};
    std::vector<float> positions = {p1, p2, p3};

    SymbolLineGradient gradient = SymbolLineGradient(angle);
    Drawing::Point offset = Drawing::Point(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    gradient.CreateGradientBrush(offset);
    gradient.CreateGradientPen(offset);
    gradient.SetColors(colors);
    gradient.SetPositions(positions);
    gradient.SetTileMode(tileMode);

    float left = fdp.ConsumeFloatingPoint<float>();
    float right = fdp.ConsumeFloatingPoint<float>();
    float top = fdp.ConsumeFloatingPoint<float>();
    float bottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect bounds = Drawing::Rect(left, top, right, bottom);
    gradient.Make(bounds);
    gradient.GetAngle();
    gradient.CreateGradientBrush(offset);
    gradient.CreateGradientPen(offset);
}

void SymbolRadialGradientFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    // Simulation input color data
    uint32_t color1 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color2 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color3 = fdp.ConsumeIntegral<uint32_t>();
    float p1 = fdp.ConsumeFloatingPoint<float>();
    float p2 = fdp.ConsumeFloatingPoint<float>();
    float p3 = fdp.ConsumeFloatingPoint<float>();
    Drawing::TileMode tileMode = static_cast<Drawing::TileMode>(fdp.ConsumeIntegral<uint8_t>());
    float radiusRatio = fdp.ConsumeFloatingPoint<float>();
    float left = fdp.ConsumeFloatingPoint<float>();
    float right = fdp.ConsumeFloatingPoint<float>();
    float top = fdp.ConsumeFloatingPoint<float>();
    float bottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect bounds = Drawing::Rect(left, top, right, bottom);
    Drawing::Point offset = Drawing::Point(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    std::vector<Drawing::ColorQuad> colors = {color1, color2, color3};
    std::vector<float> positions = {p1, p2, p3};
    Drawing::Point centerPt = Drawing::Point(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());

    SymbolRadialGradient gradient = SymbolRadialGradient(centerPt, radiusRatio);
    gradient.CreateGradientBrush(offset);
    gradient.CreateGradientPen(offset);
    gradient.SetColors(colors);
    gradient.SetPositions(positions);
    gradient.SetTileMode(tileMode);
    gradient.SetCenterPoint(centerPt);
    gradient.SetRadiusRatio(radiusRatio);
    gradient.Make(bounds);
    gradient.GetRadius();
    gradient.CreateGradientBrush(offset);
    gradient.CreateGradientPen(offset);
    float radius = fdp.ConsumeFloatingPoint<float>();
    gradient.SetRadius(radius);
    gradient.Make(bounds);
    gradient.GetRadiusRatio();
    gradient.GetCenterPoint();
}

std::shared_ptr<SymbolGradient> SymbolGradientFuzzTestGet(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    // Simulation input color data
    uint32_t color1 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color2 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t color3 = fdp.ConsumeIntegral<uint32_t>();
    float p1 = fdp.ConsumeFloatingPoint<float>();
    float p2 = fdp.ConsumeFloatingPoint<float>();
    float p3 = fdp.ConsumeFloatingPoint<float>();
    Drawing::TileMode tileMode = static_cast<Drawing::TileMode>(fdp.ConsumeIntegral<uint8_t>());
    std::vector<Drawing::ColorQuad> colors = {color1, color2, color3};
    std::vector<float> positions = {p1, p2, p3};
    uint16_t gradientIndes = fdp.ConsumeIntegral<uint16_t>();
    Drawing::Point centerPt = Drawing::Point(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());

    std::shared_ptr<SymbolGradient> gradient = nullptr;
    GradientType gradientType = static_cast<GradientType>(gradientIndes);
    switch (gradientType) {
        case GradientType::LINE_GRADIENT:
            gradient = std::make_shared<SymbolLineGradient>(fdp.ConsumeFloatingPoint<float>());
            break;
        case GradientType::RADIAL_GRADIENT:
            gradient = std::make_shared<SymbolRadialGradient>(centerPt, fdp.ConsumeFloatingPoint<float>());
            break;
        default:
            gradient = std::make_shared<SymbolGradient>();
    }
    gradient->SetColors(colors);
    gradient->SetPositions(positions);
    gradient->SetTileMode(tileMode);
    return gradient;
}

void SymbolGradientFuzzTestNearlyEqual(const uint8_t* data, size_t size)
{
    std::shared_ptr<SymbolGradient> gradient1 = SymbolGradientFuzzTestGet(data, size);
    std::shared_ptr<SymbolGradient> gradient2 = SymbolGradientFuzzTestGet(data, size);

    gradient1->IsNearlyEqual(gradient2);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::SymbolGradientFuzzTest(data, size);
    OHOS::Rosen::SymbolLineGradientFuzzTest(data, size);
    OHOS::Rosen::SymbolRadialGradientFuzzTest(data, size);
    OHOS::Rosen::SymbolGradientFuzzTestNearlyEqual(data, size);
    return 0;
}
