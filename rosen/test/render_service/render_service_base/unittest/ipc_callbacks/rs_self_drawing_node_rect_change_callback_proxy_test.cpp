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

#include "ipc_callbacks/rs_self_drawing_node_rect_change_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr auto code =
    static_cast<uint32_t>(RSISelfDrawingNodeRectChangeCallbackInterfaceCode::ON_SELF_DRAWING_NODE_RECT_CHANGED);
class RSSelfDrawingNodeRectChangeCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSelfDrawingNodeRectChangeCallbackProxyTest::SetUpTestCase() {}
void RSSelfDrawingNodeRectChangeCallbackProxyTest::TearDownTestCase() {}
void RSSelfDrawingNodeRectChangeCallbackProxyTest::SetUp() {}
void RSSelfDrawingNodeRectChangeCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnSelfDrawingNodeRectChange001
 * @tc.desc: Verify the OnSelfDrawingNodeRectChange
 * @tc.type:FUNC
 */
HWTEST_F(RSSelfDrawingNodeRectChangeCallbackProxyTest, OnSelfDrawingNodeRectChange001, TestSize.Level1)
{
    auto remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    auto SelfDrawingNodeRectChangeCallbackProxy =
        std::make_shared<RSSelfDrawingNodeRectChangeCallbackProxy>(remoteMocker);
    ASSERT_NE(SelfDrawingNodeRectChangeCallbackProxy, nullptr);
    SelfDrawingNodeRectChangeCallbackProxy->OnSelfDrawingNodeRectChange(std::make_shared<RSSelfDrawingNodeRectData>());
    ASSERT_EQ(code, remoteMocker->receivedCode_);
}

/**
 * @tc.name: OnSelfDrawingNodeRectChange002
 * @tc.desc: Verify the OnSelfDrawingNodeRectChange when SendRequest returns error
 * @tc.type:FUNC
 */
HWTEST_F(RSSelfDrawingNodeRectChangeCallbackProxyTest, OnSelfDrawingNodeRectChange002, TestSize.Level1)
{
    auto remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto SelfDrawingNodeRectChangeCallbackProxy =
        std::make_shared<RSSelfDrawingNodeRectChangeCallbackProxy>(remoteMocker);
    ASSERT_NE(SelfDrawingNodeRectChangeCallbackProxy, nullptr);
    SelfDrawingNodeRectChangeCallbackProxy->OnSelfDrawingNodeRectChange(std::make_shared<RSSelfDrawingNodeRectData>());
    ASSERT_EQ(code, remoteMocker->receivedCode_);
}
} // namespace Rosen
} // namespace OHOS
