/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_timing_protocol.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSAnimationTimingProtocolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationTimingProtocolTest::SetUpTestCase() {}
void RSAnimationTimingProtocolTest::TearDownTestCase() {}
void RSAnimationTimingProtocolTest::SetUp() {}
void RSAnimationTimingProtocolTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Cover branch: all Write operations succeed (return true)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling001 start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetStartDelay(100);
    protocol.SetSpeed(1.5f);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);

    Parcel parcel;
    bool result = RSMarshallingHelper::Marshalling(parcel, protocol);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling001 end";
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Cover branch: test with default values
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Marshalling002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling002 start";
    RSAnimationTimingProtocol protocol; // Use default values

    Parcel parcel;
    bool result = RSMarshallingHelper::Marshalling(parcel, protocol);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling002 end";
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Cover branch: test with edge values (INT32_MAX, 0.0f, etc.)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Marshalling003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling003 start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(INT32_MAX);
    protocol.SetStartDelay(INT32_MAX);
    protocol.SetSpeed(0.0f);
    protocol.SetRepeatCount(0);
    protocol.SetAutoReverse(false);

    Parcel parcel;
    bool result = RSMarshallingHelper::Marshalling(parcel, protocol);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling003 end";
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Cover branch: test with negative values
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Marshalling004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling004 start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(-1);
    protocol.SetStartDelay(-1);
    protocol.SetSpeed(-1.0f);
    protocol.SetRepeatCount(-1);
    protocol.SetAutoReverse(false);

    Parcel parcel;
    bool result = RSMarshallingHelper::Marshalling(parcel, protocol);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling004 end";
}

