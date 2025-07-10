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

#include <dirent.h> // opendir
#include <fcntl.h>
#include <sys/stat.h> // mkdir

#include "rs_profiler.h"

#include "platform/common/rs_system_properties.h"

namespace OHOS::Rosen {

static RetCodeHrpService ValidateOpenFlags(int32_t flags)
{
    constexpr int32_t allowedFlags = O_RDONLY | O_WRONLY | O_RDWR | O_CREAT;
    if (flags & ~allowedFlags) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }
    int accessMode = flags & (O_RDONLY | O_WRONLY | O_RDWR);
    if (accessMode != O_RDONLY && accessMode != O_WRONLY && accessMode != O_RDWR) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }
    return RET_HRP_SERVICE_SUCCESS;
}

static bool IsDir(const struct stat& st)
{
    bool isdir = S_ISDIR(st.st_mode);
    return isdir;
}

static RetCodeHrpService CreateSubDirs(std::string& path,
    const std::vector<const std::string*>& subDirs)
{
    for (const auto* d : subDirs) {
        if (!(*d).length()) {
            continue;
        }
        path += "/" + (*d);
        constexpr int directoryPermission = 0755;
        struct stat pathStat;
        if (stat(path.c_str(), &pathStat) == 0) {
            if (IsDir(pathStat)) {
                continue;
            } else {
                return RET_HRP_SERVICE_ERR_STUB_PATH_EXISTS_NOT_DIR;
            }
        }
        if (mkdir(path.c_str(), directoryPermission) != -1) {
            continue;
        }
        if (errno != EEXIST) {
            return errno == EACCES ? RET_HRP_SERVICE_ERR_STUB_MAKE_DIR_ACCESS_DENIED
                                   : RET_HRP_SERVICE_ERR_STUB_MAKE_DIR;
        }
    }
    return RET_HRP_SERVICE_SUCCESS;
}

RetCodeHrpService RSProfiler::HrpServiceOpenFile(const HrpServiceDirInfo& dirInfo,
    const std::string& fileName, int32_t flags, int& outFd)
{
    if (!IsHrpServiceEnabled() && !RSSystemProperties::GetProfilerEnabled()) {
        return RET_HRP_SERVICE_ERR_UNSUPPORTED;
    }
    std::string path = HrpServiceGetDirFull(dirInfo.baseDirType);
    if (fileName.length() == 0 || path.length() == 0 || !HrpServiceValidDirOrFileName(fileName)
            || !HrpServiceValidDirOrFileName(dirInfo.subDir) || !HrpServiceValidDirOrFileName(dirInfo.subDir2)) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }

    RetCodeHrpService flagCheck = ValidateOpenFlags(flags);
    if (flagCheck != RET_HRP_SERVICE_SUCCESS) {
        return flagCheck;
    }

    std::vector<const std::string*> subDirs = { &dirInfo.subDir, &dirInfo.subDir2 };
    RetCodeHrpService dirCheck = CreateSubDirs(path, subDirs);
    if (dirCheck != RET_HRP_SERVICE_SUCCESS) {
        return dirCheck;
    }

    std::string fullFileName = path + "/" + fileName;
    constexpr int filePermission = 0660;
    int retFd = open(fullFileName.c_str(), flags, filePermission);
    if (retFd < 0) {
        return errno == EACCES ? RET_HRP_SERVICE_ERR_STUB_OPEN_FILE_ACCESS_DENIED : RET_HRP_SERVICE_ERR_STUB_OPEN_FILE;
    }

    outFd = retFd;
    return RET_HRP_SERVICE_SUCCESS;
}

