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
constexpr const float HALF = 2.0;
constexpr const float RADIO = 360.0;

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

bool ParseBootConfig(const std::string& path, int32_t& duration, bool& isCompatible, bool& isMultiDisplay,
    std::vector<BootAnimationConfig>& configs)
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
    }
    cJSON* custSoundPath = cJSON_GetObjectItem(data, "cust.bootanimation.sounds");
    if (custSoundPath != nullptr && cJSON_IsString(custSoundPath)) {
        config.soundPath = custSoundPath->valuestring;
    }
    cJSON* custVideoDefaultPath = cJSON_GetObjectItem(data, "cust.bootanimation.video");
    if (custVideoDefaultPath != nullptr && cJSON_IsString(custVideoDefaultPath)) {
        config.videoDefaultPath = custVideoDefaultPath->valuestring;
    }
    cJSON* custVideoExtraPath = cJSON_GetObjectItem(data, "cust.bootanimation.video.extra");
    if (custVideoExtraPath != nullptr && cJSON_IsString(custVideoExtraPath)) {
        config.videoExtraPath = custVideoExtraPath->valuestring;
    }
    cJSON* rotateScreenJson = cJSON_GetObjectItem(data, "cust.bootanimation.rotate.screenid");
    if (rotateScreenJson != nullptr && cJSON_IsString(rotateScreenJson)) {
        config.rotateScreenId = std::atoi(rotateScreenJson->valuestring);
        LOGI("cust rotateScreenId: %{public}d", config.rotateScreenId);
    }
    cJSON* rotateDegreeJson = cJSON_GetObjectItem(data, "cust.bootanimation.rotate.degree");
    if (rotateDegreeJson != nullptr && cJSON_IsString(rotateDegreeJson)) {
        config.rotateDegree = std::atoi(rotateDegreeJson->valuestring);
        LOGI("cust rotateDegree: %{public}d", config.rotateDegree);
    }
    cJSON* extraVideoPath = cJSON_GetObjectItem(data, "cust.bootanimation.video_extensions");
    if (extraVideoPath != nullptr && cJSON_IsObject(extraVideoPath)) {
        ParseVideoExtraPath(extraVideoPath, config);
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
                config.screenId = std::strtoul(screenIdJson->valuestring, nullptr, 0);
                LOGI("screenId: " BPUBU64 "", config.screenId);
            }
            cJSON* custPicPath = cJSON_GetObjectItem(item, "cust.bootanimation.pics");
            if (custPicPath != nullptr && cJSON_IsString(custPicPath)) {
                config.picZipPath = custPicPath->valuestring;
            }
            cJSON* custSoundPath = cJSON_GetObjectItem(item, "cust.bootanimation.sounds");
            if (custSoundPath != nullptr && cJSON_IsString(custSoundPath)) {
                config.soundPath = custSoundPath->valuestring;
            }
            cJSON* custVideoDefaultPath = cJSON_GetObjectItem(item, "cust.bootanimation.video_default");
            if (custVideoDefaultPath != nullptr && cJSON_IsString(custVideoDefaultPath)) {
                config.videoDefaultPath = custVideoDefaultPath->valuestring;
            }
            cJSON* rotateDegreeJson = cJSON_GetObjectItem(item, "cust.bootanimation.rotate_degree");
            if (rotateDegreeJson != nullptr && cJSON_IsString(rotateDegreeJson)) {
                config.rotateDegree = std::atoi(rotateDegreeJson->valuestring);
                LOGI("cust rotateDegree: %{public}d", config.rotateDegree);
            }
            cJSON* extraVideoPath = cJSON_GetObjectItem(item, "cust.bootanimation.video_extensions");
            if (extraVideoPath != nullptr && cJSON_IsObject(extraVideoPath)) {
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
        if (extraPath != nullptr && extraPath->string != nullptr && extraPath->valuestring != nullptr
            && strlen(extraPath->string) != 0) {
            config.videoExtPath.emplace(extraPath->string, extraPath->valuestring);
        }
    }
}

