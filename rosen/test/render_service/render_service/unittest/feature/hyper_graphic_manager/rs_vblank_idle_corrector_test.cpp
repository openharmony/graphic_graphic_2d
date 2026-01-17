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

#include "feature/hyper_graphic_manager/rs_vblank_idle_corrector.h"
#include "hgm_core.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSVBlankIdleCorrectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSVBlankIdleCorrectorTest::SetUpTestCase() {}
void RSVBlankIdleCorrectorTest::TearDownTestCase() {}
void RSVBlankIdleCorrectorTest::SetUp() {}
void RSVBlankIdleCorrectorTest::TearDown() {}

/**
 * @tc.name: SetScreenVBlankIdleTest
 * @tc.desc: test RSVBlankIdleCorrector.SetScreenVBlankIdle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVBlankIdleCorrectorTest, SetScreenVBlankIdleTest, TestSize.Level1)
{
    auto rsVBlankIdleCorrector = std::make_shared<RSVBlankIdleCorrector>();
    ASSERT_NE(rsVBlankIdleCorrector, nullptr);

    HgmCore& hgmCore = HgmCore::Instance();
    bool orgVBlankIdleCorrectSwitch = hgmCore.vBlankIdleCorrectSwitch_;

    hgmCore.vBlankIdleCorrectSwitch_ = false;
    rsVBlankIdleCorrector->SetScreenVBlankIdle();
    EXPECT_EQ(rsVBlankIdleCorrector->isVBlankIdle_, false);

    hgmCore.vBlankIdleCorrectSwitch_ = true;
    rsVBlankIdleCorrector->SetScreenVBlankIdle();
    EXPECT_EQ(rsVBlankIdleCorrector->isVBlankIdle_, true);

    hgmCore.vBlankIdleCorrectSwitch_ = orgVBlankIdleCorrectSwitch;
}

/**
 * @tc.name: ProcessScreenConstraintTest
 * @tc.desc: test RSVBlankIdleCorrector.ProcessScreenConstraint
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVBlankIdleCorrectorTest, ProcessScreenConstraintTest, TestSize.Level1)
{
    auto rsVBlankIdleCorrector = std::make_shared<RSVBlankIdleCorrector>();
    ASSERT_NE(rsVBlankIdleCorrector, nullptr);
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    ASSERT_NE(screenManager, nullptr);

    HgmCore& hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    RSScreenManager* orgScmFromHgm = hgmCore.GetScreenManager();
    bool orgVBlankIdleCorrectSwitch = hgmCore.vBlankIdleCorrectSwitch_;
    bool orgIsAdaptive = frameRateMgr->isAdaptive_;
    bool orgIsGameNodeOnTree = frameRateMgr->isGameNodeOnTree_;
    hgmCore.SetScreenManager(screenManager.GetRefPtr());

    ScreenId id = frameRateMgr->GetCurScreenId() + 10;
    uint64_t timestamp = 10;
    uint64_t constraintTime = 10;
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);

    hgmCore.vBlankIdleCorrectSwitch_ = false;
    id = frameRateMgr->GetCurScreenId();
    rsVBlankIdleCorrector->idleFrameCount_ = 2;
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);
    EXPECT_EQ(rsVBlankIdleCorrector->idleFrameCount_, 0);

    hgmCore.vBlankIdleCorrectSwitch_ = true;
    rsVBlankIdleCorrector->SetScreenVBlankIdle();
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);
    EXPECT_EQ(rsVBlankIdleCorrector->idleFrameCount_, 1);
    EXPECT_EQ(rsVBlankIdleCorrector->isVBlankIdle_, true);
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);
    EXPECT_EQ(rsVBlankIdleCorrector->idleFrameCount_, 0);
    EXPECT_EQ(rsVBlankIdleCorrector->isVBlankIdle_, true);
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);
    EXPECT_EQ(rsVBlankIdleCorrector->isVBlankIdle_, false);

    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, 0);

    rsVBlankIdleCorrector->SetScreenVBlankIdle();
    frameRateMgr->isAdaptive_ = SupportASStatus::SUPPORT_AS;
    frameRateMgr->isGameNodeOnTree_ = true;
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);
    EXPECT_EQ(rsVBlankIdleCorrector->idleFrameCount_, 2);

    hgmCore.hgmFrameRateMgr_ = nullptr;
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);
    hgmCore.SetScreenManager(nullptr);
    rsVBlankIdleCorrector->ProcessScreenConstraint(id, timestamp, constraintTime);

    hgmCore.vBlankIdleCorrectSwitch_ = orgVBlankIdleCorrectSwitch;
    hgmCore.SetScreenManager(orgScmFromHgm);
    hgmCore.hgmFrameRateMgr_ = frameRateMgr;
    frameRateMgr->isAdaptive_ = orgIsAdaptive;
    frameRateMgr->isGameNodeOnTree_ = orgIsGameNodeOnTree;
}
} // namespace OHOS::Rosen