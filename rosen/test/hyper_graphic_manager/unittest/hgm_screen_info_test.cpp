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
#include <limits>
#include <test_header.h>

#include "hgm_screen_info.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmScreenInfoTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: GetScreenType
 * @tc.desc: Verify the execution process of GetScreenType function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmScreenInfoTest, GetScreenType, Function | SmallTest | Level1)
{
    auto& hgmScreenInfo = HgmScreenInfo::GetInstance();
    ScreenId id = 0;
    ASSERT_NO_FATAL_FAILURE({hgmScreenInfo.GetScreenType(id);});
}

/**
 * @tc.name: IsLtpoType001
 * @tc.desc: Verify the result of IsLtpoType function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmScreenInfoTest, IsLtpoType001, Function | SmallTest | Level1)
{
    auto& hgmScreenInfo = HgmScreenInfo::GetInstance();
    HgmScreenType screenType = HgmScreenType::LTPO;
    bool ret = hgmScreenInfo.IsLtpoType(screenType);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: IsLtpoType002
 * @tc.desc: Verify the result of IsLtpoType function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmScreenInfoTest, IsLtpoType002, Function | SmallTest | Level1)
{
    auto& hgmScreenInfo = HgmScreenInfo::GetInstance();
    HgmScreenType screenType = HgmScreenType::LTPS;
    bool ret = hgmScreenInfo.IsLtpoType(screenType);
    ASSERT_NE(ret, true);
}

/**
 * @tc.name: Init
 * @tc.desc: Verify the execution process of Init function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmScreenInfoTest, Init, Function | SmallTest | Level1)
{
    auto& hgmScreenInfo = HgmScreenInfo::GetInstance();
    ASSERT_NO_FATAL_FAILURE({hgmScreenInfo.Init();});
}

/**
 * @tc.name: GetPanelInfo
 * @tc.desc: Verify the execution process of GetPanelInfo function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmScreenInfoTest, GetPanelInfo, Function | SmallTest | Level1)
{
    auto& hgmScreenInfo = HgmScreenInfo::GetInstance();
    std::string panelInfo;
    ASSERT_NO_FATAL_FAILURE({hgmScreenInfo.GetPanelInfo(panelInfo);});
}

/**
 * @tc.name: InitPanelTypeMap
 * @tc.desc: Verify the execution process of InitPanelTypeMap function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmScreenInfoTest, InitPanelTypeMap, Function | SmallTest | Level1)
{
    auto& hgmScreenInfo = HgmScreenInfo::GetInstance();
    std::string panelInfo;
    ASSERT_NO_FATAL_FAILURE({hgmScreenInfo.InitPanelTypeMap(panelInfo);});
}

} // namespace Rosen
} // namespace OHOS