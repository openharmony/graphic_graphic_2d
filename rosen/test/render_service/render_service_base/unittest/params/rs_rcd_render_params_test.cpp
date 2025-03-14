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

#include "gtest/gtest.h"
#include "params/rs_rcd_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRcdRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSRcdRenderParamsTest::SetUpTestCase() {}
void RSRcdRenderParamsTest::TearDownTestCase() {}
void RSRcdRenderParamsTest::SetUp() {}
void RSRcdRenderParamsTest::TearDown() {}
void RSRcdRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSRcdRenderParams params(id);
    params.OnSync(target);
    EXPECT_EQ(target, nullptr);
}

/**
 * @tc.name: OnSync002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdRenderParamsTest, OnSync002, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRcdRenderParams>(id);
    auto targetRcdParams = static_cast<RSRcdRenderParams*>(target.get());
    RSRcdRenderParams params(id);
    params.pathBin_ = "test.bin";
    params.bufferSize_ = 10000;
    params.cldWidth_ = 100;
    params.cldHeight_ = 100;
    params.srcRect_ = RectI(0, 0, 100, 100);
    params.dstRect_ = RectI(0, 0, 100, 100);
    params.OnSync(target);
    EXPECT_EQ(params.pathBin_, targetRcdParams->pathBin_);
    EXPECT_EQ(params.bufferSize_, targetRcdParams->bufferSize_);
    EXPECT_EQ(params.cldWidth_, targetRcdParams->cldWidth_);
    EXPECT_EQ(params.cldHeight_, targetRcdParams->cldHeight_);
    EXPECT_EQ(params.srcRect_, targetRcdParams->srcRect_);
    EXPECT_EQ(params.dstRect_, targetRcdParams->dstRect_);
}

/**
 * @tc.name: SetterGetterTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdRenderParamsTest, SetterGetterTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSRcdRenderParams params(id);
    params.SetPathBin("test.bin");
    EXPECT_EQ(params.GetPathBin(), "test.bin");
    params.SetBufferSize(100);
    EXPECT_EQ(params.GetBufferSize(), 100);
    params.SetCldWidth(100);
    EXPECT_EQ(params.GetCldWidth(), 100);
    params.SetCldHeight(100);
    EXPECT_EQ(params.GetCldHeight(), 100);
    params.SetSrcRect(RectI(0, 0, 100, 100));
    EXPECT_EQ(params.GetSrcRect(), RectI(0, 0, 100, 100));
    params.SetDstRect(RectI(0, 0, 100, 100));
    EXPECT_EQ(params.GetDstRect(), RectI(0, 0, 100, 100));
}

} // namespace OHOS::Rosen