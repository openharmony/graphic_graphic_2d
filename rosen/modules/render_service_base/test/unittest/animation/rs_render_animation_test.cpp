/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderAnimationMock : public RSRenderAnimation {
public:
    static constexpr uint64_t ANIMATION_ID = 12345;
    RSRenderAnimationMock() : RSRenderAnimationMock(ANIMATION_ID) {}
    ~RSRenderAnimationMock() = default;
    void Pause()
    {
        RSRenderAnimation::Pause();
    }
    void Resume()
    {
        RSRenderAnimation::Resume();
    }
    void FinishOnCurrentPosition()
    {
        RSRenderAnimation::FinishOnCurrentPosition();
    }

protected:
    explicit RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
};

class RSRenderAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderAnimationTest::SetUpTestCase() {}
void RSRenderAnimationTest::TearDownTestCase() {}
void RSRenderAnimationTest::SetUp() {}
void RSRenderAnimationTest::TearDown() {}

/**
 * @tc.name: Pause001
 * @tc.desc: Verify the Pause
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Pause001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Pause001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->Pause();
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Pause001 end";
}

/**
 * @tc.name: Resume001
 * @tc.desc: Verify the Resume
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Resume001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Resume001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->Resume();
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Resume001 end";
}

/**
 * @tc.name: FinishOnCurrentPosition001
 * @tc.desc: Verify the FinishOnCurrentPosition
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnCurrentPosition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPosition001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->FinishOnCurrentPosition();
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPosition001 end";
}

/**
 * @tc.name: IsFinished001
 * @tc.desc: Verify the IsFinished
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, IsFinished001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsFinished001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    auto result = renderAnimation->IsFinished();
    EXPECT_TRUE(result == false);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsFinished001 end";
}

/**
 * @tc.name: SetReversed001
 * @tc.desc: Verify the SetReversed
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, SetReversed001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest SetReversed001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->SetReversed(true);
    renderAnimation->Start();
    renderAnimation->SetReversed(true);
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest SetReversed001 end";
}
} // namespace Rosen
} // namespace OHOS
