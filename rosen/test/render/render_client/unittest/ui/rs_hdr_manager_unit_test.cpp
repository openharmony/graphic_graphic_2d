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

class RsHdrManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsHdrManagerTest::SetUpTestCase() {}
void RsHdrManagerTest::TearDownTestCase() {}
void RsHdrManagerTest::SetUp() {}
void RsHdrManagerTest::TearDown() {}

/**
 * @tc.name: IncreaseHDRNum01
 * @tc.desc: test hdr num++
 * @tc.type:FUNC
 */
HWTEST_F(RsHdrManagerTest, IncreaseHDRNum01, TestSize.Level1)
{
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(RSHDRManager::Instance().getHDRNum() == 0);
    int ret = RSHDRManager::Instance().IncreaseHDRNum();
    ASSERT_TRUE(ret == 1);
    RSHDRManager::Instance().ResetHDRNum();
}

/**
 * @tc.name: IncreaseHDRNum02
 * @tc.desc: test hdr num++ twice
 * @tc.type:FUNC
 */
HWTEST_F(RsHdrManagerTest, IncreaseHDRNum02, TestSize.Level1)
{
    RSHDRManager::Instance().ResetHDRNum();
    ASSERT_TRUE(RSHDRManager::Instance().getHDRNum() == 0);
    int ret = RSHDRManager::Instance().IncreaseHDRNum();
    ret = RSHDRManager::Instance().IncreaseHDRNum();
    ASSERT_TRUE(ret == 2);
    RSHDRManager::Instance().ResetHDRNum();
}

/**
 * @tc.name: ReduceHDRNum01
 * @tc.desc: test hdr num--
 * @tc.type:FUNC
 */
HWTEST_F(RsHdrManagerTest, ReduceHDRNum01, TestSize.Level1)
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
 * @tc.name: ReduceHDRNum02
 * @tc.desc: test hdr num-- twice
 * @tc.type:FUNC
 */
HWTEST_F(RsHdrManagerTest, ReduceHDRNum02, TestSize.Level1)
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

/**
 * @tc.name: IncreaseHDRNum03
 * @tc.desc: test results of IncreaseHDRNum
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RsHdrManagerTest, IncreaseHDRNum03, TestSize.Level1)
{
    RSHDRManager& manager = RSHDRManager::Instance();
    manager.hdrNum_ = 1;
    manager.IncreaseHDRNum();
    EXPECT_TRUE(manager.getHDRNum() == 2);

    manager.hdrNum_ = 0;
    manager.IncreaseHDRNum();
    EXPECT_TRUE(manager.getHDRNum());

    manager.hdrNum_ = 0;
    manager.setHDRPresent_ = [](bool flag, NodeId id) {};
    manager.IncreaseHDRNum();
    EXPECT_TRUE(manager.getHDRNum());

    manager.ResetHDRNum();
    manager.nodeId_ = 1;
    manager.IncreaseHDRNum();
    EXPECT_TRUE(manager.getHDRNum());
}

/**
 * @tc.name: ReduceHDRNum03
 * @tc.desc: test results of ReduceHDRNum
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RsHdrManagerTest, ReduceHDRNum03, TestSize.Level1)
{
    RSHDRManager& manager = RSHDRManager::Instance();
    manager.hdrNum_ = 0;
    manager.ReduceHDRNum();
    EXPECT_FALSE(manager.getHDRNum());

    manager.hdrNum_ = 2;
    manager.ReduceHDRNum();
    EXPECT_TRUE(manager.getHDRNum() == 1);

    manager.hdrNum_ = 1;
    manager.ReduceHDRNum();
    EXPECT_FALSE(manager.getHDRNum());

    manager.hdrNum_ = 1;
    manager.setHDRPresent_ = [](bool flag, NodeId id) {};
    manager.ReduceHDRNum();
    EXPECT_FALSE(manager.getHDRNum());

    manager.hdrNum_ = 1;
    manager.nodeId_ = 1;
    manager.ReduceHDRNum();
    EXPECT_FALSE(manager.getHDRNum());
}

/**
 * @tc.name: RegisterSetHDRPresent01
 * @tc.desc: test results of RegisterSetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RsHdrManagerTest, RegisterSetHDRPresent01, TestSize.Level1)
{
    RSHDRManager& manager = RSHDRManager::Instance();
    HDRFunc func = [](bool flag, NodeId id) {};
    manager.RegisterSetHDRPresent(func, 1);
    EXPECT_TRUE(manager.nodeId_);
    EXPECT_TRUE(manager.setHDRPresent_);

    manager.RegisterSetHDRPresent(func, INVALID_NODEID);
    EXPECT_TRUE(manager.nodeId_);

    manager.setHDRPresent_ = [](bool flag, NodeId id) {};
    manager.RegisterSetHDRPresent(func, INVALID_NODEID);
    EXPECT_TRUE(manager.setHDRPresent_);

    func = nullptr;
    manager.RegisterSetHDRPresent(func, INVALID_NODEID);
    EXPECT_TRUE(manager.setHDRPresent_);
}

/**
 * @tc.name: UnRegisterSetHDRPresent01
 * @tc.desc: test results of UnRegisterSetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RsHdrManagerTest, UnRegisterSetHDRPresent01, TestSize.Level1)
{
    RSHDRManager& manager = RSHDRManager::Instance();
    manager.UnRegisterSetHDRPresent(INVALID_NODEID);
    EXPECT_TRUE(manager.nodeId_);

    manager.nodeId_ = 0;
    manager.UnRegisterSetHDRPresent(INVALID_NODEID);
    EXPECT_FALSE(manager.nodeId_);
    EXPECT_FALSE(manager.setHDRPresent_);
}
} // namespace OHOS::Rosen
