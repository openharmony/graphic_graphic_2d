/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "pipeline/parallel_render/rs_ui_first_render_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsUIFirstRenderListener : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsUIFirstRenderListener::SetUpTestCase() {}
void RsUIFirstRenderListener::TearDownTestCase() {}
void RsUIFirstRenderListener::SetUp() {}
void RsUIFirstRenderListener::TearDown() {}

/**
 * @tc.name: OnBufferAvailable01
 * @tc.desc: Test OnBufferAvailable
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsUIFirstRenderListener, OnBufferAvailable01, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceHandler> handler = nullptr;
    std::weak_ptr<RSSurfaceHandler> surfaceHandler = handler;
    RSUIFirstRenderListener listener(surfaceHandler);
    listener.OnBufferAvailable();
    EXPECT_FALSE(handler);

    std::shared_ptr<RSSurfaceHandler> sharedSurfaceHandler = std::make_shared<RSSurfaceHandler>(0);
    listener.surfaceHandler_ = sharedSurfaceHandler;
    listener.OnBufferAvailable();
    EXPECT_TRUE(sharedSurfaceHandler);
}
}