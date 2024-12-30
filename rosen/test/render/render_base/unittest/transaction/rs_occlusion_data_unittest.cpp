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

#include <gtest/gtest.h>

#include "transaction/rs_occlusion_data.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSOcclusionDataUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionDataUnitTest::SetUpTestCase() {}
void RSOcclusionDataUnitTest::TearDownTestCase() {}
void RSOcclusionDataUnitTest::SetUp() {}
void RSOcclusionDataUnitTest::TearDown() {}

/**
 * @tc.name: UnmarshallingTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionDataUnitTest, UnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    RSOcclusionData rsOcclusionData;
    uint64_t id = 2;
    uint32_t visibelLevel = 3;
    rsOcclusionData.visibleData_.emplace_back(std::make_pair(id, (WINDOW_LAYER_INFO_TYPE)visibelLevel));
    parcel.WriteUint32(1); // for test size
    parcel.WriteUint64(2); // for test data.first
    parcel.WriteUint32(3); // for test data.second
    RSOcclusionData* rsOcclusionDataPtr = rsOcclusionData.Unmarshalling(parcel);
    ASSERT_NE(rsOcclusionDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionDataUnitTest, UnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    RSOcclusionData rsOcclusionData;
    RSOcclusionData* rsOcclusionDataPtr = rsOcclusionData.Unmarshalling(parcel);
    ASSERT_NE(rsOcclusionDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionDataUnitTest, UnmarshallingTest003, TestSize.Level1)
{
    Parcel parcel;
    RSOcclusionData rsOcclusionData;
    parcel.WriteUint32(5); // for test size
    RSOcclusionData* rsOcclusionDataPtr = rsOcclusionData.Unmarshalling(parcel);
    ASSERT_NE(rsOcclusionDataPtr, nullptr);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionDataUnitTest, MarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    RSOcclusionData rsOcclusionData;
    uint64_t id = 2;
    uint32_t visibelLevel = 3;
    rsOcclusionData.visibleData_.emplace_back(std::make_pair(id, (WINDOW_LAYER_INFO_TYPE)visibelLevel));
    bool marshalling = rsOcclusionData.Marshalling(parcel);
    ASSERT_TRUE(marshalling);
}
} // namespace Rosen
} // namespace OHOS