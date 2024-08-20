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
};

void RSWindowAnimationTargetTest::SetUpTestCase() {}
void RSWindowAnimationTargetTest::TearDownTestCase() {}
void RSWindowAnimationTargetTest::SetUp() {}
void RSWindowAnimationTargetTest::TearDown() {}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationTargetTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationTargetTest Unmarshalling001 start";
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> animationSurfaceNode = RSSurfaceNode::Create(config, true);
    std::shared_ptr<RSWindowAnimationTarget> windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
    windowAnimationTarget->bundleName_ = "";
    windowAnimationTarget->abilityName_ = "";
    windowAnimationTarget->windowBounds_ = RRect();
    windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
    windowAnimationTarget->windowId_ = 0;
    windowAnimationTarget->displayId_ = 0;
    windowAnimationTarget->missionId_ = 0;

    Parcel parcel;
    auto res = windowAnimationTarget->Unmarshalling(parcel);
    ASSERT_EQ(res, nullptr);
    windowAnimationTarget->Marshalling(parcel);
    res = windowAnimationTarget->Unmarshalling(parcel);
    ASSERT_NE(res, nullptr);
    GTEST_LOG_(INFO) << "RSWindowAnimationTargetTest Unmarshalling001 end";
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationTargetTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationTargetTest Marshalling001 start";
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> animationSurfaceNode = RSSurfaceNode::Create(config, true);
    std::shared_ptr<RSWindowAnimationTarget> windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
    windowAnimationTarget->bundleName_ = "";
    windowAnimationTarget->abilityName_ = "";
    windowAnimationTarget->windowBounds_ = RRect();
    windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
    windowAnimationTarget->windowId_ = 0;
    windowAnimationTarget->displayId_ = 0;
    windowAnimationTarget->missionId_ = 0;

    Parcel parcel;
    bool res = windowAnimationTarget->Marshalling(parcel);
    ASSERT_EQ(res, true);
    windowAnimationTarget->surfaceNode_ = nullptr;
    res = windowAnimationTarget->Marshalling(parcel);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "RSWindowAnimationTargetTest Marshalling001 end";
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc: Verify the ReadFromParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationTargetTest, ReadFromParcel001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationTargetTest ReadFromParcel001 start";
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> animationSurfaceNode = RSSurfaceNode::Create(config, true);
    std::shared_ptr<RSWindowAnimationTarget> windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
    windowAnimationTarget->bundleName_ = "";
    windowAnimationTarget->abilityName_ = "";
    windowAnimationTarget->windowBounds_ = RRect();
    windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
    windowAnimationTarget->windowId_ = 0;
    windowAnimationTarget->displayId_ = 0;
    windowAnimationTarget->missionId_ = 0;

    Parcel parcel;
    bool res = windowAnimationTarget->ReadFromParcel(parcel);
    ASSERT_EQ(res, false);
    windowAnimationTarget->Marshalling(parcel);
    res = windowAnimationTarget->ReadFromParcel(parcel);
    ASSERT_EQ(res, true);
    
    Parcel parcelNull;
    windowAnimationTarget->surfaceNode_ = nullptr;
    windowAnimationTarget->Marshalling(parcelNull);
    res = windowAnimationTarget->ReadFromParcel(parcelNull);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "RSWindowAnimationTargetTest ReadFromParcel001 end";
}
} // namespace Rosen
} // namespace OHOS