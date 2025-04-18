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
#include "gfx/fps_info/rs_surface_fps_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceFpsManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceFpsManagerTest::SetUpTestCase() {}
void RSSurfaceFpsManagerTest::TearDownTestCase() {}
void RSSurfaceFpsManagerTest::SetUp() {}
void RSSurfaceFpsManagerTest::TearDown() {}

/**
 * @tc.name: RegisterAndUnregister
 * @tc.desc: test results of RegisterAndUnregister
 * @tc.type:FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSSurfaceFpsManagerTest, RegisterAndUnregister, TestSize.Level1)
{
    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    NodeId id = 1000;
    std::string name = "surfacefps0";
    surfaceFpsManager.RegisterSurfaceFps(id, name);
    size_t size = 1;
    EXPECT_EQ(size, surfaceFpsManager.GetSurfaceFpsMap().size());
    surfaceFpsManager.UnregisterSurfaceFps(id);
    size = 0;
    EXPECT_EQ(size, surfaceFpsManager.GetSurfaceFpsMap().size());
}

/**
 * @tc.name: GetSurfaceFps
 * @tc.desc: test results of GetSurfaceFps
 * @tc.type:FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSSurfaceFpsManagerTest, GetSurfaceFps, TestSize.Level1)
{
    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    pid_t pid = 1;
    NodeId id = 1ull << 32;
    std::string name = "surfacefps0";
    pid_t upid = 2;
    NodeId uid = 2ull << 32;
    std::string uname = "surfacefps1";
    bool isUnique = false;
    surfaceFpsManager.RegisterSurfaceFps(id, name);
    EXPECT_NE(nullptr, surfaceFpsManager.GetSurfaceFps(id));
    EXPECT_EQ(nullptr, surfaceFpsManager.GetSurfaceFps(uid));
    EXPECT_NE(nullptr, surfaceFpsManager.GetSurfaceFps(name, isUnique));
    EXPECT_EQ(nullptr, surfaceFpsManager.GetSurfaceFps(uname, isUnique));
    EXPECT_NE(nullptr, surfaceFpsManager.GetSurfaceFpsByPid(pid));
    EXPECT_EQ(nullptr, surfaceFpsManager.GetSurfaceFpsByPid(upid));
    surfaceFpsManager.UnregisterSurfaceFps(id);
}

/**
 * @tc.name: RecordPresentTime
 * @tc.desc: test results of RecordPresentTime
 * @tc.type:FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSSurfaceFpsManagerTest, RecordPresentTime, TestSize.Level1)
{
    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    NodeId id = 1000;
    std::string name = "surfacefps0";
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    NodeId uid = 1001;
    uint32_t seqNum = 0;
    surfaceFpsManager.RegisterSurfaceFps(id, name);
    EXPECT_FALSE(surfaceFpsManager.RecordPresentTime(id, timestamp, seqNum));
    EXPECT_FALSE(surfaceFpsManager.RecordPresentTime(uid, timestamp, seqNum));
    seqNum = 1;
    EXPECT_TRUE(surfaceFpsManager.RecordPresentTime(id, timestamp, seqNum));
    surfaceFpsManager.UnregisterSurfaceFps(id);
}

/**
 * @tc.name: DumpAndClearDump
 * @tc.desc: test results of DumpAndClearDump
 * @tc.type:FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSSurfaceFpsManagerTest, DumpAndClearDump, TestSize.Level1)
{
    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    pid_t pid = 1;
    NodeId id = 1ull << 32;
    std::string name = "surfacefps0";
    pid_t upid = 2;
    NodeId uid = 2ull << 32;
    std::string uname = "surfacefps1";
    surfaceFpsManager.RegisterSurfaceFps(id, name);
    std::string result("");
    surfaceFpsManager.Dump(result, id);
    EXPECT_TRUE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.Dump(result, uid);
    EXPECT_FALSE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.Dump(result, name);
    EXPECT_TRUE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.Dump(result, uname);
    EXPECT_FALSE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.DumpByPid(result, pid);
    EXPECT_TRUE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.DumpByPid(result, upid);
    EXPECT_FALSE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearDump(result, id);
    EXPECT_TRUE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearDump(result, uid);
    EXPECT_FALSE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearDump(result, name);
    EXPECT_TRUE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearDump(result, uname);
    EXPECT_FALSE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearDumpByPid(result, pid);
    EXPECT_TRUE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearDumpByPid(result, upid);
    EXPECT_FALSE(result.find("surface") != std::string::npos);
    result.clear();
    surfaceFpsManager.UnregisterSurfaceFps(id);
}

/**
 * @tc.name: DumpAndClearSurfaceNodeFps
 * @tc.desc: test results of DumpAndClearSurfaceNodeFps
 * @tc.type:FUNC
 * @tc.require: IC1APD
 */
HWTEST_F(RSSurfaceFpsManagerTest, DumpAndClearSurfaceNodeFps, TestSize.Level1)
{
    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    std::string result("");
    surfaceFpsManager.DumpSurfaceNodeFps(result, "-name", "surface");
    EXPECT_TRUE(result.find("fps") != std::string::npos);
    result.clear();
    surfaceFpsManager.DumpSurfaceNodeFps(result, "-id", "123");
    EXPECT_TRUE(result.find("fps") != std::string::npos);
    result.clear();
    surfaceFpsManager.DumpSurfaceNodeFps(result, "-id", "abc");
    EXPECT_TRUE(result.find("nodeId") != std::string::npos);
    result.clear();
    surfaceFpsManager.DumpSurfaceNodeFps(result, "-invalid", "surface");
    EXPECT_TRUE(result.find("must") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearSurfaceNodeFps(result, "-name", "surface");
    EXPECT_TRUE(result.find("fps") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearSurfaceNodeFps(result, "-id", "123");
    EXPECT_TRUE(result.find("fps") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearSurfaceNodeFps(result, "-id", "abc");
    EXPECT_TRUE(result.find("nodeId") != std::string::npos);
    result.clear();
    surfaceFpsManager.ClearSurfaceNodeFps(result, "-invalid", "surface");
    EXPECT_TRUE(result.find("must") != std::string::npos);
    result.clear();
}

/**
 * @tc.name: ProcessParamAndIsSurface
 * @tc.desc: test results of ProcessParamAndIsSurface
 * @tc.type:FUNC
 * @tc.require: IC1APD
 */
HWTEST_F(RSSurfaceFpsManagerTest, ProcessParamAndIsSurface, TestSize.Level1)
{
    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    std::string result("");
    std::unordered_set<std::u16string> surfaceArgSets{u"4234", u"-id"};
    std::unordered_set<std::u16string> displayArgSets{u"DisplayNode", u"-name"};
    std::string option;
    std::string argStr;
    surfaceFpsManager.ProcessParam(surfaceArgSets, option, argStr);
    EXPECT_TRUE(option == "-id");
    EXPECT_TRUE(argStr == "4234");
    EXPECT_TRUE(surfaceFpsManager.IsSurface(option, argStr));
    surfaceFpsManager.ProcessParam(displayArgSets, option, argStr);
    EXPECT_TRUE(option == "-name");
    EXPECT_TRUE(argStr == "DisplayNode");
    EXPECT_FALSE(surfaceFpsManager.IsSurface(option, argStr));
}
}