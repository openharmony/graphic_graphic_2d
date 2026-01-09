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
#include "gfx/dump/rs_dump_manager.h"

#include <vector>
#include <string>
#include <future>

#include "platform/common/rs_log.h"
#include "securec.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
// Define a static constant to represent the fixed length of HELPINFO_CMD
static const size_t HELPINFO_CMD_FIXED_LENGTH = 30;
constexpr int32_t SHARE_MEM_ALLOC = 2;
constexpr int32_t MAX_BUFFER_SIZE = 32 * 1024 * 1024;

RSDumpManager::RSDumpManager()
{
    // Register the help information function to the function mapping table
    RSDumpFunc helpInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                                std::string &dumpString) -> void {
        DumpHelpInfo(dumpString);
    };
    Register({ RSDumpID::HELP_INFO, helpInfoFunc });
}

// Get the instance of RSDumpManager
RSDumpManager& RSDumpManager::GetInstance()
{
    // Define a static RSDumpManager object to ensure only one instance is created
    static RSDumpManager instance;
    return instance;
}

// Register a module to RSDumpManager
void RSDumpManager::Register(RSDumpHander rsDumpHander)
{
    RSDumpID rsDumpId = rsDumpHander.rsDumpId;
    if (rsDumpHanderMap_.find(rsDumpId) != rsDumpHanderMap_.end()) {
        // If the module ID already exists, output a warning message
        RS_LOGW("RSDumpManager::Register, rsDumpId %{public}d is already exist.", static_cast<int>(rsDumpId));
    } else {
        rsDumpHanderMap_[rsDumpId] = rsDumpHander;
    }
}

// Register multiple Dump points to RSDumpManager
void RSDumpManager::Register(const std::vector<RSDumpHander> &rsDumpHanders)
{
    if (rsDumpHanders.empty()) {
        return;
    }
    for (const auto &rsDumpHander : rsDumpHanders) {
        Register(rsDumpHander);
    }
}

// Unregister a Dump point
void RSDumpManager::UnRegister(RSDumpID rsDumpId)
{
    auto it = rsDumpHanderMap_.find(rsDumpId);
    if (it != rsDumpHanderMap_.end()) {
        rsDumpHanderMap_.erase(it);
    } else {
        // If the Dump point does not exist, output a warning message
        RS_LOGW("RSDumpManager::UnRegister, rsDumpId %{public}d is not exist.", static_cast<int>(rsDumpId));
    }
}

// Execute a command
void RSDumpManager::CmdExec(std::unordered_set<std::u16string>& argSets, std::string &out)
{
    if (!argSets.empty()) {
        MatchAndExecuteCommand(argSets, out);
    } else {
        // If the parameters are empty, output an error message
        RS_LOGW("RSDumpManager::CmdExec, args is empty.");
        DumpHelpInfo(out);
    }
}

// Match and execute a command
void RSDumpManager::MatchAndExecuteCommand(std::unordered_set<std::u16string> &argSets, std::string &out)
{
    for (const std::u16string &cmd : argSets) {
        auto it = cmdMap_.find(cmd);
        if (it != cmdMap_.end()) {
            std::string cmd_str = std::string(cmd.begin(), cmd.end());
            RS_LOGI("RSDumpManager::cmdExec, cmd is '%{public}s'.", cmd_str.c_str());

            RSDumpCmd entry = it->second;
            for (RSDumpID rsDumpId : entry.rsDumpIds) {
                DoDump(rsDumpId, cmd, argSets, out);
            }
            break;
        }
    }
    if (out.empty()) {
        // If the command is not found, output help information
        DumpHelpInfo(out);
    }
}

// Execute a specific dump operation
void RSDumpManager::DoDump(RSDumpID rsDumpId, const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                           std::string &out)
{
    RS_TRACE_NAME_FMT("RSDumpManager::DoDump rsDumpId [%d]", rsDumpId);
    RS_LOGI("RSDumpManager::DoDump, rsDumpId is '%{public}u'.", static_cast<uint8_t>(rsDumpId));
    auto it = rsDumpHanderMap_.find(rsDumpId);
    if (it != rsDumpHanderMap_.end()) {
        RSDumpHander hander = it->second;
        if (!hander.tag.empty()) {
            out.append("\n\n-- ").append(hander.tag).append("\n");
        }
        hander.func(cmd, argSets, out);
    } else {
        // If the hander is not registered, output an error message
        RS_LOGE("RSDumpManager::DoDump, rsDumpId %{public}d not registered.", static_cast<int>(rsDumpId));
    }
}

// Generate help information
void RSDumpManager::DumpHelpInfo(std::string &out)
{
    out.append("------ Graphic2D--RenderSerice ------\n")
        .append("Usage:\n");
    for (const auto &entry : cmdMap_) {
        std::u16string cmd = entry.first;
        if (excludeCmds_.find(cmd) != excludeCmds_.end()) {
            continue;
        }
        std::string cmd_str = std::string(cmd.begin(), cmd.end());
        out.append(cmd_str);

        if (HELPINFO_CMD_FIXED_LENGTH - cmd_str.length() > 0) {
            // Append the necessary number of spaces to make up the difference
            out.append(HELPINFO_CMD_FIXED_LENGTH - cmd_str.length(), ' ');
        }

        out.append("|")
            .append(entry.second.helpInfo)
            .append("\n");
    }
}