void ParseBootDuration(cJSON* data, int32_t& duration)
{
    cJSON* durationJson = cJSON_GetObjectItem(data, "cust.bootanimation.duration");
    if (durationJson != nullptr && cJSON_IsString(durationJson)) {
        duration = std::atoi(durationJson->valuestring);
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
        LOGE("Get ZipGlobalInfo fail");
        return CloseZipFile(zipFile, false);
    }

    LOGD("read zip file num: %{public}ld", globalInfo.number_entry);
    for (unsigned long i = 0; i < globalInfo.number_entry; ++i) {
        unz_file_info fileInfo;
        char filename[MAX_FILE_NAME] = {0};
        if (unzGetCurrentFileInfo(zipFile, &fileInfo, filename, MAX_FILE_NAME, nullptr, 0, nullptr, 0) != UNZ_OK) {
            return CloseZipFile(zipFile, false);
        }
        size_t length = strlen(filename);
        if (length > MAX_FILE_NAME || length == 0) {
            return CloseZipFile(zipFile, false);
        }
        if (filename[length - 1] != '/') {
            if (unzOpenCurrentFile(zipFile) != UNZ_OK) {
                return CloseZipFile(zipFile, false);
            }
            std::string name = std::string(filename);
            size_t npos = name.find_last_of("//");
            if (npos != std::string::npos) {
                name = name.substr(npos + 1, name.length());
            }
            if (!ReadImageFile(zipFile, name, imgVec, frameConfig, fileInfo.uncompressed_size)) {
                LOGE("read zip deal single file failed");
                unzCloseCurrentFile(zipFile);
                return CloseZipFile(zipFile, false);
            }
            unzCloseCurrentFile(zipFile);
        }
        if (i < (globalInfo.number_entry - 1)) {
            if (unzGoToNextFile(zipFile) != UNZ_OK) {
                return CloseZipFile(zipFile, false);
            }
        }
    }
    return CloseZipFile(zipFile, true);
}

bool CloseZipFile(const unzFile zipFile, bool ret)
{
    unzClose(zipFile);
    return ret;
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

bool ReadImageFile(const unzFile zipFile, const std::string& fileName, ImageStructVec& imgVec,
    FrameRateConfig& frameConfig, unsigned long fileSize)
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
    if (frameRate != nullptr && cJSON_IsNumber(frameRate)) {
        frameConfig.frameRate = frameRate->valueint;
        LOGI("freq: %{public}d", frameConfig.frameRate);
    }
    cJSON_Delete(overallData);
    return true;
}

bool CheckImageData(const std::string& fileName, std::shared_ptr<ImageStruct> imageStruct,
    int32_t bufferLen, ImageStructVec& imgVec)
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
#ifdef RS_ENABLE_GPU
    imageStruct->imageData->MakeFromEncoded(data);
#endif
    imgVec.push_back(imageStruct);
    return true;
}

/**
 * Transate vp to pixel.
 *
 * @param sideLen The short side length of screen.
 * @param vp vp value.
 * @return Returns the font size.
 */
int32_t TransalteVp2Pixel(const int32_t sideLen, const int32_t vp)
{
    return static_cast<int32_t>(std::ceil(sideLen * HALF / RADIO) / HALF * vp);
}

std::string ReadFile(const std::string &filePath)
{
    std::string content;
    if (filePath.empty() || filePath.length() > PATH_MAX) {
        LOGE("filepath check failed.");
        return content;
    }
    char tmpPath[PATH_MAX] = {0};
    if (realpath(filePath.c_str(), tmpPath) == nullptr) {
        LOGE("filepath check failed! %{public}s %{public}d %{public}s", filePath.c_str(), errno, ::strerror(errno));
        return content;
    }
    std::ifstream infile;
    infile.open(tmpPath);
    if (!infile.is_open()) {
        LOGE("failed to open file");
        return content;
    }

    getline(infile, content);
    infile.close();
    return content;
}

std::string GetHingeStatus()
{
    if (!IsFileExisted(HING_STATUS_INFO_PATH)) {
        LOGE("failed not exist");
        return "";
    }

    return ReadFile(HING_STATUS_INFO_PATH);
}

int64_t GetSystemCurrentTime()
{
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
    return currentTime;
}
} // namespace OHOS
