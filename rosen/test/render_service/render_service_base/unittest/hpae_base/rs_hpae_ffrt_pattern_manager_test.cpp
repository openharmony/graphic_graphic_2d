/*
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
#include "gtest/gtest.h"
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>
#include "param/sys_param.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_base_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaeFfrtPatternManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;
};

void RSHpaeFfrtPatternManagerTest::SetUpTestCase()
{
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1");
}
void RSHpaeFfrtPatternManagerTest::TearDownTestCase()
{
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", hpaeAaeSwitch);
}
void RSHpaeFfrtPatternManagerTest::SetUp() {}
void RSHpaeFfrtPatternManagerTest::TearDown() {}

/**
 * @tc.name: UpdatedTest
 * @tc.desc: Verify function IsUpdated/SetUpdatedFlag/ResetUpdatedFlag/SetThreadId/IsThreadIdMatch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, UpdatedTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    ffrtManager.SetThreadId();
    ffrtManager.SetUpdatedFlag();
    bool updated = ffrtManager.IsUpdated();
    EXPECT_TRUE(updated);

    ffrtManager.ResetUpdatedFlag();
    updated = ffrtManager.IsUpdated();
    EXPECT_FALSE(updated);
}

/**
 * @tc.name: MHCCheckTest
 * @tc.desc: Verify function MHCCheck
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCCheckTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    ffrtManager.g_instance = nullptr;
    bool ret = ffrtManager.MHCCheck("MHCCheck", 0);
    EXPECT_FALSE(ret);
}

} // namespace Rosen
} // namespace OHOS