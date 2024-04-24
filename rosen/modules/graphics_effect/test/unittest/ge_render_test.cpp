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

#include <gtest/gtest.h>

#include "ge_render.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GERenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GERenderTest::SetUpTestCase(void) {}
void GERenderTest::TearDownTestCase(void) {}

void GERenderTest::SetUp() {}
void GERenderTest::TearDown() {}

/**
 * @tc.name: DrawImageEffect001
 * @tc.desc: Verify the DrawImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>("KAWASE_BLUR");
    visualEffect->SetParam("KAWASE_BLUR_RADIUS", 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    Drawing::Canvas canvas;
    const std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    geRender->DrawImageEffect(canvas, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect001 end";
}

/**
 * @tc.name: ApplyImageEffect001
 * @tc.desc: Verify the ApplyImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyImageEffect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>("KAWASE_BLUR");
    visualEffect->SetParam("KAWASE_BLUR_RADIUS", 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    Drawing::Canvas canvas;
    const std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    auto outImage = geRender->ApplyImageEffect(canvas, *veContainer, image, src, dst, sampling);
    EXPECT_TRUE(outImage == image);

    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect001 end";
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
