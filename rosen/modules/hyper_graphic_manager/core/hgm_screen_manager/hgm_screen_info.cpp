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

#include "hgm_screen_info.h"

#include <cerrno>
#include <dlfcn.h>
#include <sstream>
#include <unordered_map>

#include "file_ex.h"
#include "hgm_log.h"
#include "hgm_command.h"

namespace OHOS {
namespace Rosen {
namespace {
static std::unordered_map<std::string, HgmScreenType> g_strToHgmScreenType = {
    { "LTPS", HgmScreenType::LTPS },
    { "LTPS1", HgmScreenType::LTPS1 },
    { "LTPS2", HgmScreenType::LTPS2 },
    { "LTPO", HgmScreenType::LTPO },
    { "LTPO1", HgmScreenType::LTPO1 },
    { "LTPO2", HgmScreenType::LTPO2 },
    { "LTPO3", HgmScreenType::LTPO3 },
};
}

const std::string HgmScreenInfo::HGM_EXT_SO_PATH = "/system/lib64/libhyper_graphic_manager_ext.z.so";
const std::string HgmScreenInfo::GET_PANEL_INFO_SYMBOL = "GetPanelInfo";

HgmScreenInfo& HgmScreenInfo::GetInstance()
{
    static HgmScreenInfo instance;
    return instance;
}

HgmScreenInfo::~HgmScreenInfo()
{
    if (handle_ != nullptr) {
        dlclose(handle_);
        handle_ = nullptr;
        func_ = nullptr;
    }
    mPanelInfo_.clear();
}

inline bool IsPosNumber(const std::string& str)
{
    if (str.empty()) {
        return false;
    }
    for (char c: str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

inline HgmScreenType StringToScreenType(const std::string& str)
{
    auto it = g_strToHgmScreenType.find(str);
    if (it != g_strToHgmScreenType.end()) {
        return it->second;
    }
    return HgmScreenType::LTPO;
}

std::vector<std::string> StringSplit(const std::string& str, char delimiter)
{
    std::vector<std::string> res;
    if (str.empty()) {
        return res;
    }
    std::string s;
    std::istringstream strStream(str);
    while (std::getline(strStream, s, delimiter)) {
        if (!s.empty()) {
            s.erase(0, s.find_first_not_of(' '));
            s.erase(s.find_last_not_of(' ') + 1);
            res.push_back(s);
        }
    }
    return res;
}

std::vector<int32_t> VectorStringToVectorInt(const std::vector<std::string>& vstr)
{
    std::vector<int32_t> res;
    for (const std::string& s: vstr) {
        if (IsPosNumber(s)) {
            res.emplace_back(atoi(s.c_str()));
        }
    }
    return res;
}

void HgmScreenInfo::Init()
{
    if (FileExists(HGM_EXT_SO_PATH)) {
        handle_ = dlopen(HGM_EXT_SO_PATH.c_str(), RTLD_LAZY);
    } else {
        HGM_LOGW("HgmScreenInfo::Init cannot find lcd hdi convert lib:%{public}s", HGM_EXT_SO_PATH.c_str());
        return;
    }
    if (handle_ == nullptr) {
        auto error = dlerror();
        if (error != nullptr) {
            HGM_LOGW("HgmScreenInfo::Init cannot open lcd hdi convert lib, dlerror:%{public}s", error);
        }
        HGM_LOGW("HgmScreenInfo::Init cannot open lcd hdi convert lib, errno:%{public}d", errno);
        return;
    }
    func_ = reinterpret_cast<ConvertFuncGet>(dlsym(handle_, GET_PANEL_INFO_SYMBOL.c_str()));
    if (func_ == nullptr) {
        auto error = dlerror();
        if (error != nullptr) {
            HGM_LOGW("HgmScreenInfo::Init cannot find symbol:%{public}s, dlerror:%{public}s",
                GET_PANEL_INFO_SYMBOL.c_str(), error);
        }
        HGM_LOGW("HgmScreenInfo::Init cannot find symbol:%{public}s, errno:%{public}d",
            GET_PANEL_INFO_SYMBOL.c_str(), errno);
        return;
    }
}

int32_t HgmScreenInfo::GetPanelInfo(std::string& panelInfo)
{
    if (func_ == nullptr) {
        Init();
        if (func_ == nullptr) {
            HGM_LOGE("HgmScreenInfo::Init init convert failed");
            return HGM_ERROR;
        }
    }
    int32_t displayGetFpsRet = 0;
    displayGetFpsRet = (*func_)(panelInfo);
    if (displayGetFpsRet != 0) {
        HGM_LOGE("HgmScreenInfo::GetPanelInfo DisplayGetFps failed:%{public}d", displayGetFpsRet);
        return HGM_ERROR;
    }
    return EXEC_SUCCESS;
}

int32_t HgmScreenInfo::InitPanelTypeMap(std::string& panelInfo)
{
    int32_t ret = GetPanelInfo(panelInfo);
    if (ret != EXEC_SUCCESS) {
        return ret;
    }

    // initial get each pair of (panel id and panel type), this's not gonna change
    static std::string panelIdPre = "panel_id";
    static std::string panelTypePre = "panel_type";
    static std::string supportFpsPre = "support_fps";
    static char panelSeparator = ';';
    static char panelEntrySeparator = ':';
    static char fpsSeparator = ',';
    static uint32_t entrySize = 2;
    int32_t lastPanelId;
    std::vector<std::string> panelList;
    std::vector<std::string> entrys;

    panelList = StringSplit(panelInfo, panelSeparator);
    for (const std::string& str: panelList) {
        entrys = StringSplit(str, panelEntrySeparator);
        if (entrys.size() != entrySize) {
            continue;
        }
        if (entrys[0] == panelIdPre) {
            if (IsPosNumber(entrys[1])) {
                lastPanelId = atoi(entrys[1].c_str());
                mPanelInfo_[lastPanelId].panelId = lastPanelId;
            } else {
                HGM_LOGE("HgmScreenInfo::InitPanelTypeMap panel id is not a number:%{public}s", entrys[1].c_str());
                mPanelInfo_.clear();
                return HGM_ERROR;
            }
        } else if (entrys[0] == panelTypePre) {
            mPanelInfo_[lastPanelId].panelType = StringToScreenType(entrys[1]);
        } else if (entrys[0] == supportFpsPre) {
            HGM_LOGI("HgmScreenInfo::InitPanelTypeMap PanelId:%{public}d, PanelType:%{public}d, SupportFps:%{public}s",
                lastPanelId, mPanelInfo_[lastPanelId].panelType, entrys[1].c_str());
            entrys = StringSplit(entrys[1], fpsSeparator);
            mPanelInfo_[lastPanelId].supportFps = VectorStringToVectorInt(entrys);
        }
    }
    return EXEC_SUCCESS;
}

HgmScreenType HgmScreenInfo::GetScreenType(ScreenId screenId)
{
    HgmScreenType curPanelType = HgmScreenType::LTPO;
    std::string panelInfo;
    int32_t ret = HGM_ERROR;

    if (mPanelInfo_.empty()) {
        ret = InitPanelTypeMap(panelInfo);
        HGM_LOGI("HgmScreenInfo::GetScreenType init mPanelInfo_, size:%{public}lu", mPanelInfo_.size());
        if (ret != EXEC_SUCCESS || mPanelInfo_.empty()) {
            return curPanelType;
        }
    } else {
        ret = GetPanelInfo(panelInfo);
        if (ret != EXEC_SUCCESS) {
            return curPanelType;
        }
    }

    // get current panel id
    static std::string curPanelIdPre = "current_panel_id";
    static char panelSeparator = ';';
    int32_t curPanelId = -1;
    size_t startPos = panelInfo.find(curPanelIdPre);
    if (startPos != std::string::npos) {
        startPos += curPanelIdPre.length() + 1;
        size_t endPos = panelInfo.find(panelSeparator, startPos);
        if (endPos != std::string::npos) {
            std::string res = panelInfo.substr(startPos, endPos - startPos);
            if (IsPosNumber(res)) {
                curPanelId = atoi(res.c_str());
                curPanelType = mPanelInfo_[curPanelId].panelType;
            } else {
                HGM_LOGE("HgmScreenInfo::GetScreenType panel id is not a number:%{public}s", res.c_str());
            }
        }
    }
    HGM_LOGI("0816HgmScreenInfo::GetScreenType Current: ScreenId:%{public}lu, PanelId:%{public}d, PanelType:"\
        "%{public}d", screenId, curPanelId, curPanelType);
    return curPanelType;
}

bool HgmScreenInfo::IsLtpoType(HgmScreenType screenType)
{
    if (screenType >= HgmScreenType::LTPO) {
        return true;
    }
    return false;
}

} // namespace Rosen
} // namespace OHOS
