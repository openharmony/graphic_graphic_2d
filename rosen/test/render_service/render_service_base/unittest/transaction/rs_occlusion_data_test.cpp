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

#include <gtest/gtest.h>

#include "transaction/rs_occlusion_data.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSOcclusionDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionDataTest::SetUpTestCase() {}
void RSOcclusionDataTest::TearDownTestCase() {}
void RSOcclusionDataTest::SetUp() {}
void RSOcclusionDataTest::TearDown() {}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionDataTest, UnmarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    RSOcclusionData rsOcclusionData;
    RSOcclusionData* rsOcclusionDataPtr = rsOcclusionData.Unmarshalling(parcel);
    ASSERT_NE(rsOcclusionDataPtr,nullptr);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionDataTest, MarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    RSOcclusionData rsOcclusionData;
    bool marshalling = rsOcclusionData.Marshalling(parcel);
    ASSERT_TRUE(marshalling);
}
} // namespace Rosen
} // namespace OHOS