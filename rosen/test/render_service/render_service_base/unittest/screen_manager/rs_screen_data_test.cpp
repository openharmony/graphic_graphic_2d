/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <securec.h>
#include <gtest/gtest.h>

#include "screen_manager/rs_screen_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSScreenDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenDataTest::SetUpTestCase() {}
void RSScreenDataTest::TearDownTestCase() {}
void RSScreenDataTest::SetUp() {}
void RSScreenDataTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenDataTest, Marshalling001, TestSize.Level1)
{
    RSScreenData screenData;
    Parcel parcel;
    ASSERT_TRUE(screenData.Marshalling(parcel));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenDataTest, Unmarshalling001, TestSize.Level1)
{
    RSScreenData screenData;
    Parcel parcel;
    screenData.Unmarshalling(parcel);
}

/**
 * @tc.name: marshallingAndUnmarshallling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenDataTest, marshallingAndUnmarshallling001, TestSize.Level1)
{
    RSScreenModeInfo ri1;
    RSScreenModeInfo ri2;
    std::vector<RSScreenModeInfo> supportModeInfo = {ri1, ri2};
    RSScreenCapability capability;
    RSScreenModeInfo activityModeInfo;
    RSScreenData screenData(capability, activityModeInfo, supportModeInfo, ScreenPowerStatus::POWER_STATUS_OFF);
    Parcel parcel;
    char* buffer = static_cast<char *>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = screenData.Marshalling(parcel) & (RSScreenData::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

} // namespace Rosen
} // namespace OHOS
