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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_text.h"

#include "effect/shader_effect.h"
#include "render/rs_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ForegroundTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, GradientMask_Test_1)
{
    auto testFaNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 10, 10, 1000, 1000 });
    Drawing::Brush gradientBrush = Drawing::Brush();
    auto effect = Drawing::ShaderEffect::CreateLinearGradient(Drawing::PointF(0.0, 0.0),
        Drawing::PointF(1000.0, 1000.0), { 0xffffffff, 0x00ffffff }, { 0.0, 1.0 }, Drawing::TileMode::CLAMP);
    gradientBrush.SetShaderEffect(effect);
    auto mask = RSMask::CreateGradientMask(gradientBrush);
    testFaNode->SetMask(mask);
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testFaNode);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, PixelMapMask_Test_1)
{
    auto testFaNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 10, 10, 1000, 1000 });
    auto mask = RSMask::CreatePixelMapMask(
        DecodePixelMap("/data/local/tmp/fg_mask.jpg", Media::AllocatorType::SHARE_MEM_ALLOC));
    testFaNode->SetMask(mask);
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testFaNode);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, PathMask_Test_1)
{
    auto testFaNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 10, 10, 1000, 1000 });
    Drawing::Brush brush = Drawing::Brush();
    brush.SetColor(0xffffffff);
    Drawing::Path path = Drawing::Path();
    path.AddRect(Drawing::Rect(50, 50, 900, 900));
    auto mask = RSMask::CreatePathMask(path, brush);
    testFaNode->SetMask(mask);
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testFaNode);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, PathMask_Test_2)
{
    float radiusList[] = { 500.0f, 600.0f };
    for (int i = 0; i < 2; i++) {
        int y = i * 1020;
        auto testFaNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 10, y, 1000, 1000 });
        Drawing::Brush brush = Drawing::Brush();
        brush.SetColor(0xffffffff);
        Drawing::Path path = Drawing::Path();
        path.AddCircle(500.f, 500.f, radiusList[i]);
        auto mask = RSMask::CreatePathMask(path, brush);
        testFaNode->SetMask(mask);
        GetRootNode()->AddChild(testFaNode);
        RegisterNode(testFaNode);
    }
}
} // namespace OHOS::Rosen