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

#include <gtest/gtest.h>
#include <parameters.h>

#include "tunnel_layer_param.h"
#include "feature/tunnel_layer/rs_tunnel_layer_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
// Scoped guard that flips persist.rosen.debug.new_tunnel and restores the old value on destruction.
// Controls RSSystemProperties::GetNewTunnelEnabled() which is one operand of the && added in IsNewTunnelEnabled().
class ScopedSystemNewTunnelSwitch {
public:
    explicit ScopedSystemNewTunnelSwitch(bool enabled)
    {
        oldValue_ = system::GetParameter("persist.rosen.debug.new_tunnel", "0") == "1";
        system::SetParameter("persist.rosen.debug.new_tunnel", enabled ? "1" : "0");
    }
    ~ScopedSystemNewTunnelSwitch()
    {
        system::SetParameter("persist.rosen.debug.new_tunnel", oldValue_ ? "1" : "0");
    }
private:
    bool oldValue_ = false;
};
} // namespace

class TunnelLayerParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TunnelLayerParamTest::SetUpTestCase() {}
void TunnelLayerParamTest::TearDownTestCase() {}
void TunnelLayerParamTest::SetUp() {}
void TunnelLayerParamTest::TearDown() {}

/**
 * @tc.name: SetNewTunnelEnabledTrue
 * @tc.desc: Verify TunnelLayerParam::SetNewTunnelEnabled(true) keeps IsNewTunnelEnabled() == true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamTest, SetNewTunnelEnabledTrue, TestSize.Level1)
{
    TunnelLayerParam::SetNewTunnelEnabled(true);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), true);
}

/**
 * @tc.name: SetNewTunnelEnabledFalse
 * @tc.desc: Verify TunnelLayerParam::SetNewTunnelEnabled(false) makes IsNewTunnelEnabled() == false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamTest, SetNewTunnelEnabledFalse, TestSize.Level1)
{
    TunnelLayerParam::SetNewTunnelEnabled(false);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), false);
    TunnelLayerParam::SetNewTunnelEnabled(true);
}

/**
 * @tc.name: IsNewTunnelEnabledDefaultTrue
 * @tc.desc: Verify the default value of TunnelLayerParam::isNewTunnelEnabled_ is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamTest, IsNewTunnelEnabledDefaultTrue, TestSize.Level1)
{
    TunnelLayerParam::SetNewTunnelEnabled(true);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), true);
}

/**
 * @tc.name: IsNewTunnelEnabled_BothEnabled_ReturnsTrue
 * @tc.desc: Verify rs_tunnel_layer_utils.h IsNewTunnelEnabled() returns true when
 *           TunnelLayerParam::IsNewTunnelEnabled()=true AND RSSystemProperties::GetNewTunnelEnabled()=true
 *           (covers both operands of the && added in commit 876fa40)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamTest, IsNewTunnelEnabled_BothEnabled_ReturnsTrue, TestSize.Level1)
{
    TunnelLayerParam::SetNewTunnelEnabled(true);
    ScopedSystemNewTunnelSwitch systemSwitch(true);
    EXPECT_EQ(IsNewTunnelEnabled(), true);
}

/**
 * @tc.name: IsNewTunnelEnabled_TunnelParamDisabled_ReturnsFalse
 * @tc.desc: Verify rs_tunnel_layer_utils.h IsNewTunnelEnabled() returns false when
 *           TunnelLayerParam::IsNewTunnelEnabled()=false (short-circuits before RSSystemProperties)
 *           (covers TunnelLayerParam::IsNewTunnelEnabled() == false branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamTest, IsNewTunnelEnabled_TunnelParamDisabled_ReturnsFalse, TestSize.Level1)
{
    TunnelLayerParam::SetNewTunnelEnabled(false);
    ScopedSystemNewTunnelSwitch systemSwitch(true);
    EXPECT_EQ(IsNewTunnelEnabled(), false);
    TunnelLayerParam::SetNewTunnelEnabled(true);
}

/**
 * @tc.name: IsNewTunnelEnabled_SystemDisabled_ReturnsFalse
 * @tc.desc: Verify rs_tunnel_layer_utils.h IsNewTunnelEnabled() returns false when
 *           TunnelLayerParam::IsNewTunnelEnabled()=true AND RSSystemProperties::GetNewTunnelEnabled()=false
 *           (covers RSSystemProperties::GetNewTunnelEnabled() == false branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamTest, IsNewTunnelEnabled_SystemDisabled_ReturnsFalse, TestSize.Level1)
{
    TunnelLayerParam::SetNewTunnelEnabled(true);
    ScopedSystemNewTunnelSwitch systemSwitch(false);
    EXPECT_EQ(IsNewTunnelEnabled(), true);
}
} // namespace OHOS::Rosen
