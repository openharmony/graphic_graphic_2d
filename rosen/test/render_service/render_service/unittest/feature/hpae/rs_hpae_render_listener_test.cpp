/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
#include "gtest/gtest.h"
#include "feature/hpae/rs_hpae_render_listener.h"
#include "pipeline/rs_surface_handler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSHpaeRenderListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHpaeRenderListenerTest::SetUpTestCase() {}
void RSHpaeRenderListenerTest::TearDownTestCase() {}
void RSHpaeRenderListenerTest::SetUp() {}
void RSHpaeRenderListenerTest::TearDown() {}

/**
 * @tc.name: OnBufferAvailableTest
 * @tc.desc: Test OnBufferAvailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeRenderListenerTest, OnBufferAvailableTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceHandler> surfaceHandler1;
    auto rSHpaeRenderListener1 = RSHpaeRenderListener(surfaceHandler1);
    rSHpaeRenderListener1.OnBufferAvailable();

    std::shared_ptr<RSSurfaceHandler> surfaceHandler2 = std::make_shared<RSSurfaceHandler>(0);
    auto rSHpaeRenderListener2 = RSHpaeRenderListener(surfaceHandler2);
    rSHpaeRenderListener2.OnBufferAvailable();
    ASSERT_TRUE(surfaceHandler2->GetAvailableBufferCount() == 1);
}

} // namesapce OHOS::Rosen