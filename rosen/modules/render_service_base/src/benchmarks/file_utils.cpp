/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "benchmarks/file_utils.h"
#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "directory_ex.h"

namespace OHOS {
namespace Rosen {
namespace Benchmarks {
bool IsValidFile(const std::string& realPathStr, const std::string& validFile)
{
    return realPathStr.find(validFile) == 0;
}

std::string GetRealAndValidPath(const std::string& filePath)
{
    std::string realPathStr;
    if (!PathToRealPath(filePath, realPathStr)) {
        ROSEN_LOGE("FilePath is nullptr");
        return "";
    }
    if (IsValidFile(realPathStr)) {
        return realPathStr;
    } else {
        RS_LOGE("FileUtils: The file path is not valid!");
        return "";
    }
}

bool CreateFile(const std::string& filePath)
{
    std::string realPath = GetRealAndValidPath(filePath);
    if (realPath.empty()) {
        return false;
    }
    std::ofstream outFile(realPath);
    if (!outFile.is_open()) {
        RS_LOGE("FileUtils: file %{public}s open failed!", realPath.c_str());
        return false;
    }
    outFile.clear();
    outFile.close();
    return true;
}

bool WriteToFile(uintptr_t data, size_t size, const std::string& filePath)
{
    if (!CreateFile(filePath)) {
        return false;
    }
    if (filePath.empty()) {
        return false;
    }
    int fd = open(filePath.c_str(), O_RDWR | O_CREAT, static_cast<mode_t>(0600));
    if (fd < 0) {
        RS_LOGE("FileUtils: %{public}s failed. file: %{public}s, fd = %{public}d", __func__, filePath.c_str(), fd);
        return false;
    }
    ssize_t nwrite = write(fd, reinterpret_cast<uint8_t *>(data), size);
    if (nwrite < 0) {
        RS_LOGE("FileUtils: %{public}s failed to persist data, size = %{public}zu,  fd = %{public}d",
            __func__, size, fd);
    }
    close(fd);
    return true;
}

bool WriteStringToFile(int fd, const std::string& str)
{
    const char* p = str.data();
    ssize_t remaining = static_cast<ssize_t>(str.size());
    while (remaining > 0) {
        ssize_t n = write(fd, p, remaining);
        if (n == -1) {
            return false;
        }
        p += n;
        remaining -= n;
    }
    p = nullptr;
    return true;
}

bool WriteStringToFile(const std::string& str, const std::string& filePath)
{
    if (!CreateFile(filePath)) {
        return false;
    }
    if (filePath.empty()) {
        return false;
    }
    int fd = open(filePath.c_str(), O_RDWR | O_CREAT, static_cast<mode_t>(0600));
    if (fd < 0) {
        RS_LOGE("FileUtils: %{public}s failed. file: %{public}s, fd = %{public}d", __func__, filePath.c_str(), fd);
        return false;
    }
    bool result = WriteStringToFile(fd, str);
    close(fd);
    return result;
}

bool WriteMessageParcelToFile(std::shared_ptr<MessageParcel> messageParcel, const std::string& opsDescription,
    int frameNum, const std::string& fileDir)
{
    // file name
    std::string drawCmdListFile = fileDir + "/frame" + std::to_string(frameNum) + ".drawing";
    std::string opsFile = fileDir + "/ops_frame" + std::to_string(frameNum) + ".txt";
    // get data
    size_t sz = messageParcel->GetDataSize();
    auto buf = reinterpret_cast<uintptr_t>(messageParcel->GetData());
    std::string line = "RSRecordingThread::FinishRecordingOneFrame curDumpFrame = " +
        std::to_string(frameNum) + ", size = " + std::to_string(sz);
    RS_LOGD("%{public}s", line.c_str());
    RS_TRACE_NAME(line);

    return OHOS::Rosen::Benchmarks::WriteToFile(buf, sz, drawCmdListFile) &&
        OHOS::Rosen::Benchmarks::WriteStringToFile(opsDescription, opsFile);
}
}
} // namespace Rosen
} // namespace OHOS