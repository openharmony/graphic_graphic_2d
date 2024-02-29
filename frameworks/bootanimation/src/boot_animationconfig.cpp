/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "boot_animationconfig.h"
#include "log.h"
#include "config_policy_utils.h"

using namespace OHOS;
static const std::string BOOT_CUSTOM_PATHSUFFIX = "etc/bootanimation/bootanimation_custom_config.json";
static const std::string BOOT_PIC_ZIP = "/system/etc/graphic/bootpic.zip";
static const std::string BOOT_VIDEO_PATH = "/system/etc/graphic/bootvideo.mp4";
static const std::string BOOT_SOUND_URI = "file://system/etc/graphic/bootsound.wav";

bool BootAnimationConfig::ReadPicZipFile(ImageStructVec& vec, int32_t& freq)
{
    BootAniConfig jsonConfig;
    std::string picZipPath = GetPicZipPath();
    ReadZipFile(picZipPath, vec, jsonConfig);
    int32_t imageNum = static_cast<int32_t>(vec.size());
    if (imageNum < 0) {
        LOGE("zip pic num is invalid.");
        return false;
    }
    SortZipFile(vec);
    if (CheckFrameRateValid(jsonConfig.frameRate)) {
        freq = jsonConfig.frameRate;
    } else {
        LOGI("Only Support 30, 60 frame rate: %{public}d", jsonConfig.frameRate);
    }
    LOGI("end to Readzip pics freq: %{public}d totalPicNum: %{public}d", freq, imageNum);
    return true;
}

bool BootAnimationConfig::CheckFrameRateValid(int32_t ratevalue)
{
    std::vector<int> freqs = {60, 30};
    int nCount = std::count(freqs.begin(), freqs.end(), ratevalue);
    if (nCount <= 0) {
        return false;
    }
    return true;
}

void BootAnimationConfig::CheckBootVideoEnabled()
{
    if (custConfig_.custVideoPath.empty() && !custConfig_.custPicZipPath.empty()) {
        LOGI("Custom picture path is config and custom video path is not config");
        bootVideoEnabled_ = false;
        return;
    }

    if (!custConfig_.custVideoPath.empty() && !IsFileExisted(custConfig_.custVideoPath)) {
        LOGE("Custom video file is not found");
    }
}

std::string BootAnimationConfig::GetPicZipPath()
{
    std::string retstr = BOOT_PIC_ZIP;
    if (custConfig_.custPicZipPath.empty()) {
        LOGI("custpic path not config");
        return retstr;
    }

    if (IsFileExisted(custConfig_.custPicZipPath)) {
        LOGI("custpic path %{public}s", custConfig_.custPicZipPath.c_str());
        retstr = custConfig_.custPicZipPath;
    }
    return retstr;
}

std::string BootAnimationConfig::GetSoundUrl()
{
    std::string retstr = BOOT_SOUND_URI;
    if (custConfig_.custSoundsPath.empty()) {
        LOGI("custsound path not config");
        return retstr;
    }

    if (IsFileExisted(custConfig_.custSoundsPath)) {
        LOGI("custsound path %{public}s", custConfig_.custSoundsPath.c_str());
        retstr = "file:/" + custConfig_.custSoundsPath;
    }
    return retstr;
}

std::string BootAnimationConfig::GetCustomCfgFile()
{
    std::string ret = "";
    char buf[MAX_PATH_LEN] = {0};
    char *pfile = GetOneCfgFile(BOOT_CUSTOM_PATHSUFFIX.c_str(), buf, MAX_PATH_LEN);
    if (pfile != nullptr) {
        LOGI("get one filePath:%{public}s", pfile);
        return pfile;
    } else {
        LOGI("not find %{public}s", BOOT_CUSTOM_PATHSUFFIX.c_str());
    }
    return ret;
}

std::string BootAnimationConfig::GetBootVideoPath()
{
    if (custConfig_.custVideoPath.empty()) {
        return BOOT_VIDEO_PATH;
    }
    return custConfig_.custVideoPath;
}

std::string BootAnimationConfig::GetBootExtraVideoPath()
{
    if (custConfig_.custExtraVideoPath.empty()) {
        return GetBootVideoPath();
    }
    return custConfig_.custExtraVideoPath;
}

int32_t BootAnimationConfig::GetRotateScreenId()
{
    return custConfig_.rotateScreenId;
}

int32_t BootAnimationConfig::GetRotateDegree()
{
    return custConfig_.rotateDegree;
}

void BootAnimationConfig::ParserCustomCfgFile()
{
    std::string file = GetCustomCfgFile();
    if (!file.empty()) {
        ReadCustomBootConfig(file, custConfig_);
        CheckBootVideoEnabled();
    }
}

bool BootAnimationConfig::IsBootVideoEnabled()
{
    return bootVideoEnabled_;
}
