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
 * @tc.name: SetShouldCollectBuffers001
 * @tc.desc: Test SetShouldCollectBuffers with true
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, SetShouldCollectBuffers001, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    EXPECT_TRUE(RSSurfaceBufferCallbackManager::Instance().ShouldCollectBuffers());
}

/**
 * @tc.name: SetShouldCollectBuffers002
 * @tc.desc: Test SetShouldCollectBuffers with false
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, SetShouldCollectBuffers002, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(false);
    EXPECT_FALSE(RSSurfaceBufferCallbackManager::Instance().ShouldCollectBuffers());
}

/**
 * @tc.name: SetShouldCollectBuffers003
 * @tc.desc: Test SetShouldCollectBuffers toggle with atomic operations
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, SetShouldCollectBuffers003, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(false);
    EXPECT_FALSE(RSSurfaceBufferCallbackManager::Instance().ShouldCollectBuffers());
    
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    EXPECT_TRUE(RSSurfaceBufferCallbackManager::Instance().ShouldCollectBuffers());
    
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(false);
    EXPECT_FALSE(RSSurfaceBufferCallbackManager::Instance().ShouldCollectBuffers());
}

/**
 * @tc.name: StoreSurfaceBufferInfo001
 * @tc.desc: Test StoreSurfaceBufferInfo with null buffer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, StoreSurfaceBufferInfo001, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    DrawingSurfaceBufferInfo info;
    info.surfaceBuffer_ = nullptr;
    info.pid_ = 1001;
    info.uid_ = 10001;
    
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(1001);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: StoreSurfaceBufferInfo002
 * @tc.desc: Test StoreSurfaceBufferInfo when collection is disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, StoreSurfaceBufferInfo002, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(false);
    
    sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
    buffer->SetWidth(100);
    buffer->SetHeight(100);
    buffer->SetFormat(1);
    buffer->SetStride(400);
    
    DrawingSurfaceBufferInfo info;
    info.surfaceBuffer_ = buffer;
    info.pid_ = 1002;
    info.uid_ = 10002;
    
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(1002);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: StoreSurfaceBufferInfo003
 * @tc.desc: Test StoreSurfaceBufferInfo with valid buffer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, StoreSurfaceBufferInfo003, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
    buffer->SetWidth(100);
    buffer->SetHeight(100);
    buffer->SetFormat(1);
    buffer->SetStride(400);
    
    DrawingSurfaceBufferInfo info;
    info.surfaceBuffer_ = buffer;
    info.pid_ = 1003;
    info.uid_ = 10003;
    
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(1003);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].pid_, 1003);
    EXPECT_EQ(result[0].uid_, 10003);
}

/**
 * @tc.name: StoreSurfaceBufferInfo004
 * @tc.desc: Test StoreSurfaceBufferInfo with multiple buffers for same process
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, StoreSurfaceBufferInfo004, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t testPid = 1004;
    uint64_t testUid = 10004;
    
    for (int32_t i = 0; i < 5; i++) {
        sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
        buffer->SetWidth(100);
        buffer->SetHeight(100);
        buffer->SetFormat(1);
        buffer->SetStride(400);
        
        DrawingSurfaceBufferInfo info;
        info.surfaceBuffer_ = buffer;
        info.pid_ = testPid;
        info.uid_ = testUid;
        
        RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    }
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result.size(), 5);
}
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result.size(), 5);
}

/**
 * @tc.name: StoreSurfaceBufferInfo005
 * @tc.desc: Test StoreSurfaceBufferInfo LRU mechanism (max 10 buffers)
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, StoreSurfaceBufferInfo005, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t testPid = 1005;
    uint64_t testUid = 10005;
    
    for (int32_t i = 0; i < 15; i++) {
        sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
        buffer->SetWidth(100);
        buffer->SetHeight(100);
        buffer->SetFormat(1);
        buffer->SetStride(400);
        
        DrawingSurfaceBufferInfo info;
        info.surfaceBuffer_ = buffer;
        info.pid_ = testPid;
        info.uid_ = testUid;
        
        RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    }
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result.size(), 10);
}

/**
 * @tc.name: StoreSurfaceBufferInfo006
 * @tc.desc: Test StoreSurfaceBufferInfo with duplicate buffer (move to front)
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, StoreSurfaceBufferInfo006, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t testPid = 1006;
    uint64_t testUid = 10006;
    
    sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
    buffer->SetWidth(100);
    buffer->SetHeight(100);
    buffer->SetSetSeqNum(12345);
    
    DrawingSurfaceBufferInfo info;
    info.surfaceBuffer_ = buffer;
    info.pid_ = testPid;
    info.uid_ = testUid;
    
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result1.size(), 1);
    
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result2.size(), 1);
}

/**
 * @tc.name: GetSurfaceBufferInfoByPid001
 * @tc.desc: Test GetSurfaceBufferInfoByPid with non-existent pid
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, GetSurfaceBufferInfoByPid001, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(9999);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: GetSurfaceBufferInfoByPid002
 * @tc.desc: Test GetSurfaceBufferInfoByPid with existing pid
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, GetSurfaceBufferInfoByPid002, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t testPid = 1007;
    uint64_t testUid = 10007;
    
    sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
    buffer->SetWidth(200);
    buffer->SetHeight(150);
    
    DrawingSurfaceBufferInfo info;
    info.surfaceBuffer_ = buffer;
    info.pid_ = testPid;
    info.uid_ = testUid;
    
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].pid_, testPid);
    EXPECT_EQ(result[0].uid_, testUid);
    EXPECT_NE(result[0].surfaceBuffer_, nullptr);
}

/**
 * @tc.name: GetSurfaceBufferInfoByPid003
 * @tc.desc: Test GetSurfaceBufferInfoByPid with multiple processes
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, GetSurfaceBufferInfoByPid003, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t pid1 = 1008;
    pid_t pid2 = 1009;
    uint64_t uid1 = 10008;
    uint64_t uid2 = 10009;
    
    sptr<SurfaceBuffer> buffer1 = new SurfaceBuffer();
    DrawingSurfaceBufferInfo info1;
    info1.surfaceBuffer_ = buffer1;
    info1.pid_ = pid1;
    info1.uid_ = uid1;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info1);
    
    sptr<SurfaceBuffer> buffer2 = new SurfaceBuffer();
    DrawingSurfaceBufferInfo info2;
    info2.surfaceBuffer_ = buffer2;
    info2.pid_ = pid2;
    info2.uid_ = uid2;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info2);
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(pid1);
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(pid2);
    
    EXPECT_EQ(result1.size(), 1);
    EXPECT_EQ(result2.size(), 1);
    EXPECT_EQ(result1[0].pid_, pid1);
    EXPECT_EQ(result2[0].pid_, pid2);
}

/**
 * @tc.name: RemoveSurfaceBufferInfo001
 * @tc.desc: Test RemoveSurfaceBufferInfo with non-existent buffer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, RemoveSurfaceBufferInfo001, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    RSSurfaceBufferCallbackManager::Instance().RemoveSurfaceBufferInfo(99999);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(1010);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: RemoveSurfaceBufferInfo002
 * @tc.desc: Test RemoveSurfaceBufferInfo with existing buffer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, RemoveSurfaceBufferInfo002, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t testPid = 1010;
    uint64_t testUid = 10010;
    
    sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
    buffer->SetSetSeqNum(54321);
    
    DrawingSurfaceBufferInfo info;
    info.surfaceBuffer_ = buffer;
    info.pid_ = testPid;
    info.uid_ = testUid;
    
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result1.size(), 1);
    
    RSSurfaceBufferCallbackManager::Instance().RemoveSurfaceBufferInfo(54321);
    
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result2.size(), 0);
}

/**
 * @tc.name: RemoveAllSurfaceBufferInfo001
 * @tc.desc: Test RemoveAllSurfaceBufferInfo with non-existent pid/uid
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, RemoveAllSurfaceBufferInfo001, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    RSSurfaceBufferCallbackManager::Instance().RemoveAllSurfaceBufferInfo(9999, 99999);
    
    auto result = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(1011);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: RemoveAllSurfaceBufferInfo002
 * @tc.desc: Test RemoveAllSurfaceBufferInfo with existing pid/uid
 * *tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, RemoveAllSurfaceBufferInfo002, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t testPid = 1011;
    uint64_t testUid = 10011;
    
    for (int32_t i = 0; i < 5; i++) {
        sptr<SurfaceBuffer> buffer = new SurfaceBuffer();
        DrawingSurfaceBufferInfo info;
        info.surfaceBuffer_ = buffer;
        info.pid_ = testPid;
        info.uid_ = testUid;
        RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info);
    }
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result1.size(), 5);
    
    RSSurfaceBufferCallbackManager::Instance().RemoveAllSurfaceBufferInfo(testPid, testUid);
    
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(testPid);
    EXPECT_EQ(result2.size(), 0);
}

/**
 * @tc.name: RemoveAllSurfaceBufferInfo003
 * @tc.desc: Test RemoveAllSurfaceBufferInfo doesn't affect other processes
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceBufferCallbackManagerTest, RemoveAllSurfaceBufferInfo003, TestSize.Level1)
{
    RSSurfaceBufferCallbackManager::Instance().SetShouldCollectBuffers(true);
    
    pid_t pid1 = 1012;
    pid_t pid2 = 1013;
    uint64_t uid1 = 10012;
    uint64_t uid2 = 10013;
    
    sptr<SurfaceBuffer> buffer1 = new SurfaceBuffer();
    DrawingSurfaceBufferInfo info1;
    info1.surfaceBuffer_ = buffer1;
    info1.pid_ = pid1;
    info1.uid_ = uid1;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info1);
    
    sptr<SurfaceBuffer> buffer2 = new SurfaceBuffer();
    DrawingSurfaceBufferInfo info2;
    info2.surfaceBuffer_ = buffer2;
    info2.pid_ = pid2;
    info2.uid_ = uid2;
    RSSurfaceBufferCallbackManager::Instance().StoreSurfaceBufferInfo(info2);
    
    RSSurfaceBufferCallbackManager::Instance().RemoveAllSurfaceBufferInfo(pid1, uid1);
    
    auto result1 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(pid1);
    auto result2 = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(pid2);
    
    EXPECT_EQ(result1.size(), 0);
    EXPECT_EQ(result2.size(), 1);
}

} // namespace Rosen
} // namespace OHOS
