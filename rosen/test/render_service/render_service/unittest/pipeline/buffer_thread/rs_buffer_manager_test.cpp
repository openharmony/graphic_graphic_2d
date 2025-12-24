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

#include "pipeline/buffer_thread/rs_buffer_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBufferManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBufferManagerTest::SetUpTestCase() {}
void RSBufferManagerTest::TearDownTestCase() {}
void RSBufferManagerTest::SetUp() {}
void RSBufferManagerTest::TearDown() {}

HWTEST_F(RSBufferManagerTest, AddPendingReleaseBufferTest001, TestSize.Level1)
{
    auto bufferManager = std::make_shared<RSBufferManager>();
    ASSERT_NE(bufferManager, nullptr);
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(0);
    surfaceHandler->consumer_ = IConsumerSurface::Create("testName");
    auto surfaceBuffer = std::make_shared<RSSurfaceHandler::SurfaceBufferEntry>();
    bufferManager->AddPendingReleaseBuffer(surfaceHandler->consumer_, surfaceBuffer->buffer, SyncFence::InvalidFence());
}

HWTEST_F(RSBufferManagerTest, AddPendingReleaseBufferTest002, TestSize.Level1)
{
    auto bufferManager = std::make_shared<RSBufferManager>();
    ASSERT_NE(bufferManager, nullptr);
    bufferManager->AddPendingReleaseBuffer(0, SyncFence::InvalidFence());
}
}