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

#include "ipc_callbacks/rs_self_drawing_node_rect_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSelfDrawingNodeRectChangeCallbackStubMock : public RSSelfDrawingNodeRectChangeCallbackStub {
public:
    RSSelfDrawingNodeRectChangeCallbackStubMock() = default;
    virtual ~RSSelfDrawingNodeRectChangeCallbackStubMock() = default;
    void OnSelfDrawingNodeRectChange(std::shared_ptr<RSSelfDrawingNodeRectData> data) override {};
};

class RSSelfDrawingNodeRectChangeCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSelfDrawingNodeRectChangeCallbackStubTest::SetUpTestCase() {}
void RSSelfDrawingNodeRectChangeCallbackStubTest::TearDownTestCase() {}
void RSSelfDrawingNodeRectChangeCallbackStubTest::SetUp() {}
void RSSelfDrawingNodeRectChangeCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSelfDrawingNodeRectChangeCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsSelfDrawingNodeRectChangeCallbackStub = std::make_shared<RSSelfDrawingNodeRectChangeCallbackStubMock>();
    uint32_t code = -1;
    int res = rsSelfDrawingNodeRectChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSISelfDrawingNodeRectChangeCallbackInterfaceCode::ON_SELF_DRAWING_NODE_RECT_CHANGED);

    res = rsSelfDrawingNodeRectChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSelfDrawingNodeRectChangeCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsSelfDrawingNodeRectChangeCallbackStub = std::make_shared<RSSelfDrawingNodeRectChangeCallbackStubMock>();
    auto code =
        static_cast<uint32_t>(RSISelfDrawingNodeRectChangeCallbackInterfaceCode::ON_SELF_DRAWING_NODE_RECT_CHANGED);
    data.WriteInterfaceToken(RSISelfDrawingNodeRectChangeCallback::GetDescriptor());

    int res = rsSelfDrawingNodeRectChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
    res = rsSelfDrawingNodeRectChangeCallbackStub->OnRemoteRequest(-1, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

} // namespace OHOS::Rosen
