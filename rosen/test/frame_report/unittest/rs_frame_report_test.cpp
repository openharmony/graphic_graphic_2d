/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <string>
#ifdef RS_ENABLE_VK
#include <dlfcn.h>
#include <gmock/gmock.h>
#endif

#include "rs_frame_report.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsFrameReportTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameReportTest::SetUpTestCase() {}
void RsFrameReportTest::TearDownTestCase() {}
void RsFrameReportTest::SetUp() {}
void RsFrameReportTest::TearDown() {}
#ifdef RS_ENABLE_VK
class MockDlopen {
public:
    MOCK_METHOD(void*, dlopen, (const char* filename, int flag));
};

class MockDlsym {
public:
    MOCK_METHOD(void*, dlsym, (void* restrict, const char* funcname));
};
#endif

/**
 * @tc.name: NoBranchTestCase001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, NoBranchTestCase, TestSize.Level1)
{
    ASSERT_FALSE(RsFrameReport::IsInitSchedCompleted());
    RsFrameReport::InitDeadline();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
    
    RsFrameReport::SetFrameParam(0, 0, 0, 0);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::SendCommandsStart();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::RenderStart(1, 1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::RenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::DirectRenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::UniRenderStart();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::UniRenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckUnblockMainThreadPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckPostAndWaitPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckBeginFlushPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::ReportComposerInfo(1, 1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::ReportDDGRTaskInfo();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportScbSceneInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportScbSceneInfo001, TestSize.Level1)
{
    std::string description = "test";
    bool eventStatus = true;
    RsFrameReport::ReportScbSceneInfo(description, eventStatus);
 
    eventStatus = false;
    RsFrameReport::ReportScbSceneInfo(description, eventStatus);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportFrameDeadline001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportFrameDeadline001, TestSize.Level1)
{
    int deadline = 8333;
    uint32_t currentRate = 120;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
 
    deadline = -8333;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
 
    deadline = 0;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportBufferCount001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportBufferCount001, TestSize.Level1)
{
    RsFrameReport::ReportBufferCount(1);
    RsFrameReport::ReportBufferCount(1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportUnmarshalData001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportUnmarshalData001, TestSize.Level1)
{
    int unmarshalTid = 123456;
    size_t dataSize = 20 * 1024;
    RsFrameReport::ReportUnmarshalData(unmarshalTid, dataSize);
 
    unmarshalTid = 0;
    RsFrameReport::ReportUnmarshalData(unmarshalTid, dataSize);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: InitializeVulkanExtensions001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions001, TestSize.Level1)
{
    RsFrameReport::isInit.store(true);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
    bool result = RsFrameReport::InitializeVulkanExtensions(nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: InitializeVulkanExtensions002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions002, TestSize.Level1)
{
    RsFrameReport::isInit.store(true);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = reinterpret_cast<PFN_vkSetFrontWindowStatusHUAWEI>(0x1234);
    bool result = RsFrameReport::InitializeVulkanExtensions(nullptr);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: InitializeVulkanExtensions003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions003, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = reinterpret_cast<PFN_vkSetFrontWindowStatusHUAWEI>(0x1234);
    RsFrameReport::InitializeVulkanExtensions(nullptr);
    EXPECT_FALSE(RsFrameReport::isInit.load());
}

/**
 * @tc.name: InitializeVulkanExtensions004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions004, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
    bool result = RsFrameReport::InitializeVulkanExtensions(nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: InitializeVulkanExtensions005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions005, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
    MockDlsym mockDlsym;
    RsFrameReport::dlsymFunc = [&mockDlsym](void* restrict, const char* funcname) {
        return mockDlsym.dlsym(restrict, funcname);
    };
    EXPECT_CALL(mockDlsym, dlsym(testing::_, testing::_)).WillOnce(testing::Return(nullptr));
    EXPECT_FALSE(RsFrameReport::InitializeVulkanExtensions(reinterpret_cast<VkDevice>(1)));
}

/**
 * @tc.name: InitializeVulkanExtensions006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions006, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
    MockDlopen mockDlopen;
    RsFrameReport::dlopenFunc = [&mockDlopen](const char* filename, int flag) {
        return mockDlopen.dlopen(filename, flag);
    };
    EXPECT_CALL(mockDlopen, dlopen(testing::_, testing::_)).WillOnce(testing::Return(nullptr));
    EXPECT_FALSE(RsFrameReport::InitializeVulkanExtensions(reinterpret_cast<VkDevice>(1)));
}

/**
 * @tc.name: ReportWindowInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportWindowInfo001, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
    RsFrameReport::ReportWindowInfo(nullptr, false, "com.test.app");
    EXPECT_FALSE(RsFrameReport::isInit.load());
}

/**
 * @tc.name: VkHandleDeleter001
 * @tc.desc: test Nullptr Do Not Call_dlclose
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, VkHandleDeleter001, TestSize.Level1)
{
    RsFrameReport::VkHandleDeleter deleter;
    deleter(nullptr);
    EXPECT_FALSE(RsFrameReport::isInit.load());
}
#endif
} // namespace Rosen
} // namespace OHOS
