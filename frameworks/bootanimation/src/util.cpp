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

#include <event_handler.h>
#include <fstream>
#include <securec.h>
#include <sstream>

namespace OHOS {
void PostTask(std::function<void()> func, uint32_t delayTime)
{
    auto handler = AppExecFwk::EventHandler::Current();
    if (handler) {
        handler->PostTask(func, delayTime);
    }
}

bool IsFileExisted(const std::string& filePath)
{
    if (filePath.empty()) {
        LOGE("check filepath is empty");
        return false;
    }
    char newpath[PATH_MAX + 1] = { 0x00 };
    if (strlen(filePath.c_str()) > PATH_MAX || realpath(filePath.c_str(), newpath) == nullptr) {
        LOGE("check filepath fail! %{public}s %{public}d %{public}s", filePath.c_str(), errno, ::strerror(errno));
        return false;
    }
    struct stat info = {0};
    if (stat(newpath, &info) != 0) {
        LOGE("stat filepath fail! %{public}s %{public}d %{public}s", filePath.c_str(), errno, ::strerror(errno));
        return false;
    }
    return true;
}

bool ParseBootConfig(const std::string& path, int32_t& duration,
    bool& isCompatible, bool& isMultiDisplay, std::vector<BootAnimationConfig>& configs)
{
    char newpath[PATH_MAX + 1] = { 0x00 };
    if (strlen(path.c_str()) > PATH_MAX || realpath(path.c_str(), newpath) == nullptr) {
        LOGE("check config path fail! %{public}s %{public}d %{public}s", path.c_str(), errno, ::strerror(errno));
        return false;
    }

    std::ifstream configFile;
    configFile.open(newpath);
    std::stringstream JFilterParamsStream;
    JFilterParamsStream << configFile.rdbuf();
    configFile.close();
    std::string JParamsString = JFilterParamsStream.str();

    cJSON* overallData = cJSON_Parse(JParamsString.c_str());
    if (overallData == nullptr) {
        LOGE("can not parse config to json");
        return false;
    }
    cJSON_bool isNewConfig = cJSON_HasObjectItem(overallData, "screen_config");
    if (!isNewConfig) {
        isCompatible = true;
        ParseOldConfigFile(overallData, configs);
    } else {
        ParseNewConfigFile(overallData, isMultiDisplay, configs);
    }
    ParseBootDuration(overallData, duration);
    cJSON_Delete(overallData);
    return true;
}

void ParseOldConfigFile(cJSON* data, std::vector<BootAnimationConfig>& configs)
{
    LOGD("ParseOldConfigFile");
    BootAnimationConfig config;
    cJSON* custPicPath = cJSON_GetObjectItem(data, "cust.bootanimation.pics");
    if (custPicPath != nullptr && cJSON_IsString(custPicPath)) {
        config.picZipPath = custPicPath->valuestring;
        LOGI("cust piczip path: %{public}s", config.picZipPath.c_str());
    }
    cJSON* custSoundPath = cJSON_GetObjectItem(data, "cust.bootanimation.sounds");
    if (custSoundPath != nullptr && cJSON_IsString(custSoundPath)) {
        config.soundPath = custSoundPath->valuestring;
        LOGI("cust sound path: %{public}s", config.soundPath.c_str());
    }
    cJSON* custVideoDefaultPath = cJSON_GetObjectItem(data, "cust.bootanimation.video");
    if (custVideoDefaultPath != nullptr && cJSON_IsString(custVideoDefaultPath)) {
        config.videoDefaultPath = custVideoDefaultPath->valuestring;
        LOGI("cust video path: %{public}s", config.videoDefaultPath.c_str());
    }
    cJSON* custVideoExtraPath = cJSON_GetObjectItem(data, "cust.bootanimation.video.extra");
    if (custVideoExtraPath != nullptr && cJSON_IsString(custVideoExtraPath)) {
        config.videoExtraPath = custVideoExtraPath->valuestring;
        LOGI("cust extra video path: %{public}s", config.videoExtraPath.c_str());
    }
    cJSON* rotateScreenJson = cJSON_GetObjectItem(data, "cust.bootanimation.rotate.screenid");
    if (rotateScreenJson != nullptr && cJSON_IsString(rotateScreenJson)) {
        config.rotateScreenId = std::stoi(rotateScreenJson->valuestring);
        LOGI("cust rotateScreenId: %{public}d", config.rotateScreenId);
    }
    cJSON* rotateDegreeJson = cJSON_GetObjectItem(data, "cust.bootanimation.rotate.degree");
    if (rotateDegreeJson != nullptr && cJSON_IsString(rotateDegreeJson)) {
        config.rotateDegree = std::stoi(rotateDegreeJson->valuestring);
        LOGI("cust rotateDegree: %{public}d", config.rotateDegree);
    }
    configs.emplace_back(config);
}

void ParseNewConfigFile(cJSON* data, bool& isMultiDisplay, std::vector<BootAnimationConfig>& configs)
{
    LOGD("ParseNewConfigFile");
    cJSON* isSupport = cJSON_GetObjectItem(data, "cust.bootanimation.multi_display");
    if (isSupport != nullptr && cJSON_IsBool(isSupport)) {
        if (cJSON_IsTrue(isSupport)) {
            isMultiDisplay = true;
        }
    }
    LOGI("isMultiDisplay: %{public}d", isMultiDisplay);

    cJSON* screens = cJSON_GetObjectItem(data, "screen_config");
    if (screens != nullptr) {
        BootAnimationConfig config;
        cJSON* item = screens->child;
        while (item != nullptr) {
            cJSON* screenIdJson = cJSON_GetObjectItem(item, "cust.bootanimation.screen_id");
            if (screenIdJson != nullptr && cJSON_IsString(screenIdJson)) {
                config.screenId = std::stoul(screenIdJson->valuestring);
                LOGI("screenId: " BPUBU64 "", config.screenId);
            }
            cJSON* custPicPath = cJSON_GetObjectItem(item, "cust.bootanimation.pics");
            if (custPicPath != nullptr && cJSON_IsString(custPicPath)) {
                config.picZipPath = custPicPath->valuestring;
                LOGI("cust piczip path: %{public}s", config.picZipPath.c_str());
            }
            cJSON* custSoundPath = cJSON_GetObjectItem(item, "cust.bootanimation.sounds");
            if (custSoundPath != nullptr && cJSON_IsString(custSoundPath)) {
                config.soundPath = custSoundPath->valuestring;
                LOGI("cust sound path: %{public}s", config.soundPath.c_str());
            }
            cJSON* custVideoDefaultPath = cJSON_GetObjectItem(item, "cust.bootanimation.video_default");
            if (custVideoDefaultPath != nullptr && cJSON_IsString(custVideoDefaultPath)) {
                config.videoDefaultPath = custVideoDefaultPath->valuestring;
                LOGI("cust default video path: %{public}s", config.videoDefaultPath.c_str());
            }
            cJSON* rotateDegreeJson = cJSON_GetObjectItem(item, "cust.bootanimation.rotate_degree");
            if (rotateDegreeJson != nullptr && cJSON_IsString(rotateDegreeJson)) {
                config.rotateDegree = std::stoi(rotateDegreeJson->valuestring);
                LOGI("cust rotateDegree: %{public}d", config.rotateDegree);
            }
            cJSON* extraVideoPath = cJSON_GetObjectItem(item, "cust.bootanimation.video_extensions");
            if (extraVideoPath != nullptr && cJSON_IsArray(extraVideoPath)) {
                ParseVideoExtraPath(extraVideoPath, config);
            }
            configs.emplace_back(config);
            item = item->next;
        }
    }
}

void ParseVideoExtraPath(cJSON* data, BootAnimationConfig& config)
{
    int size = cJSON_GetArraySize(data);
    for (int index = 0; index < size; index++) {
        cJSON* extraPath = cJSON_GetArrayItem(data, index);
        if (extraPath != nullptr && cJSON_IsString(extraPath)) {
            config.videoExtPath.emplace_back(extraPath->valuestring);
        }
    }
}

void ParseBootDuration(cJSON* data, int32_t& duration)
{
    cJSON* durationJson = cJSON_GetObjectItem(data, "cust.bootanimation.duration");
    if (durationJson != nullptr && cJSON_IsString(durationJson)) {
        duration = std::stoi(durationJson->valuestring);
        LOGI("cust duration: %{public}d", duration);
    }
}

bool ReadZipFile(const std::string& srcFilePath, ImageStructVec& imgVec, FrameRateConfig& frameConfig)
{
    unzFile zipFile = unzOpen2(srcFilePath.c_str(), nullptr);
    if (zipFile == nullptr) {
        LOGE("Open zipFile fail: %{public}s", srcFilePath.c_str());
        return false;
    }

    unz_global_info globalInfo;
    if (unzGetGlobalInfo(zipFile, &globalInfo) != UNZ_OK) {
        unzClose(zipFile);
        LOGE("Get ZipGlobalInfo fail");
        return false;
    }

    LOGD("read zip file num: %{public}ld", globalInfo.number_entry);
    for (unsigned long i = 0; i < globalInfo.number_entry; ++i) {
        unz_file_info fileInfo;
        char filename[MAX_FILE_NAME] = {0};
        if (unzGetCurrentFileInfo(zipFile, &fileInfo, filename, MAX_FILE_NAME, nullptr, 0, nullptr, 0) != UNZ_OK) {
            unzClose(zipFile);
            return false;
        }
        if (filename[strlen(filename) - 1] != '/') {
            if (unzOpenCurrentFile(zipFile) != UNZ_OK) {
                unzClose(zipFile);
                return false;
            }
            std::string name = std::string(filename);
            size_t npos = name.find_last_of("//");
            if (npos != std::string::npos) {
                name = name.substr(npos + 1, name.length());
            }
            if (!ReadImageFile(zipFile, name, imgVec, frameConfig, fileInfo.uncompressed_size)) {
                LOGE("read zip deal single file failed");
                unzCloseCurrentFile(zipFile);
                unzClose(zipFile);
                return false;
            }
            unzCloseCurrentFile(zipFile);
        }
        if (i < (globalInfo.number_entry - 1)) {
            if (unzGoToNextFile(zipFile) != UNZ_OK) {
                unzClose(zipFile);
                return false;
            }
        }
    }
    return true;
}

void SortZipFile(ImageStructVec& imgVec)
{
    if (imgVec.size() == 0) {
        return;
    }

    sort(imgVec.begin(), imgVec.end(), [](std::shared_ptr<ImageStruct> image1,
        std::shared_ptr<ImageStruct> image2)
        -> bool {return image1->fileName < image2->fileName;});
}

bool ReadImageFile(const unzFile zipFile, const std::string& fileName,
    ImageStructVec& imgVec, FrameRateConfig& frameConfig, unsigned long fileSize)
{
    if (zipFile == nullptr) {
        LOGE("ReadImageFile failed, zip is null");
        return false;
    }
    int readLen = UNZ_OK;
    int totalLen = 0;
    int size = static_cast<int>(fileSize);
    char readBuffer[READ_SIZE] = {0};
    std::shared_ptr<ImageStruct> imageStruct = std::make_shared<ImageStruct>();
    imageStruct->memPtr.SetBufferSize(fileSize);
    do {
        readLen = unzReadCurrentFile(zipFile, readBuffer, READ_SIZE);
        if (readLen < 0) {
            LOGE("unzReadCurrentFile length error");
            return false;
        }
        if (imageStruct->memPtr.memBuffer == nullptr) {
            LOGE("ReadImageFile memPtr is null");
            return false;
        }
        if (memcpy_s(imageStruct->memPtr.memBuffer + totalLen, size - totalLen, \
            readBuffer, readLen) == EOK) {
            totalLen += readLen;
        }
    } while (readLen > 0);

    if (totalLen > 0) {
        LOGD("fileName: %{public}s, fileSize: %{public}d, totalLen: %{public}d", fileName.c_str(), size, totalLen);
        if (strstr(fileName.c_str(), BOOT_PIC_CONFIG_FILE.c_str()) != nullptr) {
            ParseImageConfig(imageStruct->memPtr.memBuffer, totalLen, frameConfig);
        } else {
            CheckImageData(fileName, imageStruct, totalLen, imgVec);
        }
    }
    return true;
}

bool ParseImageConfig(const char* fileBuffer, int totalsize, FrameRateConfig& frameConfig)
{
    std::string JParamsString;
    JParamsString.assign(fileBuffer, totalsize);
    cJSON* overallData = cJSON_Parse(JParamsString.c_str());
    if (overallData == nullptr) {
        LOGE("parse image config failed");
        return false;
    }
    cJSON* frameRate = cJSON_GetObjectItem(overallData, "FrameRate");
    if (frameRate != nullptr) {
        frameConfig.frameRate = frameRate->valueint;
        LOGI("freq: %{public}d", frameConfig.frameRate);
    }
    cJSON_Delete(overallData);
    return true;
}

bool CheckImageData(const std::string& fileName,
    std::shared_ptr<ImageStruct> imageStruct, int32_t bufferLen, ImageStructVec& imgVec)
{
    if (imageStruct->memPtr.memBuffer == nullptr) {
        LOGE("json file buffer is null");
        return false;
    }
    auto data = std::make_shared<Rosen::Drawing::Data>();
    data->BuildFromMalloc(imageStruct->memPtr.memBuffer, bufferLen);
    if (data->GetData() == nullptr) {
        LOGE("data memory data is null. update data failed");
        return false;
    }
    imageStruct->memPtr.setOwnerShip(data);
    imageStruct->fileName = fileName;
    imageStruct->imageData = std::make_shared<Rosen::Drawing::Image>();
    imageStruct->imageData->MakeFromEncoded(data);
    imgVec.push_back(imageStruct);
    return true;
}
} // namespace OHOS
