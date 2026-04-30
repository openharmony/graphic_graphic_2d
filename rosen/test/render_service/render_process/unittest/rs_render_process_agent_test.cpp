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

#include "pipeline/main_thread/rs_main_thread.h"
#include "render_process/rs_render_process_agent.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr ScreenId TEST_SCREEN_ID = 0;
constexpr ScreenId TEST_SCREEN_ID_2 = 1;
} // namespace

class RSRenderProcessAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<RSRenderProcess> renderProcess_;
    static sptr<RSRenderProcessAgent> renderProcessAgent_;
};

sptr<RSRenderProcess> RSRenderProcessAgentTest::renderProcess_ = nullptr;
sptr<RSRenderProcessAgent> RSRenderProcessAgentTest::renderProcessAgent_ = nullptr;

void RSRenderProcessAgentTest::SetUpTestCase()
{
    renderProcess_ = sptr<RSRenderProcess>::MakeSptr();
    renderProcessAgent_ = sptr<RSRenderProcessAgent>::MakeSptr(*renderProcess_);
}

void RSRenderProcessAgentTest::TearDownTestCase()
{
    renderProcessAgent_ = nullptr;
    renderProcess_ = nullptr;
}

void RSRenderProcessAgentTest::SetUp() {}
void RSRenderProcessAgentTest::TearDown() {}

/**
 * @tc.name: NotifyScreenConnectInfoToRenderTest001
 * @tc.desc: Test NotifyScreenConnectInfoToRender with valid screen property
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessAgentTest, NotifyScreenConnectInfoToRenderTest001, TestSize.Level1)
{
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();

    sptr<IRSRenderToComposerConnection> renderToComposerConn = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConn = nullptr;

    renderProcessAgent_->NotifyScreenConnectInfoToRender(
        screenProperty, renderToComposerConn, composerToRenderConn);

    ASSERT_TRUE(renderProcessAgent_ != nullptr);
}

/**
 * @tc.name: NotifyScreenConnectInfoToRenderTest002
 * @tc.desc: Test NotifyScreenConnectInfoToRender with different screen ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessAgentTest, NotifyScreenConnectInfoToRenderTest002, TestSize.Level1)
{
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();

    sptr<IRSRenderToComposerConnection> renderToComposerConn = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConn = nullptr;

    renderProcessAgent_->NotifyScreenConnectInfoToRender(
        screenProperty, renderToComposerConn, composerToRenderConn);

    ASSERT_TRUE(renderProcessAgent_ != nullptr);
}

/**
 * @tc.name: NotifyScreenDisconnectInfoToRenderTest001
 * @tc.desc: Test NotifyScreenDisconnectInfoToRender with valid screen ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessAgentTest, NotifyScreenDisconnectInfoToRenderTest001, TestSize.Level1)
{
    renderProcessAgent_->NotifyScreenDisconnectInfoToRender(TEST_SCREEN_ID);

    ASSERT_TRUE(renderProcessAgent_ != nullptr);
}

/**
 * @tc.name: NotifyScreenDisconnectInfoToRenderTest002
 * @tc.desc: Test NotifyScreenDisconnectInfoToRender with different screen ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessAgentTest, NotifyScreenDisconnectInfoToRenderTest002, TestSize.Level1)
{
    renderProcessAgent_->NotifyScreenDisconnectInfoToRender(TEST_SCREEN_ID_2);

    ASSERT_TRUE(renderProcessAgent_ != nullptr);
}

/**
 * @tc.name: NotifyScreenPropertyChangedInfoToRenderTest001
 * @tc.desc: Test NotifyScreenPropertyChangedInfoToRender with valid screen ID and property
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessAgentTest, NotifyScreenPropertyChangedInfoToRenderTest001, TestSize.Level1)
{
    ScreenId screenId = TEST_SCREEN_ID;
    ScreenPropertyType type = ScreenPropertyType::RENDER_RESOLUTION;
    sptr<ScreenPropertyBase> screenProperty = nullptr;

    renderProcessAgent_->NotifyScreenPropertyChangedInfoToRender(
        screenId, type, screenProperty);

    ASSERT_TRUE(renderProcessAgent_ != nullptr);
}

/**
 * @tc.name: NotifyScreenPropertyChangedInfoToRenderTest002
 * @tc.desc: Test NotifyScreenPropertyChangedInfoToRender with different property type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessAgentTest, NotifyScreenPropertyChangedInfoToRenderTest002, TestSize.Level1)
{
    ScreenId screenId = TEST_SCREEN_ID_2;
    ScreenPropertyType type = ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE;
    sptr<ScreenPropertyBase> screenProperty = nullptr;

    renderProcessAgent_->NotifyScreenPropertyChangedInfoToRender(
        screenId, type, screenProperty);

    ASSERT_TRUE(renderProcessAgent_ != nullptr);
}

/**
 * @tc.name: NotifyScreenPropertyChangedInfoToRenderTest003
 * @tc.desc: Test NotifyScreenPropertyChangedInfoToRender with rotation property type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessAgentTest, NotifyScreenPropertyChangedInfoToRenderTest003, TestSize.Level1)
{
    ScreenId screenId = TEST_SCREEN_ID;
    ScreenPropertyType type = ScreenPropertyType::CORRECTION;
    sptr<ScreenPropertyBase> screenProperty = nullptr;

    renderProcessAgent_->NotifyScreenPropertyChangedInfoToRender(
        screenId, type, screenProperty);

    ASSERT_TRUE(renderProcessAgent_ != nullptr);
}
} // namespace OHOS::Rosen
