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

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_protocol.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSAnimationProtocolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSUIContext> rsUIContext;
};

void RSAnimationProtocolTest::SetUpTestCase() {}
void RSAnimationProtocolTest::TearDownTestCase() {}

void RSAnimationProtocolTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
}

void RSAnimationProtocolTest::TearDown() {}

/**
 * @tc.name: SetInterfaceName
 * @tc.desc: Verify the SetInterfaceName of time protocol
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationProtocolTest, SetInterfaceName, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init
     */
    std::string test = "test"; // test string
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetInterfaceName(test);
    EXPECT_EQ(test, timingProtocol.GetInterfaceName());
}

/**
 * @tc.name: RSDummyAnimationRebuildInRender
 * @tc.desc: Verify the RebuildInRender of RSDummyAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationProtocolTest, RSDummyAnimationRebuildInRender, TestSize.Level1)
{
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->RebuildInRender();
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: RSAnimationGetPropertyType
 * @tc.desc: Verify the GetPropertyType of RSAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationProtocolTest, RSAnimationGetPropertyType, TestSize.Level1)
{
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    EXPECT_EQ(animation->GetPropertyType(), ModifierNG::RSPropertyType::INVALID);
}

/**
 * @tc.name: RSAnimationDumpAnimationInfo
 * @tc.desc: Verify the DumpAnimationInfo of RSAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationProtocolTest, RSAnimationDumpAnimationInfo, TestSize.Level1)
{
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    std::string dumpInfo;
    animation->DumpAnimationInfo(dumpInfo);
    EXPECT_NE(animation, nullptr);
}

} // namespace Rosen
} // namespace OHOS
