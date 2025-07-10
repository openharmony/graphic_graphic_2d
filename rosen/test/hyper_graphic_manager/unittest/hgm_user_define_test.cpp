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

#include <gtest/gtest.h>
#include <limits>
#include <test_header.h>

#include "hgm_frame_rate_manager.h"
#include "hgm_user_define.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmUserDefineTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: Init
 * @tc.desc: Verify the result of Init function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmUserDefineTest, Init, Function | SmallTest | Level0)
{
    HgmUserDefine userDefine;
    userDefine.Init();
    ASSERT_NE(userDefine.impl_, nullptr);

    HgmFrameRateManager framRateManager;
    framRateManager.Init(nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(framRateManager.userDefine_.impl_, nullptr);
}
} // namespace Rosen
} // namespace OHOS
