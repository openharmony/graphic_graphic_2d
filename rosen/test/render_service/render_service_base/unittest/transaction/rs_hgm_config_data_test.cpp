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

#include "transaction/rs_hgm_config_data.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 256;
constexpr int32_t MAX_PAGE_NAME_SIZE = 64;

class RSHgmConfigDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHgmConfigDataTest::SetUpTestCase() {}
void RSHgmConfigDataTest::TearDownTestCase() {}
void RSHgmConfigDataTest::SetUp() {}
void RSHgmConfigDataTest::TearDown() {}

/**
 * @tc.name: UnmarshallingTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    std::string type = "test";
    std::string name = "test";
    int minSpeed = 1;
    int maxSpeed = 2;
    int preferredFps = 3;
    AnimDynamicItem item = {type, name, minSpeed, maxSpeed, preferredFps};
    rsHgmConfigData.AddAnimDynamicItem(item);
    rsHgmConfigData.Marshalling(parcel);
    RSHgmConfigData* rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel);
    ASSERT_NE(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    parcel.WriteFloat(rsHgmConfigData.ppi_);
    parcel.WriteFloat(rsHgmConfigData.xDpi_);
    parcel.WriteFloat(rsHgmConfigData.yDpi_);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    parcel.WriteInt32(1000);
    parcel.WriteInt32(120);
    parcel.WriteUint32(1);
    parcel.WriteString("pageName");
    RSHgmConfigData* rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rsHgmConfigDataPtr->configData_.size(), 1);
    EXPECT_EQ(rsHgmConfigDataPtr->pageNameList_.size(), 1);

    Parcel parcel2;
    parcel2.WriteFloat(rsHgmConfigData.ppi_);
    parcel2.WriteFloat(rsHgmConfigData.xDpi_);
    parcel2.WriteFloat(rsHgmConfigData.yDpi_);
    parcel2.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE + 1);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel2);

    Parcel parcel3;
    parcel3.WriteFloat(rsHgmConfigData.ppi_);
    parcel3.WriteFloat(rsHgmConfigData.xDpi_);
    parcel3.WriteFloat(rsHgmConfigData.yDpi_);
    parcel3.WriteUint32(0);
    parcel3.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE + 1);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel3);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    std::string type = "test";
    std::string name = "test";
    int minSpeed = 1;
    int maxSpeed = 2;
    int preferredFps = 3;
    AnimDynamicItem item = {type, name, minSpeed, maxSpeed, preferredFps};
    rsHgmConfigData.AddAnimDynamicItem(item);
    bool marshalling = rsHgmConfigData.Marshalling(parcel);
    ASSERT_TRUE(marshalling);
}
} // namespace Rosen
} // namespace OHOS