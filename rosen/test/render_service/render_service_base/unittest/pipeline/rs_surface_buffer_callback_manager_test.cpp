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

#ifdef ROSEN_OHOS

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_draw_cmd.h"
#include "surface_buffer.h"
#include "surface_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSurfaceBufferCallbackManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceBufferCallbackManagerTest::SetUpTestCase() {}
void RSSurfaceBufferCallbackManagerTest::TearDownTestCase() {}
void RSSurfaceBufferCallbackManagerTest::SetUp() {}
void RSSurfaceBufferCallbackManagerTest::TearDown() {}

/**
 * @tc.name: UnregisterSurfaceBufferCallback001
 * @tc.desc: Test UnregisterSurfaceBufferCallback(pid_t) clears buffer info
 * @tc.type: FUNC
 * @tc.require: issue28175
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, UnregisterSurfaceBufferCallback001, TestSize.Level1)
{
    pid_t testPid = 2000;
    uint64_t testUid1 = 20001;
    uint64_t testUid2 = 20002;
    
    sptr<SurfaceBuffer> buffer1 = SurfaceBuffer::Create();
    DrawingSurfaceBufferInfo info1;
    info1.surfaceBuffer_ = buffer1;
    info1.pid_ = testPid;
    info1.uid_ = testUid1;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info1);
    
    sptr<SurfaceBuffer> buffer2 = SurfaceBuffer::Create();
    DrawingSurfaceBufferInfo info2;
    info2.surfaceBuffer_ = buffer2;
    info2.pid_ = testPid;
    info2.uid_ = testUid2;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info2);
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result1.size(), 2);
    
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(testPid);
    
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result2.size(), 0);
}

/**
 * @tc.name: UnregisterSurfaceBufferCallback002
 * @tc.desc: Test UnregisterSurfaceBufferCallback(pid_t) doesn't affect other processes
 * @tc.type: FUNC
 * @tc.require: issue28175
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, UnregisterSurfaceBufferCallback002, TestSize.Level1)
{
    pid_t pid1 = 2001;
    pid_t pid2 = 2002;
    uint64_t uid1 = 20011;
    uint64_t uid2 = 20021;
    
    sptr<SurfaceBuffer> buffer1 = SurfaceBuffer::Create();
    DrawingSurfaceBufferInfo info1;
    info1.surfaceBuffer_ = buffer1;
    info1.pid_ = pid1;
    info1.uid_ = uid1;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info1);
    
    sptr<SurfaceBuffer> buffer2 = SurfaceBuffer::Create();
    DrawingSurfaceBufferInfo info2;
    info2.surfaceBuffer_ = buffer2;
    info2.pid_ = pid2;
    info2.uid_ = uid2;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info2);
    
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid1);
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(pid1);
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(pid2);
    
    EXPECT_EQ(result1.size(), 0);
    EXPECT_EQ(result2.size(), 1);
}

/**
 * @tc.name: UnregisterSurfaceBufferCallback003
 * @tc.desc: Test UnregisterSurfaceBufferCallback(pid_t, uid) clears buffer info
 * @tc.type: FUNC
 * @tc.require: issue28175
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, UnregisterSurfaceBufferCallback003, TestSize.Level1)
{
    pid_t testPid = 2003;
    uint64_t testUid = 20031;
    
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    DrawingSurfaceBufferInfo info;
    info.surfaceBuffer_ = buffer;
    info.pid_ = testPid;
    info.uid_ = testUid;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result1.size(), 1);
    
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(testPid, testUid);
    
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result2.size(), 0);
}

/**
 * @tc.name: UnregisterSurfaceBufferCallback004
 * @tc.desc: Test UnregisterSurfaceBufferCallback(pid_t, uid) doesn't affect other uids
 * @tc.type: FUNC
 * @tc.require: issue28175
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, UnregisterSurfaceBufferCallback004, TestSize.Level1)
{
    pid_t testPid = 2004;
    uint64_t uid1 = 20041;
    uint64_t uid2 = 20042;
    
    sptr<SurfaceBuffer> buffer1 = SurfaceBuffer::Create();
    DrawingSurfaceBufferInfo info1;
    info1.surfaceBuffer_ = buffer1;
    info1.pid_ = testPid;
    info1.uid_ = uid1;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info1);
    
    sptr<SurfaceBuffer> buffer2 = SurfaceBuffer::Create();
    DrawingSurfaceBufferInfo info2;
    info2.surfaceBuffer_ = buffer2;
    info2.pid_ = testPid;
    info2.uid_ = uid2;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info2);
    
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(testPid, uid1);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].uid_, uid2);
}

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_OHOS
