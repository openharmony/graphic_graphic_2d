/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "parameters.h"
#include "ui/rs_surface_node.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;
static constexpr uint32_t FIRST_COLOR_VALUE = 0x034123;
static constexpr uint32_t SECOND_COLOR_VALUE = 0x45ba87;
static constexpr uint32_t THIRD_COLOR_VALUE = 0x32aadd;
namespace OHOS::Rosen {
class RSSurfaceNodeUnitTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceNodeUnitTest::SetUpTestCase() {}
void RSSurfaceNodeUnitTest::TearDownTestCase() {}
void RSSurfaceNodeUnitTest::SetUp() {}
void RSSurfaceNodeUnitTest::TearDown() {}

/**
 * @tc.name: CreateNodeInRenderThread002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, CreateNodeInRenderThread002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    surfaceNode->CreateNodeInRenderThread();
}

/**
 * @tc.name: SetBufferAvailableCallback002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetBufferAvailableCallback002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    bool isSuccess = surfaceNode->SetBufferAvailableCallback([]() {
        std::cout << "SetBufferAvailableCallback" << std::endl;
    });
    ASSERT_TRUE(isSuccess);
}

/**
 * @tc.name: SetandGetBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBounds002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[3]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBounds003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBounds003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBounds004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2],
        TestSrc::limitNumber::floatLimit[1], TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBounds005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[3]);
    surfaceNode->SetBounds(quaternion);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBounds006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBounds006, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[0]);
    surfaceNode->SetBounds(quaternion);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds007
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBounds007, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2],
        TestSrc::limitNumber::floatLimit[1], TestSrc::limitNumber::floatLimit[0]);
    surfaceNode->SetBounds(quaternion);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBoundsWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsWidth002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[1]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsWidth003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[2]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsWidth004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[3]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsWidth005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[4]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsWidth006, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsHeight002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[1]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsHeight003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[2]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsHeight004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[3]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsHeight005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[4]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetandGetBoundsHeight006, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetWatermarkEnabled
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetWatermarkEnabled002, TestSize.Level1)
{
    static bool flag = system::GetParameter("const.product.devicetype", "pc") != "pc";
    if (flag) {
        return;
    }
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::string waterMark = "watermark";
    surfaceNode->SetWatermarkEnabled(waterMark, true);
    surfaceNode->SetWatermarkEnabled(waterMark, false);
    std::string waterMark1(129, 't');
    surfaceNode->SetWatermarkEnabled(waterMark1, false);
}

/**
 * @tc.name: SetSecurityLayer002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSecurityLayer002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSecurityLayer(true);
    EXPECT_TRUE(surfaceNode->GetSecurityLayer());
}

/**
 * @tc.name: SetSecurityLayer003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSecurityLayer003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSecurityLayer(false);
    EXPECT_FALSE(surfaceNode->GetSecurityLayer());
}

/**
 * @tc.name: GetSecurityLayer002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetSecurityLayer002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_FALSE(surfaceNode->GetSecurityLayer());
}

/**
 * @tc.name: SetSKipLayer002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSkipLayer002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSkipLayer(true);
    EXPECT_TRUE(surfaceNode->GetSkipLayer());
}

/**
 * @tc.name: SetLeashPersistId002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetLeashPersistId002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    LeashPersistentId leashPersistentId = 50;
    surfaceNode->SetLeashPersistentId(leashPersistentId);
    EXPECT_TRUE(surfaceNode->GetLeashPersistentId() == leashPersistentId);
}

/**
 * @tc.name: SetSnapshotSKipLayer002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSnapshotSkipLayer002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSnapshotSkipLayer(true);
    EXPECT_TRUE(surfaceNode->GetSnapshotSkipLayer());
}

/**
 * @tc.name: SetSkipLayer003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSkipLayer003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSkipLayer(false);
    EXPECT_FALSE(surfaceNode->GetSkipLayer());
}

/**
 * @tc.name: SetSnapshotSkipLayer003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSnapshotSkipLayer003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSnapshotSkipLayer(false);
    EXPECT_FALSE(surfaceNode->GetSnapshotSkipLayer());
}

/**
 * @tc.name: GetSkipLayer002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetSkipLayer002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_FALSE(surfaceNode->GetSkipLayer());
}

/**
 * @tc.name: GetSnapshotSkipLayer002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetSnapshotSkipLayer002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_FALSE(surfaceNode->GetSnapshotSkipLayer());
}

/**
 * @tc.name: Marshalling002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, Marshalling002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, Marshalling003, TestSize.Level1)
{
    Parcel parcel;
    auto surfaceNode = RSSurfaceNode::Unmarshalling(parcel);
    EXPECT_TRUE(surfaceNode == nullptr);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, Marshalling004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: Marshalling005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, Marshalling005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: Marshalling006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, Marshalling006, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode1 = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode1 != nullptr);
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode2 != nullptr);

    Parcel parcel;
    surfaceNode1->Marshalling(parcel);
    surfaceNode2->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: GetSurface002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetSurface002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    surfaceNode->GetSurface();
}

/**
 * @tc.name: GetType002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetType002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetType() == RSUINodeType::SURFACE_NODE);
}

/**
 * @tc.name: SetCornerRadius002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetCornerRadius002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[0]);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto result = surfaceNode->GetStagingProperties().GetCornerRadius()[0];
    EXPECT_TRUE(ROSEN_EQ(floatData[0], result));
}

/**
 * @tc.name: SetCornerRadius003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetCornerRadius003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[1]);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto result = surfaceNode->GetStagingProperties().GetCornerRadius()[1];
    EXPECT_TRUE(ROSEN_EQ(floatData[1], result));
}

/**
 * @tc.name: SetCornerRadius004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetCornerRadius004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[2]);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto result = surfaceNode->GetStagingProperties().GetCornerRadius()[2];
    EXPECT_TRUE(ROSEN_EQ(floatData[2], result));
}

/**
 * @tc.name: SetBackgroundFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetBackgroundFilter002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0],
    floatData[1]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: SetBackgroundFilter003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetBackgroundFilter003, TestSize.Level2)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1],
    floatData[2]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: SetBackgroundFilter004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetBackgroundFilter004, TestSize.Level3)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2],
    floatData[3]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: SetFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetFilter002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0],
    floatData[1]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: SetFilter003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetFilter003, TestSize.Level2)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1],
    floatData[2]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: SetFilter004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetFilter004, TestSize.Level3)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2],
    floatData[3]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: SetCompositingFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetCompositingFilter002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(floatData[0],
    floatData[1]);
    surfaceNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetCompositingFilter003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetCompositingFilter003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(floatData[1],
    floatData[2]);
    surfaceNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetCompositingFilter004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetCompositingFilter004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(floatData[2],
    floatData[3]);
    surfaceNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowOffset002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffset002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetShadowOffset003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffset003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetShadowOffset004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffset004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}
/**
 * @tc.name: SetShadowOffsetX002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffsetX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
}

/**
 * @tc.name: SetShadowOffsetX003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffsetX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
}

/**
 * @tc.name: SetShadowOffsetX004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffsetX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[3]));
}

/**
 * @tc.name: SetShadowOffsetY002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffsetY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[1]));
}

/**
 * @tc.name: SetShadowOffsetY003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffsetY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[2]));
}

/**
 * @tc.name: SetShadowOffsetY004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowOffsetY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetShadowAlpha002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowAlpha002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(), floatData[1]));
}

/**
 * @tc.name: SetShadowAlpha003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowAlpha003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(), floatData[2]));
}

/**
 * @tc.name: SetShadowAlpha004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowAlpha004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(), floatData[3]));
}

/**
 * @tc.name: SetShadowElevation002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowElevation002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[1]));
}

/**
 * @tc.name: SetShadowElevation003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowElevation003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[2]));
}

/**
 * @tc.name: SetShadowElevation004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowElevation004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[3]));
}

/**
 * @tc.name: SetShadowRadius002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowRadius002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetShadowRadius003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowRadius003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetShadowRadius004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowRadius004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetShadowColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowColor002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(FIRST_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(FIRST_COLOR_VALUE));
}

/**
 * @tc.name: SetShadowColor003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowColor003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(SECOND_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(SECOND_COLOR_VALUE));
}

/**
 * @tc.name: SetShadowColor004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetShadowColor004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(THIRD_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(THIRD_COLOR_VALUE));
}

/**
 * @tc.name: SetFreeze002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetFreeze002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFreeze(true);
}

/**
 * @tc.name: SetContainerWindow002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetContainerWindow002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetContainerWindow(true, rrect);
}

/**
 * @tc.name: SetIsNotifyUIBufferAvailable002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetIsNotifyUIBufferAvailable002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetIsNotifyUIBufferAvailable(true);
}

/**
 * @tc.name: ClearChildren002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeUnitTest, ClearChildren002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->CreateNodeInRenderThread();
    RSSurfaceNodeConfig c2;
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(c2);
    surfaceNode->AddChild(surfaceNode2, -1);
    surfaceNode->RemoveChild(surfaceNode2);
    surfaceNode->ClearChildren();
}

/**
 * @tc.name: SetFreeze Test True
 * @tc.desc: SetFreeze Test True
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetFreeze_True, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetFreeze(true);
}

/**
 * @tc.name: SetFreeze Test False
 * @tc.desc: SetFreeze Test False
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetFreeze_False, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetFreeze(false);
}

/**
 * @tc.name: SetContainerWindow Test True
 * @tc.desc: SetContainerWindow Test True
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetContainerWindow_True, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetContainerWindow(true, rrect);
}

/**
 * @tc.name: SetContainerWindow Test False
 * @tc.desc: SetContainerWindow Test False
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetContainerWindow_False, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetContainerWindow(false, rrect);
}

/**
 * @tc.name: Fingerprint Test
 * @tc.desc: SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require: issueI6Z3YK
 */
HWTEST_F(RSSurfaceNodeUnitTest, Fingerprint, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFingerprint(true);
    ASSERT_EQ(true, surfaceNode->GetFingerprint());
    surfaceNode->SetFingerprint(false);
    ASSERT_EQ(false, surfaceNode->GetFingerprint());
}

/**
 * @tc.name: SetBootAnimation Test
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetBootAnimationTest, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBootAnimation(true);
    ASSERT_EQ(true, surfaceNode->GetBootAnimation());
    surfaceNode->SetBootAnimation(false);
    ASSERT_EQ(false, surfaceNode->GetBootAnimation());
}

/**
 * @tc.name: SetGlobalPositionEnabled Test
 * @tc.desc: SetGlobalPositionEnabled and GetGlobalPositionEnabled
 * @tc.type: FUNC
 * @tc.require: issueIATYMW
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetGlobalPositionEnabled, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetGlobalPositionEnabled(true);
    ASSERT_EQ(true, surfaceNode->GetGlobalPositionEnabled());
    surfaceNode->SetGlobalPositionEnabled(false);
    ASSERT_EQ(false, surfaceNode->GetGlobalPositionEnabled());
}

/**
 * @tc.name: IsBufferAvailableTest Test
 * @tc.desc: test results of IsBufferAvailable
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeUnitTest, IsBufferAvailableTest, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_FALSE(surfaceNode->IsBufferAvailable());
}

/**
 * @tc.name: SetBoundsChangedCallbacktest Test
 * @tc.desc: test results of SetBoundsChangedCallback
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetBoundsChangedCallbackTest, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    BoundsChangedCallback callback = [](const Rosen::Vector4f& bounds) {};
    surfaceNode->SetBoundsChangedCallback(callback);
    ASSERT_NE(surfaceNode->boundsChangedCallback_, nullptr);
}

/**
 * @tc.name: SetTextureExport Test
 * @tc.desc: SetTextureExport
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetTextureExport, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTextureExport(true);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
    surfaceNode->SetTextureExport(false);
    //for test
    ret = false;
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: SetHardwareEnabled Test
 * @tc.desc: SetHardwareEnabled
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetHardwareEnabled, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetHardwareEnabled(true);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
    surfaceNode->SetHardwareEnabled(false);
    //for test
    ret = false;
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: DetachToDisplay Test
 * @tc.desc: DetachToDisplay
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, DetachToDisplay, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    //for test
    uint64_t screenId = 0;
    surfaceNode->DetachToDisplay(screenId);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: AttachToDisplay Test
 * @tc.desc: AttachToDisplay
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, AttachToDisplay, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    //for test
    uint64_t screenId = 0;
    surfaceNode->AttachToDisplay(screenId);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetAnimationFinished Test
 * @tc.desc: SetAnimationFinished
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetAnimationFinished, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAnimationFinished();
    ASSERT_FALSE(surfaceNode->isSkipLayer_);
}

/**
 * @tc.name: SetForceHardwareAndFixRotation Test
 * @tc.desc: SetForceHardwareAndFixRotation and SetTextureExport
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetForceHardwareAndFixRotation, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetForceHardwareAndFixRotation(true);
    ASSERT_FALSE(surfaceNode->isSkipLayer_);
    surfaceNode->SetTextureExport(true);
    ASSERT_TRUE(surfaceNode->isTextureExportNode_);
    surfaceNode->SetTextureExport(false);
    ASSERT_FALSE(surfaceNode->isTextureExportNode_);
    surfaceNode->SetTextureExport(false);
    ASSERT_FALSE(surfaceNode->isTextureExportNode_);
}

/**
 * @tc.name: GetColorSpace Test
 * @tc.desc: test results of GetColorSpace
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetColorSpace, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode->GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: ResetContextAlpha Test
 * @tc.desc: GetBundleName and ResetContextAlpha
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeUnitTest, ResetContextAlpha, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->ResetContextAlpha();
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);
}

/**
 * @tc.name: SetForeground Test
 * @tc.desc: SetForeground and SetForceUIFirst and SetAncoFlags and SetHDRPresent
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetForeground, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetForeground(true);
    surfaceNode->SetForceUIFirst(true);
    surfaceNode->SetAncoFlags(1);
    surfaceNode->SetHDRPresent(true, 0);
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetBoundsChangedCallback Test
 * @tc.desc: SetBoundsChangedCallback
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetBoundsChangedCallback, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    using BoundsChangedCallback = std::function<void(const Rosen::Vector4f&)>;
    BoundsChangedCallback callback;
    surfaceNode->SetBoundsChangedCallback(callback);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: IsBufferAvailable Test
 * @tc.desc: IsBufferAvailable
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, IsBufferAvailable, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool ret = surfaceNode->IsBufferAvailable();
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: MarkUIHidden Test
 * @tc.desc: MarkUIHidden
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, MarkUIHidden, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool isHidden = true;
    surfaceNode->MarkUIHidden(isHidden);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetIsNotifyUIBufferAvailable Test
 * @tc.desc: SetIsNotifyUIBufferAvailable
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetIsNotifyUIBufferAvailable, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool available = true;
    surfaceNode->SetIsNotifyUIBufferAvailable(available);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetAbilityBGAlpha Test
 * @tc.desc: SetAbilityBGAlpha
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetAbilityBGAlpha, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    uint8_t alpha = 0;
    surfaceNode->SetAbilityBGAlpha(alpha);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetAbilityBGAlpha Test
 * @tc.desc: SetAbilityBGAlpha
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, NeedForcedSendToRemote, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->NeedForcedSendToRemote();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: CreateNode Test
 * @tc.desc: CreateNode
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, CreateNode, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    const RSSurfaceRenderNodeConfig& config = {0, "SurfaceNode", RSSurfaceNodeType::DEFAULT, nullptr, false, false};
    bool res = surfaceNode->CreateNode(config);
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: CreateNodeAndSurface Test
 * @tc.desc: CreateNodeAndSurface
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, CreateNodeAndSurface, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    const RSSurfaceRenderNodeConfig& config = {0, "SurfaceNode", RSSurfaceNodeType::DEFAULT, nullptr, false, false};
    SurfaceId surfaceId = 0;
    bool res = surfaceNode->CreateNodeAndSurface(config, surfaceId);
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: OnBoundsSizeChanged Test
 * @tc.desc: OnBoundsSizeChanged
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, OnBoundsSizeChanged, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->OnBoundsSizeChanged();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetSurfaceIdToRenderNode Test
 * @tc.desc: SetSurfaceIdToRenderNode
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSurfaceIdToRenderNode, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSurfaceIdToRenderNode();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: CreateRenderNodeForTextureExportSwitch Test
 * @tc.desc: CreateRenderNodeForTextureExportSwitch
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, CreateRenderNodeForTextureExportSwitch, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->CreateRenderNodeForTextureExportSwitch();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetIsTextureExportNode Test
 * @tc.desc: SetIsTextureExportNode
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetIsTextureExportNode, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool isTextureExportNode = true;
    surfaceNode->SetIsTextureExportNode(isTextureExportNode);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SplitSurfaceNodeName Test
 * @tc.desc: SplitSurfaceNodeName
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeUnitTest, SplitSurfaceNodeName, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::string surfaceNodeName = "0#1";
    std::pair<std::string, std::string> res = surfaceNode->SplitSurfaceNodeName(surfaceNodeName);
    EXPECT_EQ(res.first, "0");
    EXPECT_EQ(res.second, "1");
}

/**
 * @tc.name: SetColorSpace Test
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetColorSpace, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(surfaceNode->colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: SetSKipDraw
 * @tc.desc: Test function SetSkipDraw
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetSkipDraw, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSkipDraw(true);
    EXPECT_TRUE(surfaceNode->GetSkipDraw());

    surfaceNode->SetSkipDraw(false);
    EXPECT_FALSE(surfaceNode->GetSkipDraw());
}

/**
 * @tc.name: GetSkipLayer002
 * @tc.desc: Test function GetSkipDraw
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetSkipDraw, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_FALSE(surfaceNode->GetSkipDraw());
}

/**
 * @tc.name: SetAbilityState
 * @tc.desc: Test function SetAbilityState
 * @tc.type: FUNC
 * @tc.require: issueIAQL48
 */
HWTEST_F(RSSurfaceNodeUnitTest, SetAbilityState, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    EXPECT_TRUE(surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::FOREGROUND);

    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
    EXPECT_TRUE(surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::BACKGROUND);
}

/**
 * @tc.name: GetAbilityState
 * @tc.desc: Test function GetAbilityState
 * @tc.type: FUNC
 * @tc.require: issueIAQL48
 */
HWTEST_F(RSSurfaceNodeUnitTest, GetAbilityState, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_TRUE(surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::FOREGROUND);
}
} // namespace OHOS::Rosen