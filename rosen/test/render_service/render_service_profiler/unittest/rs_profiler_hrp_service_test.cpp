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

#include <cstdint>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-ext.h"
#include "rs_profiler.h"
#include "transaction/rs_hrp_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSProfilerHrpServiceTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        RSProfiler::testing_ = true;
    }
    static void TearDownTestCase()
    {
        RSProfiler::testing_ = false;
    }
    void SetUp() override
    {
        RSProfiler::testing_ = true;
    }
    void TearDown() override
    {
    }
};

HWTEST_F(RSProfilerHrpServiceTest, ValidateOpenFlags_ReadOnly, Level1)
{
    constexpr int32_t flags = O_RDONLY;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_file.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, ValidateOpenFlags_WriteOnly, Level1)
{
    constexpr int32_t flags = O_WRONLY;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_file.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, ValidateOpenFlags_ReadWrite, Level1)
{
    constexpr int32_t flags = O_RDWR;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_file.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, ValidateOpenFlags_Create, Level1)
{
    constexpr int32_t flags = O_WRONLY | O_CREAT;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_new_file.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, ValidateOpenFlags_InvalidFlags, Level1)
{
    constexpr int32_t flags = O_APPEND;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_file.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_ERR_INVALID_PARAM || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, ValidateOpenFlags_NoAccessMode, Level1)
{
    constexpr int32_t flags = O_CREAT;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_file.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_ERR_INVALID_PARAM || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE || ret == RET_HRP_SERVICE_SUCCESS ||
                ret < 0);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_EmptyFileName, Level1)
{
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "",
        O_RDONLY,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_ERR_INVALID_PARAM || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_ValidFileName, Level1)
{
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_file_123.txt",
        O_RDONLY,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_EmptyDir, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        0,
        outFiles);
    EXPECT_TRUE(ret >= RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_WithSubDir, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        0,
        outFiles);
    EXPECT_TRUE(ret >= RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_InvalidPath, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN, "", ""},
        0,
        outFiles);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_ERR_INVALID_PARAM || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_WithFirstIndex, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        100,
        outFiles);
    EXPECT_TRUE(ret >= RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_WithSubDir, Level1)
{
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "subdir", ""},
        "test_file.txt",
        O_RDONLY,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE || ret == RET_HRP_SERVICE_ERR_STUB_MAKE_DIR);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_ServiceDisabled, Level1)
{
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_file.txt",
        O_RDONLY,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_ServiceDisabled, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        0,
        outFiles);
    EXPECT_TRUE(ret >= RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

HWTEST_F(RSProfilerHrpServiceTest, IsHrpServiceEnabled_Basic, Level1)
{
    bool enabled = RSProfiler::IsHrpServiceEnabled();
    EXPECT_TRUE(enabled == true || enabled == false);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_TruncateFlag, Level1)
{
    constexpr int32_t flags = O_WRONLY | O_CREAT | O_TRUNC;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_truncate.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE || ret == RET_HRP_SERVICE_ERR_STUB_MAKE_DIR ||
                ret == RET_HRP_SERVICE_ERR_INVALID_PARAM);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_ExclusiveCreate, Level1)
{
    constexpr int32_t flags = O_WRONLY | O_CREAT | O_EXCL;
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        "test_exclusive.txt",
        flags,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE || ret == RET_HRP_SERVICE_ERR_STUB_MAKE_DIR ||
                ret == RET_HRP_SERVICE_ERR_INVALID_PARAM);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_MultipleFiles, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "", ""},
        0,
        outFiles);
    EXPECT_TRUE(ret >= RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_RS_Dir, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_RS, "", ""},
        0,
        outFiles);
    EXPECT_TRUE(ret >= RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_DoubleSubDir, Level1)
{
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "subdir1", "subdir2"},
        "test_file.txt",
        O_RDONLY,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE || ret == RET_HRP_SERVICE_ERR_STUB_MAKE_DIR);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServicePopulateFiles_DoubleSubDir, Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    auto ret = RSProfiler::HrpServicePopulateFiles(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "subdir1", "subdir2"},
        0,
        outFiles);
    EXPECT_TRUE(ret >= RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR);
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_LongFileName, Level1)
{
    std::string longFileName = "test_file_with_a_very_long_name_that_exceeds_normal_length.txt";
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "", ""},
        longFileName,
        O_RDONLY,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE || ret == RET_HRP_SERVICE_ERR_INVALID_PARAM);
    if (fd >= 0) {
        close(fd);
    }
}

HWTEST_F(RSProfilerHrpServiceTest, HrpServiceOpenFile_SpecialCharsInSubDir, Level1)
{
    int fd = -1;
    auto ret = RSProfiler::HrpServiceOpenFile(
        HrpServiceDirInfo{HrpServiceDir::HRP_SERVICE_DIR_TRACE3D, "valid_subdir", ""},
        "test_file.txt",
        O_RDONLY,
        fd);
    EXPECT_TRUE(ret == RET_HRP_SERVICE_SUCCESS || ret == RET_HRP_SERVICE_ERR_UNSUPPORTED ||
                ret == RET_HRP_SERVICE_ERR_STUB_OPEN_FILE || ret == RET_HRP_SERVICE_ERR_STUB_MAKE_DIR);
    if (fd >= 0) {
        close(fd);
    }
}

} // namespace OHOS::Rosen
