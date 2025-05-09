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

#include "screen_manager/rs_screen_hdr_capability.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSScreenHDRCapabilityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenHDRCapabilityTest::SetUpTestCase() {}
void RSScreenHDRCapabilityTest::TearDownTestCase() {}
void RSScreenHDRCapabilityTest::SetUp() {}
void RSScreenHDRCapabilityTest::TearDown() {}

/**
 * @tc.name: SetMaxLumTest
 * @tc.desc: test SetMaxLum
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenHDRCapabilityTest, SetMaxLumTest, TestSize.Level1)
{
    RSScreenHDRCapability capability;
    capability.SetMaxLum(1.f);
    ASSERT_EQ(1.f, capability.GetMaxLum());

    capability.SetMaxLum(0.f);
    ASSERT_EQ(0.f, capability.GetMaxLum());
}

/**
 * @tc.name: SetMinLumTest
 * @tc.desc: test SetMinLum
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenHDRCapabilityTest, SetMinLumTest, TestSize.Level1)
{
    RSScreenHDRCapability capability;
    capability.SetMinLum(1.f);
    ASSERT_EQ(1.f, capability.GetMinLum());

    capability.SetMinLum(0.f);
    ASSERT_EQ(0.f, capability.GetMinLum());
}

/**
 * @tc.name: SetMaxAverageLumTest
 * @tc.desc: test SetMaxAverageLum
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenHDRCapabilityTest, SetMaxAverageLumTest, TestSize.Level1)
{
    RSScreenHDRCapability capability;
    capability.SetMaxAverageLum(1.f);
    ASSERT_EQ(1.f, capability.GetMaxAverageLum());

    capability.SetMaxAverageLum(0.f);
    ASSERT_EQ(0.f, capability.GetMaxAverageLum());
}

/**
 * @tc.name: SetHdrFormatsTest
 * @tc.desc: test SetHdrFormats
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenHDRCapabilityTest, SetHdrFormatsTest, TestSize.Level1)
{
    RSScreenHDRCapability capability;
    std::vector<ScreenHDRFormat> formats { ScreenHDRFormat::VIDEO_HDR_VIVID };
    capability.SetHdrFormats(formats);
    ASSERT_EQ(formats, capability.GetHdrFormats());
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenHDRCapabilityTest, Marshalling001, TestSize.Level1)
{
    RSScreenHDRCapability capability;
    Parcel parcel;
    ASSERT_TRUE(capability.Marshalling(parcel));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenHDRCapabilityTest, Unmarshalling001, TestSize.Level1)
{
    std::vector<ScreenHDRFormat> formats = {ScreenHDRFormat::VIDEO_HDR10, ScreenHDRFormat::VIDEO_HDR_VIVID};
    RSScreenHDRCapability rsScreen(1.f, 1.f, 1.f, formats);
    Parcel parcel;
    ASSERT_TRUE(rsScreen.Marshalling(parcel));
    ASSERT_NE(RSScreenHDRCapability::Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: marshallingAndUnmarshallling001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenHDRCapabilityTest, marshallingAndUnmarshallling001, TestSize.Level1)
{
    std::vector<ScreenHDRFormat> formats = {ScreenHDRFormat::VIDEO_HDR10, ScreenHDRFormat::VIDEO_HDR_VIVID};
    RSScreenHDRCapability rs(1.f, 1.f, 1.f, formats);
    Parcel parcel;
    auto bufferSize = parcel.GetMaxCapacity();
    char* buffer = static_cast<char *>(malloc(parcel.GetMaxCapacity()));
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = rs.Marshalling(parcel) && (RSScreenHDRCapability::Unmarshalling(parcel) != nullptr);
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
