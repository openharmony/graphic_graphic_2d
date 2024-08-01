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

#include "gtest/gtest.h"

#include "rs_window_animation_target.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSWindowAnimationTargetMock : public RSWindowAnimationTarget {
public:
    RSWindowAnimationTargetMock() = default;
    virtual ~RSWindowAnimationTargetMock() = default;
};

class RSWindowAnimationTargetTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSWindowAnimationTarget> windowAnimationTarget_;
};
std::shared_ptr<RSWindowAnimationTarget> RSWindowAnimationStubTest::windowAnimationTarget_ = nullptr;

void RSWindowAnimationTargetTest::SetUpTestCase() {
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> animationSurfaceNode = RSSurfaceNode::Create(config, true);
    std::shared_ptr<RSWindowAnimationTarget> windowAnimationTarget_ = std::make_shared<RSWindowAnimationTarget>();
    windowAnimationTarget_->bundleName_ = "";
    windowAnimationTarget_->abilityName_ = "";
    windowAnimationTarget_->windowBounds_ = RRect();
    windowAnimationTarget_->surfaceNode_ = animationSurfaceNode;
    windowAnimationTarget_->windowId_ = 0;
    windowAnimationTarget_->displayId_ = 0;
    windowAnimationTarget_->missionId_ = 0;
}
void RSWindowAnimationTargetTest::TearDownTestCase() {
    windowAnimationTarget_ = nullptr;
}
void RSWindowAnimationTargetTest::SetUp() {}
void RSWindowAnimationTargetTest::TearDown() {}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationTargetTest, Unmarshalling001, TestSize.Level1)
{
    Parcel parcel;
    auto res = windowAnimationTarget->Unmarshalling(parcel);
    ASSERT_EQ(res, nullptr);
    windowAnimationTarget_->Marshalling(parcel);
    res = windowAnimationTarget_->Unmarshalling(parcel);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationTargetTest, Marshalling001, TestSize.Level1)
{
    Parcel parcel;
    bool res = windowAnimationTarget_->Marshalling(parcel);
    ASSERT_EQ(res, true);
    windowAnimationTarget_->surfaceNode_ = nullptr;
    res = windowAnimationTarget_->Marshalling(parcel);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc: Verify the ReadFromParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationTargetTest, ReadFromParcel001, TestSize.Level1)
{
    Parcel parcel;
    bool res = windowAnimationTarget_->ReadFromParcel(parcel);
    ASSERT_EQ(res, false);
    windowAnimationTarget_->Marshalling(parcel);
    res = windowAnimationTarget_->ReadFromParcel(parcel);
    ASSERT_EQ(res, true);
    
    Parcel parcelNull;
    windowAnimationTarget_->surfaceNode_ = nullptr;
    windowAnimationTarget_->Marshalling(parcelNull);
    res = windowAnimationTarget_->ReadFromParcel(parcelNull);
    ASSERT_EQ(res, true);
}
} // namespace Rosen
} // namespace OHOS