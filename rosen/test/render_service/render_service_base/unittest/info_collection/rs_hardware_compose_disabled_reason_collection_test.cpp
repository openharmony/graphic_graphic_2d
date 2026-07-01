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

#include "info_collection/rs_hardware_compose_disabled_reason_collection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsHardwareComposeDisabledReasonCollectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsHardwareComposeDisabledReasonCollectionTest::SetUpTestCase() {}
void RsHardwareComposeDisabledReasonCollectionTest::TearDownTestCase() {}
void RsHardwareComposeDisabledReasonCollectionTest::SetUp() {}
void RsHardwareComposeDisabledReasonCollectionTest::TearDown() {}

/**
 * @tc.name: UpdateHwcDisabledReasonForDFX001
 * @tc.desc: UpdateHwcDisabledReasonForDFX test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsHardwareComposeDisabledReasonCollectionTest, UpdateHwcDisabledReasonForDFX001, TestSize.Level1)
{
    auto& hardwareComposeDisabledReasonCollection = HwcDisabledReasonCollection::GetInstance();
    auto originDisabledReasonInfoMapSize = hardwareComposeDisabledReasonCollection.GetHwcDisabledReasonInfo().size();

    NodeId id = 0;
    int32_t disabledReason = 1;
    std::string nodeName = "node";
    hardwareComposeDisabledReasonCollection.UpdateHwcDisabledReasonForDFX(id, disabledReason, nodeName);
    ++originDisabledReasonInfoMapSize;
    EXPECT_TRUE(hardwareComposeDisabledReasonCollection.GetHwcDisabledReasonInfo().size() ==
        originDisabledReasonInfoMapSize);

    disabledReason = -1;
    ++originDisabledReasonInfoMapSize;
    hardwareComposeDisabledReasonCollection.UpdateHwcDisabledReasonForDFX(id, disabledReason, nodeName);
    EXPECT_FALSE(hardwareComposeDisabledReasonCollection.GetHwcDisabledReasonInfo().size() ==
        originDisabledReasonInfoMapSize);

    disabledReason = static_cast<int32_t>(HwcDisabledReasons::DISABLED_REASON_LENGTH);
    ++originDisabledReasonInfoMapSize;
    hardwareComposeDisabledReasonCollection.UpdateHwcDisabledReasonForDFX(id, disabledReason, nodeName);
    EXPECT_FALSE(hardwareComposeDisabledReasonCollection.GetHwcDisabledReasonInfo().size() ==
        originDisabledReasonInfoMapSize);
}
} // namespace OHOS::Rosen
