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
#include <regex>
#include "limit_number.h"
#include "parameters.h"
#include "feature/chipset_vsync/rs_chipset_vsync.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsChipsetVsyncTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void RsChipsetVsyncTest::SetUpTestCase() {}
void RsChipsetVsyncTest::TearDownTestCase() {}
void RsChipsetVsyncTest::SetUp() {}
void RsChipsetVsyncTest::TearDown() {}

/*
 * @tc.name: LoadLibrary_001
 * @tc.desc: Test LoadLibrary
 * @tc.type: FUNC
 * @tc.require: issueICM8KJ
 */
HWTEST_F(RsChipsetVsyncTest, LoadLibrary001, TestSize.Level1)
{
    RsChipsetVsync::Instance.chipsetVsyncLibHandle_ = nullptr;
    RsChipsetVsync::Instance.LoadLibrary();
    ASSERT_NE(RsChipsetVsync::Instance.chipsetVsyncLibHandle_, nullptr);
}

/*
 * @tc.name: CloseLibrary_001
 * @tc.desc: Test CloseLibrary
 * @tc.type: FUNC
 * @tc.require: issueICM8KJ
 */
HWTEST_F(RsChipsetVsyncTest, CloseLibrary001, TestSize.Level1)
{
    RsChipsetVsync::Instance.LoadLibrary();
    RsChipsetVsync::Instance.CloseLibrary();
    ASSERT_EQ(RsChipsetVsync::Instance.chipsetVsyncLibHandle_, nullptr);
}
/*
 * @tc.name: InitChipsetVsync_001
 * @tc.desc: Test InitChipsetVsync
 * @tc.type: FUNC
 * @tc.require: issueICM8KJ
 */
HWTEST_F(RsChipsetVsyncTest, InitChipsetVsync001, TestSize.Level1)
{
    RsChipsetVsync::Instance.LoadLibrary();
    RsChipsetVsync::Instance.InitChipsetVsync();
    ASSERT_NE(RsChipsetVsync::Instance.chipsetVsyncLibHandle_, nullptr);
    RsChipsetVsync::Instance.CloseLibrary();
    RsChipsetVsync::Instance.InitChipsetVsync();
    ASSERT_EQ(RsChipsetVsync::Instance.chipsetVsyncLibHandle_, nullptr);
}
/*
 * @tc.name: SetVsync_001
 * @tc.desc: Test SetVsync
 * @tc.type: FUNC
 * @tc.require: issueICM8KJ
 */
HWTEST_F(RsChipsetVsyncTest, SetVsync001, TestSize.Level1)
{
    RsChipsetVsync::Instance.LoadLibrary();
    RsChipsetVsync::Instance.SetVsync(0, 0, 16666666, false);
    ASSERT_NE(RsChipsetVsync::Instance.chipsetVsyncLibHandle_, nullptr);
    RsChipsetVsync::Instance.CloseLibrary();
    RsChipsetVsync::Instance.SetVsync(0, 0, 16666666, false);
    ASSERT_EQ(RsChipsetVsync::Instance.chipsetVsyncLibHandle_, nullptr);
}

}