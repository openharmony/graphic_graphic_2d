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

#include "ipc_callbacks/rs_uiextension_callback_proxy.h"
#include "mock_iremoter_objects.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr auto code = static_cast<uint32_t>(RSCallbackIUIExtensionInterfaceCode::ON_UIEXTENSION);
constexpr uint64_t userId = 0;
class RSCallbackProxyUIExtensionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCallbackProxyUIExtensionTest::SetUpTestCase() {}
void RSCallbackProxyUIExtensionTest::TearDownTestCase() {}
void RSCallbackProxyUIExtensionTest::SetUp() {}
void RSCallbackProxyUIExtensionTest::TearDown() {}

/**
 * @tc.name: OnUIExtension001
 * @tc.desc: Verify the OnUIExtension
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyUIExtensionTest, OnUIExtension001, TestSize.Level1)
{
    auto remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    auto onUIExtensionCallbackProxy = std::make_shared<RSUIExtensionCallbackProxy>(remoteMocker);
    ASSERT_NE(onUIExtensionCallbackProxy, nullptr);
    onUIExtensionCallbackProxy->OnUIExtension(std::make_shared<RSUIExtensionData>(), userId);
    ASSERT_EQ(code, remoteMocker->receivedCode_);
}

/**
 * @tc.name: OnUIExtension002
 * @tc.desc: Verify the OnUIExtension when remote object is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyUIExtensionTest, OnUIExtension003, TestSize.Level1)
{
    auto onUIExtensionCallbackProxy = std::make_shared<RSUIExtensionCallbackProxy>(nullptr);
    ASSERT_NE(onUIExtensionCallbackProxy, nullptr);
    onUIExtensionCallbackProxy->OnUIExtension(std::make_shared<RSUIExtensionData>(), userId);
}

/**
 * @tc.name: OnUIExtension003
 * @tc.desc: Verify the OnUIExtension when SendRequest returns error
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyUIExtensionTest, OnUIExtension002, TestSize.Level1)
{
    auto remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto onUIExtensionCallbackProxy = std::make_shared<RSUIExtensionCallbackProxy>(remoteMocker);
    ASSERT_NE(onUIExtensionCallbackProxy, nullptr);
    onUIExtensionCallbackProxy->OnUIExtension(std::make_shared<RSUIExtensionData>(), userId);
    ASSERT_EQ(code, remoteMocker->receivedCode_);
}
} // namespace Rosen
} // namespace OHOS
