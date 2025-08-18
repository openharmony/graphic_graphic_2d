/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <string>

#include "rs_frame_blur_predict.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsFrameBlurPredictTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameBlurPredictTest::SetUpTestCase() {}
void RsFrameBlurPredictTest::TearDownTestCase() {}
void RsFrameBlurPredictTest::SetUp() {}
void RsFrameBlurPredictTest::TearDown() {}

/**
 * @tc.name: PredictDrawLargeAreaBlur001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, PredictDrawLargeAreaBlur001, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(true);
    RSRenderNode node = RSRenderNode(0);
    RsFrameBlurPredict::GetInstance().PredictDrawLargeAreaBlur(node);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().RS_BLUR_PREDICT_INVALID, "0");
}

/**
 * @tc.name: PredictDrawLargeAreaBlur002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, PredictDrawLargeAreaBlur002, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(false);
    RSRenderNode node = RSRenderNode(0);
    RsFrameBlurPredict::GetInstance().PredictDrawLargeAreaBlur(node);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().RS_BLUR_PREDICT_LONG, "1");
}
} // namespace Rosen
} // namespace OHOS
