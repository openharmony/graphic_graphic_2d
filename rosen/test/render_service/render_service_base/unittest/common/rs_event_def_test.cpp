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
#include "parcel.h"
#include "common/rs_event_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSEventDefTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEventDefTest::SetUpTestCase() {}
void RSEventDefTest::TearDownTestCase() {}
void RSEventDefTest::SetUp() {}
void RSEventDefTest::TearDown() {}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify Unmarshalling with valid type EXT_SCREEN_UNSUPPORT
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Unmarshalling001, TestSize.Level1)
{
    Parcel parcel;
    uint32_t validType = static_cast<uint32_t>(RSExposedEventType::EXT_SCREEN_UNSUPPORT);
    ASSERT_TRUE(parcel.WriteUint32(validType));
    
    auto data = RSExposedEventDataBase::Unmarshalling(parcel);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->type_, RSExposedEventType::EXT_SCREEN_UNSUPPORT);
    delete data;
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Verify Unmarshalling with invalid type EXPOSED_EVENT_INVALID
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Unmarshalling002, TestSize.Level1)
{
    Parcel parcel;
    uint32_t invalidType = static_cast<uint32_t>(RSExposedEventType::EXPOSED_EVENT_INVALID);
    ASSERT_TRUE(parcel.WriteUint32(invalidType));
    
    auto data = RSExposedEventDataBase::Unmarshalling(parcel);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->type_, RSExposedEventType::EXPOSED_EVENT_INVALID);
    delete data;
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: Verify Unmarshalling with type value greater than EXPOSED_EVENT_INVALID
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Unmarshalling003, TestSize.Level1)
{
    Parcel parcel;
    uint32_t outOfRangeType = static_cast<uint32_t>(RSExposedEventType::EXPOSED_EVENT_INVALID) + 1;
    ASSERT_TRUE(parcel.WriteUint32(outOfRangeType));
    
    auto data = RSExposedEventDataBase::Unmarshalling(parcel);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->type_, RSExposedEventType::EXPOSED_EVENT_INVALID);
    delete data;
}

