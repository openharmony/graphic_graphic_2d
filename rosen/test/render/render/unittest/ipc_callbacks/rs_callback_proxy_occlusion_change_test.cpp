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

#include "ipc_callbacks/rs_occlusion_change_callback_proxy.h"
#include "mock_iremoter_objects.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSCallbackProxyOcclusionChangeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCallbackProxyOcclusionChangeTest::SetUpTestCase() {}
void RSCallbackProxyOcclusionChangeTest::TearDownTestCase() {}
void RSCallbackProxyOcclusionChangeTest::SetUp() {}
void RSCallbackProxyOcclusionChangeTest::TearDown() {}

/**
 * @tc.name: OnOcclusionVisibleChanged001
 * @tc.desc: Verify the OnOcclusionVisibleChanged
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyOcclusionChangeTest, OnOcclusionVisibleChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto occlusionChangeCallbackProxy = std::make_shared<RSOcclusionChangeCallbackProxy>(remoteMocker);
    ASSERT_TRUE(occlusionChangeCallbackProxy != nullptr);
    occlusionChangeCallbackProxy->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>());
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIOcclusionChangeCallbackInterfaceCode::ON_OCCLUSION_VISIBLE_CHANGED));
}

/**
 * @tc.name: OnOcclusionVisibleChanged002
 * @tc.desc: Verify the OnOcclusionVisibleChanged
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyOcclusionChangeTest, OnOcclusionVisibleChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto occlusionChangeCallbackProxy->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>());
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIOcclusionChangeCallbackInterfaceCode::ON_OCCLUSION_VISIBLE_CHANGED));
    occlusionChangeCallbackProxy = std::make_shared<RSOcclusionChangeCallbackProxy>(remoteMocker);
    ASSERT_TRUE(occlusionChangeCallbackProxy != nullptr);
}

/**
 * @tc.name: OnOcclusionVisibleChanged003
 * @tc.desc: Verify the OnOcclusionVisibleChanged
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyOcclusionChangeTest, OnOcclusionVisibleChanged003, TestSize.Level1)
{
    auto occlusionChangeCallbackProxy = std::make_shared<RSOcclusionChangeCallbackProxy>(nullptr);
    ASSERT_TRUE(occlusionChangeCallbackProxy != nullptr);
    occlusionChangeCallbackProxy->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>());
}
} // namespace Rosen
} // namespace OHOS