/**
 * @tc.name: Marshalling005
 * @tc.desc: Cover branch: test with large speed value
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Marshalling005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling005 start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetStartDelay(0);
    protocol.SetSpeed(FLT_MAX);
    protocol.SetRepeatCount(1);
    protocol.SetAutoReverse(true);

    Parcel parcel;
    bool result = RSMarshallingHelper::Marshalling(parcel, protocol);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling005 end";
}

/**
 * @tc.name: Marshalling006
 * @tc.desc: Attempt to test Marshalling failure (limited by Parcel implementation)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Marshalling006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling006 start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetStartDelay(100);
    protocol.SetSpeed(1.5f);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);

    Parcel parcel;
    // Note: In current Parcel implementation, Write operations always return true
    bool result = RSMarshallingHelper::Marshalling(parcel, protocol);
    EXPECT_TRUE(result);  // Will succeed in current environment

    // To truly test the failure path, would need:
    // 1. A Mock Parcel implementation that can return false on Write
    // 2. Or a Parcel with capacity limits enforced
    // 3. Or memory allocation failure simulation
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Marshalling006 end (Note: failure path not triggerable)";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Cover branch: all Read operations succeed (return true)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling001 start";
    // First, marshal a protocol
    RSAnimationTimingProtocol protocolOriginal;
    protocolOriginal.SetDuration(1000);
    protocolOriginal.SetStartDelay(100);
    protocolOriginal.SetSpeed(1.5f);
    protocolOriginal.SetRepeatCount(2);
    protocolOriginal.SetAutoReverse(true);

    Parcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, protocolOriginal);
    ASSERT_TRUE(marshalResult);

    // Reset parcel read position
    parcel.RewindRead(0);

    // Now unmarshal
    RSAnimationTimingProtocol protocolUnmarshalled;
    bool result = RSMarshallingHelper::Unmarshalling(parcel, protocolUnmarshalled);
    EXPECT_TRUE(result);
    EXPECT_EQ(protocolUnmarshalled.GetDuration(), 1000);
    EXPECT_EQ(protocolUnmarshalled.GetStartDelay(), 100);
    EXPECT_FLOAT_EQ(protocolUnmarshalled.GetSpeed(), 1.5f);
    EXPECT_EQ(protocolUnmarshalled.GetRepeatCount(), 2);
    EXPECT_EQ(protocolUnmarshalled.GetAutoReverse(), true);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling001 end";
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Cover branch: test with default values
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Unmarshalling002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling002 start";
    RSAnimationTimingProtocol protocolOriginal; // Use default values

    Parcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, protocolOriginal);
    ASSERT_TRUE(marshalResult);

    parcel.RewindRead(0);

    RSAnimationTimingProtocol protocolUnmarshalled;
    bool result = RSMarshallingHelper::Unmarshalling(parcel, protocolUnmarshalled);
    EXPECT_TRUE(result);
    EXPECT_EQ(protocolUnmarshalled.GetDuration(), 300); // default value
    EXPECT_EQ(protocolUnmarshalled.GetStartDelay(), 0);
    EXPECT_FLOAT_EQ(protocolUnmarshalled.GetSpeed(), 1.0f); // default value
    EXPECT_EQ(protocolUnmarshalled.GetRepeatCount(), 1); // default value
    EXPECT_EQ(protocolUnmarshalled.GetAutoReverse(), false); // default value

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling002 end";
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: Cover branch: test with edge values (INT32_MAX, 0.0f, etc.)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Unmarshalling003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling003 start";
    RSAnimationTimingProtocol protocolOriginal;
    protocolOriginal.SetDuration(INT32_MAX);
    protocolOriginal.SetStartDelay(INT32_MAX);
    protocolOriginal.SetSpeed(0.0f);
    protocolOriginal.SetRepeatCount(0);
    protocolOriginal.SetAutoReverse(false);

    Parcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, protocolOriginal);
    ASSERT_TRUE(marshalResult);

    parcel.RewindRead(0);

    RSAnimationTimingProtocol protocolUnmarshalled;
    bool result = RSMarshallingHelper::Unmarshalling(parcel, protocolUnmarshalled);
    EXPECT_TRUE(result);
    EXPECT_EQ(protocolUnmarshalled.GetDuration(), INT32_MAX);
    EXPECT_EQ(protocolUnmarshalled.GetStartDelay(), INT32_MAX);
    EXPECT_FLOAT_EQ(protocolUnmarshalled.GetSpeed(), 0.0f);
    EXPECT_EQ(protocolUnmarshalled.GetRepeatCount(), 0);
    EXPECT_EQ(protocolUnmarshalled.GetAutoReverse(), false);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling003 end";
}

/**
 * @tc.name: Unmarshalling004
 * @tc.desc: Cover branch: test with negative values
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Unmarshalling004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling004 start";
    RSAnimationTimingProtocol protocolOriginal;
    protocolOriginal.SetDuration(-1);
    protocolOriginal.SetStartDelay(-1);
    protocolOriginal.SetSpeed(-1.0f);
    protocolOriginal.SetRepeatCount(-1);
    protocolOriginal.SetAutoReverse(false);

    Parcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, protocolOriginal);
    ASSERT_TRUE(marshalResult);

    parcel.RewindRead(0);

    RSAnimationTimingProtocol protocolUnmarshalled;
    bool result = RSMarshallingHelper::Unmarshalling(parcel, protocolUnmarshalled);
    EXPECT_TRUE(result);
    EXPECT_EQ(protocolUnmarshalled.GetDuration(), -1);
    EXPECT_EQ(protocolUnmarshalled.GetStartDelay(), -1);
    EXPECT_FLOAT_EQ(protocolUnmarshalled.GetSpeed(), -1.0f);
    EXPECT_EQ(protocolUnmarshalled.GetRepeatCount(), -1);
    EXPECT_EQ(protocolUnmarshalled.GetAutoReverse(), false);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling004 end";
}

/**
 * @tc.name: Unmarshalling005
 * @tc.desc: Cover branch: test with large speed value
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Unmarshalling005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling005 start";
    RSAnimationTimingProtocol protocolOriginal;
    protocolOriginal.SetDuration(1000);
    protocolOriginal.SetStartDelay(0);
    protocolOriginal.SetSpeed(FLT_MAX);
    protocolOriginal.SetRepeatCount(1);
    protocolOriginal.SetAutoReverse(true);

    Parcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, protocolOriginal);
    ASSERT_TRUE(marshalResult);

    parcel.RewindRead(0);

    RSAnimationTimingProtocol protocolUnmarshalled;
    bool result = RSMarshallingHelper::Unmarshalling(parcel, protocolUnmarshalled);
    EXPECT_TRUE(result);
    EXPECT_EQ(protocolUnmarshalled.GetDuration(), 1000);
    EXPECT_EQ(protocolUnmarshalled.GetStartDelay(), 0);
    EXPECT_FLOAT_EQ(protocolUnmarshalled.GetSpeed(), FLT_MAX);
    EXPECT_EQ(protocolUnmarshalled.GetRepeatCount(), 1);
    EXPECT_EQ(protocolUnmarshalled.GetAutoReverse(), true);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling005 end";
}

/**
 * @tc.name: Unmarshalling006
 * @tc.desc: Cover branch: Read operations fail (empty parcel, return false)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, Unmarshalling006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling006 start";
    Parcel parcel; // Empty parcel, no data to read

    RSAnimationTimingProtocol protocol;
    bool result = RSMarshallingHelper::Unmarshalling(parcel, protocol);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest Unmarshalling006 end";
}

/**
 * @tc.name: MarshallingUnmarshalling001
 * @tc.desc: Cover branch: round-trip test (marshal then unmarshal)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, MarshallingUnmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest MarshallingUnmarshalling001 start";
    RSAnimationTimingProtocol protocolOriginal;
    protocolOriginal.SetDuration(500);
    protocolOriginal.SetStartDelay(50);
    protocolOriginal.SetSpeed(2.0f);
    protocolOriginal.SetRepeatCount(3);
    protocolOriginal.SetAutoReverse(false);

    Parcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, protocolOriginal);
    ASSERT_TRUE(marshalResult);

    parcel.RewindRead(0);

    RSAnimationTimingProtocol protocolUnmarshalled;
    bool unmarshalResult = RSMarshallingHelper::Unmarshalling(parcel, protocolUnmarshalled);
    EXPECT_TRUE(unmarshalResult);

    // Verify all fields match
    EXPECT_EQ(protocolOriginal.GetDuration(), protocolUnmarshalled.GetDuration());
    EXPECT_EQ(protocolOriginal.GetStartDelay(), protocolUnmarshalled.GetStartDelay());
    EXPECT_FLOAT_EQ(protocolOriginal.GetSpeed(), protocolUnmarshalled.GetSpeed());
    EXPECT_EQ(protocolOriginal.GetRepeatCount(), protocolUnmarshalled.GetRepeatCount());
    EXPECT_EQ(protocolOriginal.GetAutoReverse(), protocolUnmarshalled.GetAutoReverse());

    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest MarshallingUnmarshalling001 end";
}

} // namespace Rosen
} // namespace OHOS