static RetCodeHrpService GetFileInfo(const char* name, HrpServiceFileInfo& outInfo)
{
    struct stat st{};
    outInfo = {};

    if (stat(name, &st) != 0) {
        return RET_HRP_SERVICE_ERR_STUB_FILE_READ_ERROR_NOT_EXISTS;
    }
    outInfo.name = name;
    outInfo.isDir = IsDir(st);
    if (!outInfo.isDir) {
        outInfo.size = st.st_size;
    }
    struct timespec accessTime = st.st_atim;
    struct timespec modifyTime = st.st_mtim;

    const auto accUsr = uint8_t(0xff & ((st.st_mode & S_IRWXU) >> 6));
    const auto accGrp = uint8_t(0xff & ((st.st_mode & S_IRWXG) >> 3));
    const auto accOwn = uint8_t(0xff & ((st.st_mode & S_IRWXO)));

    const auto usrShift = 16;
    const auto grpShift = 8;

    outInfo.accessBits = (accUsr << usrShift) | (accGrp << grpShift) | (accOwn);
    outInfo.accessTime = HrpServiceFileInfo::TimeValue { (uint64_t)accessTime.tv_sec, (uint64_t)accessTime.tv_nsec };
    outInfo.modifyTime = HrpServiceFileInfo::TimeValue { (uint64_t)modifyTime.tv_sec, (uint64_t)modifyTime.tv_nsec };
    return RET_HRP_SERVICE_SUCCESS;
}

static RetCodeHrpService GetFilesFromDir(const std::string& path,
    uint32_t firstFileIndex, std::vector<std::string>& files)
{
    struct dirent* entry;
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        return RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR;
    }
    size_t allocatedSize = 0;
    uint32_t fileIndex = 0;
    RetCodeHrpService retCode = RET_HRP_SERVICE_SUCCESS;
    while ((entry = readdir(dir)) != nullptr) {
        if (fileIndex >= firstFileIndex) {
            const std::string fileName = path + "/" + entry->d_name;
            const size_t appendSize = fileName.length() + 1;
            if (allocatedSize + appendSize > HRP_SERVICE_REQUEST_MAX_SIZE_BYTES) {
                retCode = RET_HRP_SERVICE_MORE_DATA_AVAILABLE;
                break;
            }
            files.push_back(fileName);
            allocatedSize += appendSize;
        }
        fileIndex++;
    }
    closedir(dir);
    return retCode;
}

RetCodeHrpService RSProfiler::HrpServicePopulateFiles(const HrpServiceDirInfo& dirInfo,
    uint32_t firstFileIndex, std::vector<HrpServiceFileInfo>& outFiles)
{
    if (!IsHrpServiceEnabled() && !RSSystemProperties::GetProfilerEnabled()) {
        return RET_HRP_SERVICE_ERR_UNSUPPORTED;
    }
    std::string path = HrpServiceGetDirFull(dirInfo.baseDirType);
    if (path.length() == 0 || !HrpServiceValidDirOrFileName(dirInfo.subDir)
            || !HrpServiceValidDirOrFileName(dirInfo.subDir2)) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }

    std::vector<const std::string*> subDirs = { &dirInfo.subDir, &dirInfo.subDir2 };
    for (const auto* d : subDirs) {
        if ((*d).length()) {
            path += "/" + (*d);
        }
    }
    std::vector<std::string> fileNames;
    RetCodeHrpService retCode = GetFilesFromDir(path, firstFileIndex, fileNames);
    if (retCode < RET_HRP_SERVICE_SUCCESS) {
        return retCode;
    }
    size_t allocatedSize = 0;
    for (const auto& fname : fileNames) {
        HrpServiceFileInfo fi;
        RetCodeHrpService retCode2 = GetFileInfo(fname.c_str(), fi);
        if (retCode2 < RET_HRP_SERVICE_SUCCESS) {
            retCode = retCode2;
            outFiles.clear();
            break;
        }
        const size_t appendSize = sizeof(HrpServiceBaseFileInfo) + fi.name.length() + 1;
        if (allocatedSize + appendSize > HRP_SERVICE_REQUEST_MAX_SIZE_BYTES) {
            retCode = RET_HRP_SERVICE_MORE_DATA_AVAILABLE;
            break;
        }
        allocatedSize += appendSize;
        outFiles.push_back(fi);
    }
    return retCode;
}
} // namespace OHOS::Rosen
