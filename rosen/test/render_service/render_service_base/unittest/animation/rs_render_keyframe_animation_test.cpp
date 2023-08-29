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
#include "animation/rs_render_keyframe_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderKeyframeAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderKeyframeAnimationTest::SetUpTestCase() {}
void RSRenderKeyframeAnimationTest::TearDownTestCase() {}
void RSRenderKeyframeAnimationTest::SetUp() {}
void RSRenderKeyframeAnimationTest::TearDown() {}

/**
 * @tc.name: AddKeyframeTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, AddKeyframeTest, Level1)
{
    auto originValue = std::shared_ptr<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    auto value = std::shared_ptr<RSRenderPropertyBase>();
    auto interpolator = std::shared_ptr<RSInterpolator>();
    rsRenderKeyframeAnimation.AddKeyframe(1.0f, value, interpolator);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, MarshallingTest, Level1)
{
    auto originValue = std::shared_ptr<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    Parcel parcel;
    bool res = rsRenderKeyframeAnimation.Marshalling(parcel);
    ASSERT_NE(res, false);
}
}