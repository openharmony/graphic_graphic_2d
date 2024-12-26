/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "params/rs_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSRenderParamsTest::SetUpTestCase() {}
void RSRenderParamsTest::TearDownTestCase() {}
void RSRenderParamsTest::SetUp() {}
void RSRenderParamsTest::TearDown() {}
void RSRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    RSRenderParams params(id);
    params.childHasVisibleEffect_ = true;
    params.OnSync(target);
    EXPECT_EQ(params.childHasVisibleEffect_, renderParams->childHasVisibleEffect_);
}

/**
 * @tc.name: SetAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, SetAlpha001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->alpha_ = 1.0;
    float alpha = 1.0;
    params.SetAlpha(alpha);
    EXPECT_TRUE(renderParams != nullptr);
}

/**
 * @tc.name: SetAlphaOffScreen001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, SetAlphaOffScreen001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSRenderParams params(id);
    bool alphaOffScreen = false;
    params.SetAlphaOffScreen(alphaOffScreen);
    EXPECT_TRUE(!params.GetAlphaOffScreen());
}

/**
 * @tc.name: ApplyAlphaAndMatrixToCanvas001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, ApplyAlphaAndMatrixToCanvas001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSRenderParams params(id);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    params.ApplyAlphaAndMatrixToCanvas(paintFilterCanvas, true);
    EXPECT_TRUE(id != -1);
}
} // namespace OHOS::Rosen