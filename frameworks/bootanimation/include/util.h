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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_UTIL_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_UTIL_H

#include "contrib/minizip/unzip.h"
#include "contrib/minizip/zip.h"
#include "zlib.h"
#include "log.h"

#include <cstdint>
#include <dirent.h>
#include <functional>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <iremote_object.h>
#include <platform/ohos/rs_irender_service.h>
#include <system_ability_definition.h>

namespace OHOS {
static const int READ_SIZE = 8192;
static const int MAX_FILE_NAME = 512;
static const int SLEEP_TIME_US = 30000;
static const std::string BOOT_PIC_CONFIGFILE = "config.json";
using MemStruct = struct MemStruct {
public:
    char* memBuffer = nullptr;
    unsigned long bufsize = 0;
    std::shared_ptr<Rosen::Drawing::Data> data_ = nullptr;
    ~MemStruct()
    {
        if (data_ != nullptr) {
            data_ = nullptr;
        } else {
            free(memBuffer);
            memBuffer = nullptr;
        }
    }
    void setOwnerShip(std::shared_ptr<Rosen::Drawing::Data>& data)
    {
        data_ = data;
    }
    void SetBufferSize(unsigned long ibufsize)
    {
        if (ibufsize == 0) {
            LOGE("MemStruct SetBuffer size is invalid!");
            return;
        }
        if (memBuffer == nullptr) {
            bufsize = ibufsize + 1;
            memBuffer = static_cast<char *>(malloc(bufsize + 1));
        }
    }
};
using ImageStruct = struct ImageStruct {
public:
    std::string fileName = {};
    std::shared_ptr<Rosen::Drawing::Image> imageData = nullptr;
    MemStruct memPtr;
    ~ImageStruct()
    {
        imageData = nullptr;
    }
};
using BootAniConfig = struct BootAniConfig {
public:
    int32_t frameRate = 30;
};
using BootCustomConfig = struct BootCustomConfig {
public:
    std::string custPicZipPath = {};
    std::string custSoundsPath = {};
    std::string custVideoPath = {};
    std::string custExtraVideoPath = {};
    int32_t rotateScreenId = -1;
    int32_t rotateDegree = 0;
};
using ImageStructVec = std::vector<std::shared_ptr<ImageStruct>>;
int64_t GetNowTime();
void PostTask(std::function<void()> func, uint32_t delayTime = 0);
bool ReadZipFile(const std::string& srcFilePath, ImageStructVec& outBgImgVec, BootAniConfig& aniconfig);
void WaitRenderServiceInit();
bool ReadCurrentFile(const unzFile zipfile, const std::string& filename, ImageStructVec& outBgImgVec,
    BootAniConfig& aniconfig, unsigned long fileSize);
bool GenImageData(const std::string& filename, std::shared_ptr<ImageStruct> imagetruct, int32_t bufferlen,
    ImageStructVec& outBgImgVec);
bool ReadJsonConfig(const char* filebuffer, int totalsize, BootAniConfig& aniconfig);
void SortZipFile(ImageStructVec& outBgImgVec);
bool IsFileExisted(const std::string& filePath);
bool ReadCustomBootConfig(const std::string& path, BootCustomConfig& aniconfig);
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_UTIL_H
