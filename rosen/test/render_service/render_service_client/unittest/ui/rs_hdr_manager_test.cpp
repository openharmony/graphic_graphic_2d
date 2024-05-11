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

#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_hdr_manager.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSHDRManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHDRManagerTest::SetUpTestCase() {}
void RSHDRManagerTest::TearDownTestCase() {}
void RSHDRManagerTest::SetUp() {}
void RSHDRManagerTest::TearDown() {}

/**
 * @tc.name: IncreaseHDRNum001
 * @tc.desc: test hdr num++
 * @tc.type:FUNC
 */
HWTEST_F(RSHDRManagerTest, IncreaseHDRNum001, TestSize.Level1)
{
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(RSHDRManager::Instance().getHDRNum() == 0);
    int ret = RSHDRManager::Instance().IncreaseHDRNum();
    ASSERT_TRUE(ret == 1);
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: IncreaseHDRNum002
 * @tc.desc: test hdr num++ twice
 * @tc.type:FUNC
 */
HWTEST_F(RSHDRManagerTest, IncreaseHDRNum002, TestSize.Level1)
{
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(RSHDRManager::Instance().getHDRNum() == 0);
    int ret = RSHDRManager::Instance().IncreaseHDRNum();
    ret = RSHDRManager::Instance().IncreaseHDRNum();
    ASSERT_TRUE(ret == 2);
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: ReduceHDRNum001
 * @tc.desc: test hdr num--
 * @tc.type:FUNC
 */
HWTEST_F(RSHDRManagerTest, ReduceHDRNum001, TestSize.Level1)
{
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(RSHDRManager::Instance().getHDRNum() == 0);
    int ret = RSHDRManager::Instance().IncreaseHDRNum();
    ASSERT_TRUE(ret == 1);
    ret = RSHDRManager::Instance().ReduceHDRNum();
    ASSERT_TRUE(ret == 0);
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: ReduceHDRNum002
 * @tc.desc: test hdr num-- twice
 * @tc.type:FUNC
 */
HWTEST_F(RSHDRManagerTest, ReduceHDRNum002, TestSize.Level1)
{
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(RSHDRManager::Instance().getHDRNum() == 0);
    int ret = RSHDRManager::Instance().ReduceHDRNum();
    ASSERT_TRUE(ret == 0);
    ret = RSHDRManager::Instance().ReduceHDRNum();
    ASSERT_TRUE(ret == 0);
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(ret == 0);
}

} // namespace OHOS::Rosen
