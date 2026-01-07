/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <future>
#include <chrono>
#include "message_parcel.h"
#include "rs_layer_marshalling_helper.h"
#include "rs_render_layer_cmd.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerMarshallingHelperTest : public Test {};
/**
 * Function: Basic_Types_Marshall_Unmarshall
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. marshal bool/int32_t/uint64_t/float/double
 *                  2. unmarshal into variables
 *                  3. verify values match
 */
HWTEST(RSLayerMarshallingHelperTest, Basic_Types_Marshall_Unmarshall, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(RSLayerMarshallingHelper::Marshalling(parcel, true));
    ASSERT_TRUE(RSLayerMarshallingHelper::Marshalling(parcel, int32_t{-7}));
    ASSERT_TRUE(RSLayerMarshallingHelper::Marshalling(parcel, uint64_t{99}));
    ASSERT_TRUE(RSLayerMarshallingHelper::Marshalling(parcel, 1.5f));
    ASSERT_TRUE(RSLayerMarshallingHelper::Marshalling(parcel, 2.5));

    bool b = false; int32_t i = 0; uint64_t u = 0; float f = 0.0f; double d = 0.0;
    ASSERT_TRUE(RSLayerMarshallingHelper::Unmarshalling(parcel, b));
    ASSERT_TRUE(RSLayerMarshallingHelper::Unmarshalling(parcel, i));
    ASSERT_TRUE(RSLayerMarshallingHelper::Unmarshalling(parcel, u));
    ASSERT_TRUE(RSLayerMarshallingHelper::Unmarshalling(parcel, f));
    ASSERT_TRUE(RSLayerMarshallingHelper::Unmarshalling(parcel, d));
    EXPECT_TRUE(b);
    EXPECT_EQ(i, -7);
    EXPECT_EQ(u, 99u);
    EXPECT_FLOAT_EQ(f, 1.5f);
    EXPECT_DOUBLE_EQ(d, 2.5);
}

/**
 * Function: CmdType_Unmarshall_Fail_NoData
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. call Unmarshalling for RSLayerCmdType on empty parcel
 *                  2. expect Unmarshalling returns false
 */
HWTEST(RSLayerMarshallingHelperTest, CmdType_Unmarshall_Fail_NoData, TestSize.Level1)
{
    MessageParcel empty;
    RSLayerCmdType ty = RSLayerCmdType::INVALID;
    EXPECT_FALSE(RSLayerMarshallingHelper::Unmarshalling(empty, ty));
}

/**
 * Function: CmdPtr_Unmarshall_Fail_Invalid
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. call Unmarshalling for command ptr on empty parcel
 *                  2. expect Unmarshalling returns false and pointer remains null
 */
HWTEST(RSLayerMarshallingHelperTest, CmdPtr_Unmarshall_Fail_Invalid, TestSize.Level1)
{
    MessageParcel empty;
    std::shared_ptr<RSRenderLayerCmd> out;
    EXPECT_FALSE(RSLayerMarshallingHelper::Unmarshalling(empty, out));
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: CmdPtr_Marshall_Fail_NullPtr
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. call Marshalling for command ptr with nullptr
 *                  2. expect Marshalling returns false
 */
HWTEST(RSLayerMarshallingHelperTest, CmdPtr_Marshall_Fail_NullPtr, TestSize.Level1)
{
    MessageParcel parcel;
    std::shared_ptr<RSRenderLayerCmd> nullCmd;
    // Guard against potential hang: run with timeout
    auto fut = std::async(std::launch::async, [&]() {
        return RSLayerMarshallingHelper::Marshalling(parcel, nullCmd);
    });
    ASSERT_EQ(fut.wait_for(std::chrono::milliseconds(500)), std::future_status::ready)
        << "Marshalling(nullCmd) timed out";
    EXPECT_FALSE(fut.get());
}
} // namespace OHOS::Rosen
