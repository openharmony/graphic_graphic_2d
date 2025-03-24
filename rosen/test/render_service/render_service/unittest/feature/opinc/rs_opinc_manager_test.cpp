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

#include "feature/opinc/rs_opinc_manager.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSOpincManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline RSOpincManager& opincManager_ = RSOpincManager::Instance();
};

void RSOpincManagerTest::SetUpTestCase() {}
void RSOpincManagerTest::TearDownTestCase() {}
void RSOpincManagerTest::SetUp() {}
void RSOpincManagerTest::TearDown() {}

/**
 * @tc.name: SetOPIncSwitch
 * @tc.desc: Verify the SetOPIncSwitch function
 * @tc.type: FUNC
 * @tc.require: #IBQETW
 */
HWTEST_F(RSOpincManagerTest, SetOPIncEnable, Function | SmallTest | Level1)
{
    opincManager_.SetOPIncSwitch(true);
    ASSERT_EQ(opincManager_.GetOPIncSwitch(), true);
}

/**
 * @tc.name: ReadOPIncCcmParam
 * @tc.desc: Verify the ReadOPIncCcmParam function
 * @tc.type: FUNC
 * @tc.require: #IBQETW
 */
HWTEST_F(RSOpincManagerTest, ReadOPIncCcmParam, Function | SmallTest | Level1)
{
    auto res = opincManager_.ReadOPIncCcmParam();
    ASSERT_EQ(res, true);
}
}