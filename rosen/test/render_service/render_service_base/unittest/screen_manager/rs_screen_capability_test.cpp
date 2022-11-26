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

#include <gtest/gtest.h>
#include <securec.h>

#include "screen_manager/rs_screen_capability.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSScreenCapabilityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenCapabilityTest::SetUpTestCase() {}
void RSScreenCapabilityTest::TearDownTestCase() {}
void RSScreenCapabilityTest::SetUp() {}
void RSScreenCapabilityTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCapabilityTest, Marshalling001, TestSize.Level1)
{
    RSScreenCapability capability;
    Parcel parcel;
    ASSERT_TRUE(capability.Marshalling(parcel));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCapabilityTest, Unmarshalling001, TestSize.Level1)
{
    RSScreenCapability capability;
    Parcel parcel;
    capability.Unmarshalling(parcel);
}

/**
 * @tc.name: marshallingAndUnmarshallling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCapabilityTest, marshallingAndUnmarshallling001, TestSize.Level1)
{
    RSScreenProps rp1;
    RSScreenProps rp2;
    std::vector<RSScreenProps> props = { rp1, rp2 };
    RSScreenCapability capability("1", ScreenInterfaceType::DISP_INTF_LCD, 2, 4, 3, 3, true, props);
    Parcel parcel;
    auto bufferSize = parcel.GetMaxCapacity();
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = capability.Marshalling(parcel) & (RSScreenCapability::Unmarshalling(parcel) != nullptr);
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