/**
 * @tc.name: Unmarshalling004
 * @tc.desc: Verify Unmarshalling with maximum uint32_t value
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Unmarshalling004, TestSize.Level1)
{
    Parcel parcel;
    uint32_t maxType = UINT32_MAX;
    ASSERT_TRUE(parcel.WriteUint32(maxType));
    
    auto data = RSExposedEventDataBase::Unmarshalling(parcel);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->type_, RSExposedEventType::EXPOSED_EVENT_INVALID);
    delete data;
}

/**
 * @tc.name: Unmarshalling005
 * @tc.desc: Verify Unmarshalling with empty parcel (read failure)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Unmarshalling005, TestSize.Level1)
{
    Parcel parcel;
    
    auto data = RSExposedEventDataBase::Unmarshalling(parcel);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->type_, RSExposedEventType::EXPOSED_EVENT_INVALID);
    delete data;
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify Marshalling with EXT_SCREEN_UNSUPPORT type
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Marshalling001, TestSize.Level1)
{
    RSExposedEventDataBase data;
    data.type_ = RSExposedEventType::EXT_SCREEN_UNSUPPORT;
    
    Parcel parcel;
    ASSERT_TRUE(data.Marshalling(parcel));
    
    uint32_t readType;
    ASSERT_TRUE(parcel.ReadUint32(readType));
    EXPECT_EQ(readType, static_cast<uint32_t>(RSExposedEventType::EXT_SCREEN_UNSUPPORT));
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Verify Marshalling with EXPOSED_EVENT_INVALID type
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Marshalling002, TestSize.Level1)
{
    RSExposedEventDataBase data;
    data.type_ = RSExposedEventType::EXPOSED_EVENT_INVALID;
    
    Parcel parcel;
    ASSERT_TRUE(data.Marshalling(parcel));
    
    uint32_t readType;
    ASSERT_TRUE(parcel.ReadUint32(readType));
    EXPECT_EQ(readType, static_cast<uint32_t>(RSExposedEventType::EXPOSED_EVENT_INVALID));
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Verify Marshalling with RSExtScreenUnsupportData
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Marshalling003, TestSize.Level1)
{
    RSExtScreenUnsupportData data;
    
    Parcel parcel;
    ASSERT_TRUE(data.Marshalling(parcel));
    
    uint32_t readType;
    ASSERT_TRUE(parcel.ReadUint32(readType));
    EXPECT_EQ(readType, static_cast<uint32_t>(RSExposedEventType::EXT_SCREEN_UNSUPPORT));
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Verify Marshalling with default constructed RSExposedEventDataBase
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, Marshalling004, TestSize.Level1)
{
    RSExposedEventDataBase data;
    
    Parcel parcel;
    ASSERT_TRUE(data.Marshalling(parcel));
    
    uint32_t readType;
    ASSERT_TRUE(parcel.ReadUint32(readType));
    EXPECT_EQ(readType, static_cast<uint32_t>(RSExposedEventType::EXPOSED_EVENT_INVALID));
}

/**
 * @tc.name: RoundTrip001
 * @tc.desc: Verify round-trip Marshalling and Unmarshalling with EXT_SCREEN_UNSUPPORT
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, RoundTrip001, TestSize.Level1)
{
    RSExposedEventDataBase originalData;
    originalData.type_ = RSExposedEventType::EXT_SCREEN_UNSUPPORT;
    
    Parcel parcel;
    ASSERT_TRUE(originalData.Marshalling(parcel));
    
    auto unmarshalledData = RSExposedEventDataBase::Unmarshalling(parcel);
    ASSERT_NE(unmarshalledData, nullptr);
    EXPECT_EQ(unmarshalledData->type_, originalData.type_);
    delete unmarshalledData;
}

/**
 * @tc.name: RoundTrip002
 * @tc.desc: Verify round-trip Marshalling and Unmarshalling with EXPOSED_EVENT_INVALID
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, RoundTrip002, TestSize.Level1)
{
    RSExposedEventDataBase originalData;
    originalData.type_ = RSExposedEventType::EXPOSED_EVENT_INVALID;
    
    Parcel parcel;
    ASSERT_TRUE(originalData.Marshalling(parcel));
    
    auto unmarshalledData = RSExposedEventDataBase::Unmarshalling(parcel);
    ASSERT_NE(unmarshalledData, nullptr);
    EXPECT_EQ(unmarshalledData->type_, originalData.type_);
    delete unmarshalledData;
}

/**
 * @tc.name: RSExtScreenUnsupportData001
 * @tc.desc: Verify RSExtScreenUnsupportData constructor sets correct type
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, RSExtScreenUnsupportData001, TestSize.Level1)
{
    RSExtScreenUnsupportData data;
    EXPECT_EQ(data.type_, RSExposedEventType::EXT_SCREEN_UNSUPPORT);
}

/**
 * @tc.name: RSExposedEventDataBase001
 * @tc.desc: Verify RSExposedEventDataBase default constructor sets EXPOSED_EVENT_INVALID
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, RSExposedEventDataBase001, TestSize.Level1)
{
    RSExposedEventDataBase data;
    EXPECT_EQ(data.type_, RSExposedEventType::EXPOSED_EVENT_INVALID);
}

/**
 * @tc.name: RSExposedEventDataBase002
 * @tc.desc: Verify RSExposedEventDataBase move constructor
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDefTest, RSExposedEventDataBase002, TestSize.Level1)
{
    RSExposedEventDataBase data1;
    data1.type_ = RSExposedEventType::EXT_SCREEN_UNSUPPORT;
    
    RSExposedEventDataBase data2(std::move(data1));
    EXPECT_EQ(data2.type_, RSExposedEventType::EXT_SCREEN_UNSUPPORT);
}
} // namespace OHOS::Rosen