bool RSDumpManager::WriteFileDescriptor(Parcel &parcel, int fd)
{
    if (fd < 0) {
        RS_LOGI("RSDumpManager::WriteFileDescriptor get fd failed, fd:[%{public}d].", fd);
        return false;
    }
    int dupFd = dup(fd);
    if (dupFd < 0) {
        RS_LOGI("RSDumpManager::WriteFileDescriptor dup fd failed, dupFd:[%{public}d].", dupFd);
        return false;
    }
    sptr<IPCFileDescriptor> descriptor = new IPCFileDescriptor(dupFd);
    bool result = parcel.WriteObject<IPCFileDescriptor>(descriptor);
    if (!result) {
        close(dupFd);
    }
    return result;
}
 
int RSDumpManager::ReadFileDescriptor(Parcel &parcel)
{
    sptr<IPCFileDescriptor> descriptor = parcel.ReadObject<IPCFileDescriptor>();
    if (descriptor == nullptr) {
        RS_LOGI("RSDumpManager::ReadFileDescriptor get descriptor failed");
        return -1;
    }
    int fd = descriptor->GetFd();
    if (fd < 0) {
        RS_LOGI("RSDumpManager::ReadFileDescriptor get fd failed, fd:[%{public}d].", fd);
        return -1;
    }
    return dup(fd);
}
 
bool RSDumpManager::WriteAshmemDataToParcel(Parcel &parcel, size_t size, const char* dataPtr)
{
    std::string name = "Parcel DumpString";
    int fd = AshmemCreate(name.c_str(), size);
    RS_LOGI("AshmemCreate:[%{public}d].", fd);
    if (fd < 0) {
        RS_LOGE("RSDumpManager::WriteAshmemDataToParcel failed fd=%{public}d invalid", fd);
        return false;
    }
 
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    ROSEN_LOGI("AshmemSetProt:[%{public}d].", result);
    if (result < 0) {
        ::close(fd);
        return false;
    }
    void *ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        ::close(fd);
        RS_LOGE("WriteAshmemData map failed, errno:%{public}d", errno);
        return false;
    }
    ROSEN_LOGI("mmap success");
 
    if (memcpy_s(ptr, size, dataPtr, size) != EOK) {
        ::munmap(ptr, size);
        ::close(fd);
        RS_LOGE("WriteAshmemData memcpy_s error");
        return false;
    }
 
    RS_LOGI("memcpy mmap success");
    if (!WriteFileDescriptor(parcel, fd)) {
        ::munmap(ptr, size);
        ::close(fd);
        RS_LOGE("WriteAshmemData WriteFileDescriptor error");
        return false;
    }
    
    ::munmap(ptr, size);
    ::close(fd);
    return true;
}
 
char *RSDumpManager::ReadAshmemDataFromParcel(Parcel &parcel, int32_t size)
{
    char *base = nullptr;
    int fd = ReadFileDescriptor(parcel);
    if (!CheckAshmemSize(fd, size)) {
        RS_LOGE("RSDumpManager::ReadAshmemDataFromParcel check ashmem size failed, fd:[%{public}d].", fd);
        close(fd);
        return nullptr;
    }
    if (size <= 0 || size > MAX_BUFFER_SIZE) {
        RS_LOGE("RSDumpManager::ReadAshmemDataFromParcel"
                " malloc parameter bufferSize:[%{public}d] error.", size);
        close(fd);
        return nullptr;
    }
 
    void *ptr = ::mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        RS_LOGE("RSDumpManager::ReadAshmemDataFromParcel mmap error");
        ::close(fd);
        return base;
    }
 
    base = static_cast<char *>(malloc(size));
    if (base == nullptr) {
        RS_LOGE("RSDumpManager::ReadAshmemDataFromParcel malloc error");
        ReleaseMemory(SHARE_MEM_ALLOC, ptr, &fd, size);
        return base;
    }
    if (memcpy_s(base, size, ptr, size) != 0) {
        RS_LOGE("RSDumpManager::ReadAshmemDataFromParcel memcpy_s error");
        ReleaseMemory(SHARE_MEM_ALLOC, ptr, &fd, size);
        free(base);
        base = nullptr;
        return base;
    }
 
    RS_LOGI("RSDumpManager::ReadAshmemDataFromParcel success");
    ReleaseMemory(SHARE_MEM_ALLOC, ptr, &fd, size);
    return base;
}
 
void RSDumpManager::ReleaseMemory(int32_t allocType, void *addr, void *context, uint32_t size)
{
    if (allocType == SHARE_MEM_ALLOC) {
        if (context != nullptr) {
            int *fd = static_cast<int *>(context);
            if (addr != nullptr) {
                ::munmap(addr, size);
            }
            if (fd != nullptr) {
                ::close(*fd);
            }
            context = nullptr;
            addr = nullptr;
        }
    }
}
}  // namespace OHOS::Rosen