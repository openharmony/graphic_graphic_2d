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

#include "feature/capture/rs_ui_capture.h"
#include "transaction/rs_render_service_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "ui/rs_surface_node.h"
#include "surface_utils.h"
#include <iostream>

#include <fcntl.h>  // O_RDONLY
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

#define GTEST_COUT std::cerr << "[CUSTOMDMSG] "
namespace OHOS {
namespace Rosen {
static constexpr uint32_t SET_REFRESHRATE_SLEEP_US = 50000;  // wait for refreshrate change
class RSHrpServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;
    static inline bool profilerEnabled_ = false;
};

void RSHrpServiceTest::SetUpTestCase()
{
    profilerEnabled_ = OHOS::system::GetBoolParameter("persist.graphic.profiler.enabled", false);
    rsClient = std::make_shared<RSRenderServiceClient>();
}
void RSHrpServiceTest::TearDownTestCase()
{
    system::SetParameter("persist.graphic.profiler.enabled", profilerEnabled_ ? "1" : "0");
}
void RSHrpServiceTest::SetUp() {}
void RSHrpServiceTest::TearDown()
{
    usleep(SET_REFRESHRATE_SLEEP_US);
}

/**
 * @tc.name: ProfilerIsSecureScreenTest
 * @tc.desc: ProfilerIsSecureScreenTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerIsSecureScreenTest, TestSize.Level1)
{
    // this file should be compiled with RS_PROFILER_ENABLED
#ifdef RS_PROFILER_ENABLED
    auto res = rsClient->ProfilerIsSecureScreen();
    EXPECT_EQ(res, false);
#else
    ASSERT_FALSE(true);
#endif
}

/**
 * @tc.name: ProfilerServiceOpenFileUnsupported
 * @tc.desc: ProfilerServiceOpenFileUnsupported
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceOpenFileUnsupported, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), false);

    int32_t fd = 0;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};
    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "", 0, fd); // due to early check AI adviced
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_INVALID_PARAM); // due to early check AI adviced
}

/**
 * @tc.name: ProfilerServiceOpenFileUnsupported2
 * @tc.desc: ProfilerServiceOpenFileUnsupported2
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceOpenFileUnsupported2, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), false);

    int32_t fd = 0;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};
    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "filename", 0, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_UNSUPPORTED);
}

/**
 * @tc.name: ProfilerServicePopulateFilesUnsupported
 * @tc.desc: ProfilerServicePopulateFilesUnsupported
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServicePopulateFilesUnsupported, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), false);

    std::vector<HrpServiceFileInfo> outFiles;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN, "", ""};

    auto res = rsClient->ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_UNSUPPORTED);
}

/**
 * @tc.name: ProfilerServiceOpenFileBadDirInfo
 * @tc.desc: ProfilerServiceOpenFileBadDirInfo
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceOpenFileBadDirInfo, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    int32_t fd = 0;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN, "", ""};
    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "", 0, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_INVALID_PARAM);
}

/**
 * @tc.name: ProfilerServicePopulateFilesBadDirInfo
 * @tc.desc: ProfilerServicePopulateFilesBadDirInfo
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServicePopulateFilesBadDirInfo, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    std::vector<HrpServiceFileInfo> outFiles;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN, "", ""};

    auto res = rsClient->ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_INVALID_PARAM);
}

/**
 * @tc.name: ProfilerServiceOpenFileTest
 * @tc.desc: ProfilerServiceOpenFileTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceOpenFileTest, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    int32_t fd = -1;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};
    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "no_such_filename", O_RDONLY, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
}

/**
 * @tc.name: ProfilerServiceOpenFileTest2
 * @tc.desc: ProfilerServiceOpenFileTest2
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceOpenFileTest2, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    int32_t fd = -1;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};
    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "no_such_filename", O_WRONLY, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
}

/**
 * @tc.name: ProfilerServiceOpenFileTest3
 * @tc.desc: ProfilerServiceOpenFileTest3
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceOpenFileTest3, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    int32_t fd = -1;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};
    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "no_such_filename", O_RDWR, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
}

/**
 * @tc.name: ProfilerServiceOpenFileTestInvalidFlag
 * @tc.desc: ProfilerServiceOpenFileTestInvalidFlag
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceOpenFileTestInvalidFlag, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    int32_t fd = -1;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "subdir", "subdir2"};
    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "filename", O_WRONLY | O_RDWR, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_INVALID_PARAM);
    res = rsClient->ProfilerServiceOpenFile(dirInfo, "filename", O_RDWR | 0x80000000, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_INVALID_PARAM);
}

/**
 * @tc.name: ProfilerServicePopulateFilesTest
 * @tc.desc: ProfilerServicePopulateFilesTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServicePopulateFilesTest, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "never_exist_dir", "never_exist_dir2"};
    std::vector<HrpServiceFileInfo> outFiles;
    auto res = rsClient->ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

/**
 * @tc.name: ProfilerServiceComplexFileTest
 * @tc.desc: ProfilerServiceComplexFileTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSHrpServiceTest, ProfilerServiceComplexFileTest, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    EXPECT_EQ(system::GetBoolParameter("persist.graphic.profiler.enabled", false), true);

    int32_t fd = -1, fd2 = -1, fd3 = -1;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};

    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "test_filename1", O_RDWR | O_CREAT, fd);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_SUCCESS);

    res = rsClient->ProfilerServiceOpenFile(dirInfo, "test_filename2", O_WRONLY | O_CREAT, fd2);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_SUCCESS);
   
    res = rsClient->ProfilerServiceOpenFile(dirInfo, "test_filename3", O_RDONLY | O_CREAT, fd3);
    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_SUCCESS);

    GTEST_COUT << "File descriptors are: " << fd << " " << fd2 << " " << fd3 << std::endl;
   
    std::vector<HrpServiceFileInfo> outFiles;
    res = rsClient->ProfilerServicePopulateFiles(dirInfo, 0, outFiles);

    for (auto filename : outFiles) {
        GTEST_COUT << "Filename: " << filename.name << std::endl;
    }

    EXPECT_EQ(res, RetCodeHrpService::RET_HRP_SERVICE_SUCCESS);
    EXPECT_EQ(outFiles.size(), 5);
}
} // namespace Rosen
} // namespace OHOS
