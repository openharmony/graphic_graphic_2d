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

#include "cJSON.h"
#include "util.h"

#include <vsync_helper.h>
#include <securec.h>
#include <sys/time.h>
#include <include/codec/SkCodec.h>

namespace OHOS {
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

bool ReadJsonConfig(const std::string& filebuffer, BootAniConfig& aniconfig)
{
    std::string JParamsString;
    JParamsString.assign(filebuffer.c_str(), filebuffer.length());
    cJSON* overallData = cJSON_Parse(JParamsString.c_str());
    if (overallData == nullptr) {
        LOGE("The config json file fails to compile.");
        return false;
    }
    cJSON* frameRate = cJSON_GetObjectItem(overallData, "FrameRate");
    if (frameRate != nullptr) {
        aniconfig.frameRate = frameRate->valueint;
        LOGI("freq: %{public}d", aniconfig.frameRate);
    }
    cJSON_Delete(overallData);
    return true;
}

bool GenImageData(const std::string& filename, std::shared_ptr<ImageStruct> imagetruct, int32_t bufferlen,
    ImageStructVec& outBgImgVec)
{
    if (imagetruct->memPtr.memBuffer == nullptr) {
        LOGE("Json File buffer is null.");
        return false;
    }
    auto skData = SkData::MakeFromMalloc(imagetruct->memPtr.memBuffer, bufferlen);
    if (skData == nullptr) {
        LOGE("skdata memory data is null. update data failed");
        return false;
    }
    imagetruct->memPtr.setOwnerShip(skData);
    auto codec = SkCodec::MakeFromData(skData);
    imagetruct->fileName = filename;
    imagetruct->imageData = SkImage::MakeFromEncoded(skData);
    outBgImgVec.push_back(imagetruct);
    return true;
}

bool ReadCurrentFile(const unzFile zipfile, const std::string& filename, ImageStructVec& outBgImgVec,
    BootAniConfig& aniconfig)
{
    if (zipfile == nullptr) {
        LOGE("Readzip Json zipfile is null.");
        return false;
    }
    int readlen = UNZ_OK;
    int totalLen = 0;
    char readBuffer[READ_SIZE] = {0};
    std::shared_ptr<ImageStruct> imagestrct = std::make_shared<ImageStruct>();
    do {
        readlen = unzReadCurrentFile(zipfile, readBuffer, READ_SIZE);
        if (readlen < 0) {
            LOGE("Readzip readCurrFile failed");
            return false;
        }
        if (totalLen + readlen > imagestrct->memPtr.bufsize) {
            if (!imagestrct->memPtr.reallocBuffer()) {
                LOGE("Readzip reallocBuffer failed");
                return false;
            }
        }
        if (memcpy_s(imagestrct->memPtr.memBuffer + totalLen, imagestrct->memPtr.bufsize - readlen, \
            readBuffer, readlen) == EOK) {
            totalLen += readlen;
        }
    } while (readlen > 0);

    if (totalLen > 0) {
        if (strstr(filename.c_str(), BOOT_PIC_CONFIGFILE.c_str()) != nullptr) {
            ReadJsonConfig(std::string(imagestrct->memPtr.memBuffer), aniconfig);
        } else {
            GenImageData(filename, imagestrct, totalLen, outBgImgVec);
        }
    }
    return true;
}

void SortZipFile(ImageStructVec& outBgImgVec)
{
    if (outBgImgVec.size() == 0) {
        return;
    }

    sort(outBgImgVec.begin(), outBgImgVec.end(), [](std::shared_ptr<ImageStruct> image1,
        std::shared_ptr<ImageStruct> image2)
        -> bool {return image1->fileName < image2->fileName;});
}

bool ReadZipFile(const std::string& srcFilePath, ImageStructVec& outBgImgVec, BootAniConfig& aniconfig)
{
    unzFile zipfile = unzOpen2(srcFilePath.c_str(), nullptr);
    if (zipfile == nullptr) {
        return false;
    }
    unz_global_info globalInfo;
    if (unzGetGlobalInfo(zipfile, &globalInfo) != UNZ_OK) {
        unzClose(zipfile);
        return false;
    }
    LOGD("Readzip zip file num: %{public}ld", globalInfo.number_entry);
    for (unsigned long i = 0; i < globalInfo.number_entry; ++i) {
        unz_file_info fileInfo;
        char filename[MAX_FILE_NAME];
        if (unzGetCurrentFileInfo(zipfile, &fileInfo, filename, MAX_FILE_NAME, nullptr, 0, nullptr, 0) != UNZ_OK) {
            unzClose(zipfile);
            return false;
        }
        if (filename[strlen(filename) - 1] != '/') {
            if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
                unzClose(zipfile);
                return false;
            }
            std::string strfilename = std::string(filename);
            int npos = strfilename.find_last_of("//");
            if (npos != -1) {
                strfilename = strfilename.substr(npos + 1, strfilename.length());
            }
            if (!ReadCurrentFile(zipfile, strfilename, outBgImgVec, aniconfig)) {
                LOGE("Readzip deal single file failed");
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
            unzCloseCurrentFile(zipfile);
        }
        if (i < (globalInfo.number_entry - 1)) {
            if (unzGoToNextFile(zipfile) != UNZ_OK) {
                unzClose(zipfile);
                return false;
            }
        }
    }
    return true;
}

void WaitRenderServiceInit()
{
    while (true) {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
        if (remoteObject != nullptr) {
            LOGI("renderService is inited");
            break;
        } else {
            LOGI("renderService is not inited, wait");
            sleep(1);
        }
    }
}
} // namespace OHOS
