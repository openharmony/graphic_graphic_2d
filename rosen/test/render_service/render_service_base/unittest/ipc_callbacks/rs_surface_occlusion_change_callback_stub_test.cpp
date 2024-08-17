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

#include "ipc_callbacks/rs_surface_occlusion_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceOcclusionChangeCallbackStubMock : public RSSurfaceOcclusionChangeCallbackStub {
public:
    RSSurfaceOcclusionChangeCallbackStubMock() = default;
    virtual ~RSSurfaceOcclusionChangeCallbackStubMock() = default;
    void OnSurfaceOcclusionVisibleChanged(float visibleAreaRatio) override {};
};

class RSSurfaceOcclusionChangeCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOcclusionChangeCallbackStubTest::SetUpTestCase() {}
void RSSurfaceOcclusionChangeCallbackStubTest::TearDownTestCase() {}
void RSSurfaceOcclusionChangeCallbackStubTest::SetUp() {}
void RSSurfaceOcclusionChangeCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSSurfaceOcclusionChangeCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsSurfaceOcclusionChangeCallbackStub = std::make_shared<RSSurfaceOcclusionChangeCallbackStubMock>();
    uint32_t code = -1;
    int res = rsSurfaceOcclusionChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSISurfaceOcclusionChangeCallbackInterfaceCode::ON_SURFACE_OCCLUSION_VISIBLE_CHANGED);
    EXPECT_TRUE(RSSurfaceOcclusionChangeCallbackStubMock::securityManager_.IsInterfaceCodeAccessible(code));

    res = rsSurfaceOcclusionChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSSurfaceOcclusionChangeCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsSurfaceOcclusionChangeCallbackStub = std::make_shared<RSSurfaceOcclusionChangeCallbackStubMock>();
    auto code =
        static_cast<uint32_t>(RSISurfaceOcclusionChangeCallbackInterfaceCode::ON_SURFACE_OCCLUSION_VISIBLE_CHANGED);
    data.WriteInterfaceToken(RSISurfaceOcclusionChangeCallback::GetDescriptor());
    EXPECT_TRUE(RSSurfaceOcclusionChangeCallbackStubMock::securityManager_.IsInterfaceCodeAccessible(code));

    int res = rsSurfaceOcclusionChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}

} // namespace OHOS::Rosen
