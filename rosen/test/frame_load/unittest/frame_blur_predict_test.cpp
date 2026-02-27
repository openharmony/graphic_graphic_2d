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
 * @tc.desc: test PredictDrawLargeAreaBlur with invalid blur frame
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

/**
 * @tc.name: AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively001
 * @tc.desc: test AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively with valid blur frame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively001, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(true);
    RsFrameBlurPredict::GetInstance().predictBegin_.store(true);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first = true;
    RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first, false);
}

/**
 * @tc.name: AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively002
 * @tc.desc: test AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively with invalid blur frame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively002, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(false);
    RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().isValidBlurFrame_.load(), false);
}

/**
 * @tc.name: UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely001
 * @tc.desc: test UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely with valid blur frame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely001, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(true);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second = true;
    RsFrameBlurPredict::GetInstance().UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second, false);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictBegin_.load(), true);
}

/**
 * @tc.name: UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely002
 * @tc.desc: test UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely with invalid blur frame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely002, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(false);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second = true;
    RsFrameBlurPredict::GetInstance().UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second, true);
}

/**
 * @tc.name: TakeEffectBlurScene001
 * @tc.desc: test TakeEffectBlurScene with valid event status
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, TakeEffectBlurScene001, TestSize.Level1)
{
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "TestBlurScene";
    RsFrameBlurPredict::GetInstance().TakeEffectBlurScene(eventInfo);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().isValidBlurFrame_.load(), true);
}

/**
 * @tc.name: TakeEffectBlurScene002
 * @tc.desc: test TakeEffectBlurScene with invalid event status
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, TakeEffectBlurScene002, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().predictBegin_.store(true);
    EventInfo eventInfo;
    eventInfo.eventStatus = false;
    eventInfo.description = "TestBlurScene";
    RsFrameBlurPredict::GetInstance().TakeEffectBlurScene(eventInfo);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().isValidBlurFrame_.load(), false);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictBegin_.load(), false);
}

/**
 * @tc.name: GetCurrentFrameDrawLargeAreaBlurPredictively001
 * @tc.desc: test GetCurrentFrameDrawLargeAreaBlurPredictively with predict begin false
 * @tc.type:FUNC
 * @tc.require: issue
 */
HWTEST_F(RsFrameBlurPredictTest, GetCurrentFrameDrawLargeAreaBlurPredictively001, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().predictBegin_.store(false);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first = true;
    RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first, true);
}

/**
 * @tc.name: GetCurrentFrameDrawLargeAreaBlurPredictively002
 * @tc.desc: test GetCurrentFrameDrawLargeAreaBlurPredictively with predict begin true and long blur
 * @tc.type:FUNC
 * @tc.require: issue
 */
HWTEST_F(RsFrameBlurPredictTest, GetCurrentFrameDrawLargeAreaBlurPredictively002, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().predictBegin_.store(true);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first = true;
    RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first, true);
}

/**
 * @tc.name: GetCurrentFrameDrawLargeAreaBlurPredictively003
 * @tc.desc: test GetCurrentFrameDrawLargeAreaBlurPredictively with predict begin true and short blur
 * @tc.type:FUNC
 * @tc.require: issue
 */
HWTEST_F(RsFrameBlurPredictTest, GetCurrentFrameDrawLargeAreaBlurPredictively003, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().predictBegin_.store(true);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first = false;
    RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first, false);
}

/**
 * @tc.name: GetCurrentFrameDrawLargeAreaBlurPrecisely001
 * @tc.desc: test GetCurrentFrameDrawLargeAreaBlurPrecisely with long blur
 * @tc.type:FUNC
 * @tc.require: issue
 */
HWTEST_F(RsFrameBlurPredictTest, GetCurrentFrameDrawLargeAreaBlurPrecisely001, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second = true;
    RsFrameBlurPredict::GetInstance().UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second, true);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictBegin_.load(), true);
}

/**
 * @tc.name: GetCurrentFrameDrawLargeAreaBlurPrecisely002
 * @tc.desc: test GetCurrentFrameDrawLargeAreaBlurPrecisely with short blur
 * @tc.type:FUNC
 * @tc.require: issue
 */
HWTEST_F(RsFrameBlurPredictTest, GetCurrentFrameDrawLargeAreaBlurPrecisely002, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().predictBegin_.store(true);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second = false;
    RsFrameBlurPredict::GetInstance().UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second, false);
}

/**
 * @tc.name: ResetPredictDrawLargeAreaBlur001
 * @tc.desc: test ResetPredictDrawLargeAreaBlur resets all states
 * @tc.type:FUNC
 * @tc.require: issue
 */
HWTEST_F(RsFrameBlurPredictTest, ResetPredictDrawLargeAreaBlur001, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first = true;
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second = true;
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(true);
    RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first, false);
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second, false);
}

/**
 * @tc.name: PredictDrawLargeAreaBlur003
 * @tc.desc: test PredictDrawLargeAreaBlur with valid frame and node
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, PredictDrawLargeAreaBlur003, TestSize.Level1)
{
    RsFrameBlurPredict::GetInstance().isValidBlurFrame_.store(true);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first = false;
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second = false;
    RSRenderNode node = RSRenderNode(1);
    RsFrameBlurPredict::GetInstance().PredictDrawLargeAreaBlur(node);
    // The prediction state should be updated after processing the node
    EXPECT_TRUE(RsFrameBlurPredict::GetInstance().isValidBlurFrame_.load());
}

/**
 * @tc.name: BlurPredictWorkflow001
 * @tc.desc: test complete workflow of blur prediction
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, BlurPredictWorkflow001, TestSize.Level1)
{
    // Initialize with valid blur scene
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "WorkflowTest";
    RsFrameBlurPredict::GetInstance().TakeEffectBlurScene(eventInfo);
    EXPECT_TRUE(RsFrameBlurPredict::GetInstance().isValidBlurFrame_.load());

    // Predict blur for current frame
    RsFrameBlurPredict::GetInstance().predictBegin_.store(true);
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first = true;
    RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.first, false);

    // Update with precise measurement
    RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second = true;
    RsFrameBlurPredict::GetInstance().UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
    EXPECT_EQ(RsFrameBlurPredict::GetInstance().predictDrawLargeAreaBlur_.second, false);
    EXPECT_TRUE(RsFrameBlurPredict::GetInstance().predictBegin_.load());
}

/**
 * @tc.name: BlurPredictWorkflow002
 * @tc.desc: test workflow with invalid blur scene
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameBlurPredictTest, BlurPredictWorkflow002, TestSize.Level1)
{
    // Initialize with invalid blur scene
    RsFrameBlurPredict::GetInstance().predictBegin_.store(true);
    EventInfo eventInfo;
    eventInfo.eventStatus = false;
    eventInfo.description = "WorkflowTestInvalid";
    RsFrameBlurPredict::GetInstance().TakeEffectBlurScene(eventInfo);
    EXPECT_FALSE(RsFrameBlurPredict::GetInstance().isValidBlurFrame_.load());
    EXPECT_FALSE(RsFrameBlurPredict::GetInstance().predictBegin_.load());
}
} // namespace Rosen
} // namespace OHOS
