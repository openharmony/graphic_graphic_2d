/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "util.h"

#include <ctime>
#include <sys/time.h>
#include <string>

#include <vsync_helper.h>

namespace OHOS {
static const int MAX_FILE_NAME = 512;
static const int READ_SIZE = 8192;
int64_t GetNowTime()
{
    struct timeval start = {};
    gettimeofday(&start, nullptr);
    constexpr uint32_t secToUsec = 1000 * 1000;
    return static_cast<int64_t>(start.tv_sec) * secToUsec + start.tv_usec;
}

void PostTask(std::function<void()> func, uint32_t delayTime)
{
    auto handler = AppExecFwk::EventHandler::Current();
    if (handler) {
        handler->PostTask(func, delayTime);
    }
}

bool UnzipFile(const std::string& srcFilePath, const std::string& dstFilePath)
{
    zlib_filefunc_def *zipFuncPtrs = nullptr;
    unzFile zipfile = unzOpen2(srcFilePath.c_str(), zipFuncPtrs);
    if (zipfile == nullptr) {
        LOG("zip file not found");
        return false;
    }

    unz_global_info globalInfo;
    if (unzGetGlobalInfo(zipfile, &globalInfo) != UNZ_OK) {
        LOG("could not read file global info");
        unzClose(zipfile);
        return false;
    }

    char readBuffer[READ_SIZE];
    RemoveDir(dstFilePath.c_str());
    int ret = mkdir(dstFilePath.c_str(), 0700);
    LOG("create dir bootpic, ret: %{public}d", ret);
    if (ret == -1) {
        LOG("pic dir is already exist");
        return true;
    }

    for (unsigned long i = 0; i < globalInfo.number_entry; ++i) {
        unz_file_info fileInfo;
        char filename[MAX_FILE_NAME];
        if (unzGetCurrentFileInfo(
            zipfile,
            &fileInfo,
            filename,
            MAX_FILE_NAME,
            nullptr, 0, nullptr, 0) != UNZ_OK) {
            LOG("could not read file info");
            unzClose(zipfile);
            return false;
        }

        const size_t fileNameLength = strlen(filename);

        std::string fileStr(dstFilePath + "/" + filename);
        if (filename[fileNameLength - 1] == '/') {
            LOG("mkdir: %{public}s", filename);
            mkdir(fileStr.c_str(), 0700);
        } else {
            if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
                LOG("could not open file");
                unzClose(zipfile);
                return false;
            }
            FILE *out = fopen(fileStr.c_str(), "wb");
            if (out == nullptr) {
                LOG("could not open destination file");
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(zipfile, readBuffer, READ_SIZE);
                if (error < 0) {
                    LOG("unzReadCurrentFile error %{public}d", error);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    fclose(out);
                    return false;
                }
                if (error > 0) {
                    fwrite(readBuffer, error, 1, out);
                }
            } while (error > 0);
            fclose(out);
        }
        unzCloseCurrentFile(zipfile);

        if (i < (globalInfo.number_entry - 1)) {
            if (unzGoToNextFile(zipfile) != UNZ_OK) {
                LOG("could not read next file");
                unzClose(zipfile);
                return false;
            }
        }
    }
    return true;
}

int RemoveDir(const char *dir)
{
    char curDir[] = ".";
    char upDir[] = "..";
    DIR *dirp;
    struct dirent *dp;
    struct stat dirStat;

    if (access(dir, F_OK) != 0) {
        LOG("can not access dir");
        return 0;
    }
    int statRet = stat(dir, &dirStat);
    if (statRet < 0) {
        LOG("dir statRet: %{public}d", statRet);
        return -1;
    }

    if (S_ISREG(dirStat.st_mode)) {
        remove(dir);
    } else if (S_ISDIR(dirStat.st_mode)) {
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != nullptr) {
            if ((strcmp(curDir, dp->d_name) == 0) || (strcmp(upDir, dp->d_name) == 0)) {
                continue;
            }

            std::string dirName = dir;
            dirName += "/";
            dirName += dp->d_name;
            RemoveDir(dirName.c_str());
        }
        closedir(dirp);
        LOG("remove empty dir finally");
        rmdir(dir);
    } else {
        LOG("unknown file type");
    }
    return 0;
}

int CountPicNum(const char *dir, int32_t& picNum)
{
    char curDir[] = ".";
    char upDir[] = "..";
    DIR *dirp;
    struct dirent *dp;
    struct stat dirStat;

    if (access(dir, F_OK) != 0) {
        LOG("can not access dir");
        return picNum;
    }
    int statRet = stat(dir, &dirStat);
    if (statRet < 0) {
        LOG("dir statRet: %{public}d", statRet);
        return picNum;
    }
    if (S_ISREG(dirStat.st_mode)) {
        picNum += 1;
    } else  if (S_ISDIR(dirStat.st_mode)) {
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != nullptr) {
            if ((strcmp(curDir, dp->d_name) == 0) || (strcmp(upDir, dp->d_name) == 0)) {
                continue;
            }

            std::string dirName = dir;
            dirName += "/";
            dirName += dp->d_name;
            CountPicNum(dirName.c_str(), picNum);
        }
        closedir(dirp);
        LOG("remove empty dir finally");
        return picNum;
    } else {
        LOG("unknown file type");
    }
    return picNum;
}

void WaitRenderServiceInit()
{
    while (true) {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
        if (remoteObject != nullptr) {
            LOG("renderService is inited");
            break;
        } else {
            LOG("renderService is not inited, wait");
            sleep(1);
        }
    }
}
} // namespace OHOS
