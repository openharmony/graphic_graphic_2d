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

#include "animation/rs_animation_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class AnimationCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AnimationCallbackTest::SetUpTestCase() {}
void AnimationCallbackTest::TearDownTestCase() {}
void AnimationCallbackTest::SetUp() {}
void AnimationCallbackTest::TearDown() {}

/**
 * @tc.name: Execute001
 * @tc.desc: Verify the Execute of AnimationFinishCallback
 * @tc.type:FUNC
 */
HWTEST_F(AnimationCallbackTest, Execute001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationCallbackTest Marshalling001 start";
    bool flag = false;
    std::function<void()> func = [&]() { flag = true; };
    auto finishCallback = std::make_shared<AnimationFinishCallback>(func);
    finishCallback->Execute();
    EXPECT_TRUE(flag);

    std::function<void()> func1 = nullptr;
    auto finishCallback1 = std::make_shared<AnimationFinishCallback>(func1);
    finishCallback1->Execute();
    EXPECT_TRUE(finishCallback1 != nullptr);
    GTEST_LOG_(INFO) << "AnimationCallbackTest Execute001 end";
}

/**
 * @tc.name: Execute002
 * @tc.desc: Verify the Execute of AnimationRepeatCallback
 * @tc.type:FUNC
 */
HWTEST_F(AnimationCallbackTest, Execute002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationCallbackTest Marshalling001 start";
    bool flag = false;
    std::function<void()> func = [&]() { flag = true; };
    auto finishCallback = std::make_shared<AnimationRepeatCallback>(func);
    finishCallback->Execute();
    EXPECT_TRUE(flag);

    std::function<void()> func1 = nullptr;
    auto finishCallback1 = std::make_shared<AnimationRepeatCallback>(func1);
    finishCallback1->Execute();
    EXPECT_TRUE(finishCallback1 != nullptr);
    GTEST_LOG_(INFO) << "AnimationCallbackTest Execute002 end";
}
} // namespace Rosen
} // namespace OHOS
