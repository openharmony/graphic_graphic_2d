/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "egl_system_layers_manager.h"

#include <climits>
#include <cstdlib>
#include <dlfcn.h>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "json/json.h"

#include "wrapper_log.h"

namespace OHOS {

namespace {

const std::string defaultJsonConfig = R"__(
{
    "enableAppMode" : true,
    "appMode" : {
        "enableDefaultAppMode" : true,
        "default" : ["iGraphicsCore.z"],
    }
}
)__";

} // end of anonymous namespace

bool EglSystemLayersManager::GetProcessName(pid_t pid, char *pname, int len)
{
    std::string const fileName = std::string{"/proc/"} + std::to_string(pid) + std::string("/cmdline");
    char realFileName[PATH_MAX] = { '\0' };

    if (realpath(fileName.c_str(), realFileName) == nullptr) {
        WLOGE("realpath failed");
        return false;
    }

    FILE *f = fopen(realFileName, "r");
    if (f == nullptr) {
        WLOGE("fopen %{private}s cmdline file failed", realFileName);
        *pname = 0;
        return false;
    }

    if (fgets(pname, len, f) == nullptr) {
        WLOGE("fgets of cmdline failed");
        *pname = 0;
        if (fclose(f)) {
            WLOGE("failed to close %{private}s file", realFileName);
        }
        return false;
    }

    if (*pname == 0) {
        WLOGE("process name is empty");
        if (fclose(f)) {
            WLOGE("failed to close %{private}s file", realFileName);
        }
        return false;
    }

    if (fclose(f)) {
        WLOGE("failed to close %{private}s file", realFileName);
    }
    return true;
}

bool EglSystemLayersManager::GetJsonConfig(Json::Value &configData)
{
    Json::CharReaderBuilder builder;
    Json::CharReader *charReader = builder.newCharReader();
    if (!charReader) {
        WLOGE("Failed to create new Json::CharReader");
        return false;
    }
    const std::unique_ptr<Json::CharReader> reader(charReader);
    JSONCPP_STRING errs;
    bool ret = reader->parse(defaultJsonConfig.c_str(),
        defaultJsonConfig.c_str() + static_cast<int>(defaultJsonConfig.length()), &configData, &errs);
    if (!ret) {
        WLOGE("json parse error: %{private}s", errs.c_str());
        return false;
    } else {
        WLOGD("json parse success");
    }

    return true;
}

std::vector<std::string> EglSystemLayersManager::GetStringVectorFromJson(const Json::Value &jsonVector)
{
    std::vector<std::string> stringVector{};

    for (const auto &i : jsonVector) {
        if (i.isString()) {
            stringVector.push_back(i.asString());
        } else {
            WLOGD("%{private}s is not a string", i.asString().c_str());
        }
    }

    return stringVector;
}

std::vector<std::string> EglSystemLayersManager::GetSystemLayersFromConfig(Json::Value &appModeSection,
    const std::string &processName)
{
    if (!appModeSection.isMember(processName)) {
        std::string enableDefaultAppModeName("enableDefaultAppMode");
        if (!appModeSection.isMember(enableDefaultAppModeName)) {
            WLOGE("Failed to find %{private}s section", enableDefaultAppModeName.c_str());
            return std::vector<std::string>{};
        }

        if (!appModeSection[enableDefaultAppModeName].isBool()) {
            WLOGE("Failed to get value of %{private}s section, not a boolean", enableDefaultAppModeName.c_str());
            return std::vector<std::string>{};
        }

        if (!appModeSection[enableDefaultAppModeName].asBool()) {
            return std::vector<std::string>{};
        }

        std::string defaultName("default");
        if (!appModeSection.isMember(defaultName)) {
            WLOGE("Failed to find default app mode");
            return std::vector<std::string>{};
        }

        const Json::Value defaultArray = appModeSection[defaultName];
        if (!defaultArray.isArray()) {
            WLOGE("%{private}s value is not array", defaultName.c_str());
            return std::vector<std::string>{};
        }

        return GetStringVectorFromJson(defaultArray);
    }

    const Json::Value layersArray = appModeSection[processName];
    if (!layersArray.isArray()) {
        WLOGE("%{private}s value is not array", processName.c_str());
        return std::vector<std::string>{};
    }

    return GetStringVectorFromJson(layersArray);
}

std::vector<std::string> EglSystemLayersManager::GetSystemLayers()
{
    Json::Value configData{};
    if (!GetJsonConfig(configData)) {
        WLOGE("Failed to get json config");
        return std::vector<std::string>{};
    }

    std::string enableAppModeName("enableAppMode");
    if (!configData.isMember(enableAppModeName)) {
        WLOGE("Failed to find %{private}s parameter in json config", enableAppModeName.c_str());
        return std::vector<std::string>{};
    }

    if (!configData[enableAppModeName].isBool()) {
        WLOGE("Failed to get %{private}s parameter form config, not a boolean", enableAppModeName.c_str());
        return std::vector<std::string>{};
    }

    if (!configData[enableAppModeName].asBool()) {
        return std::vector<std::string>{};
    }

    const std::string appModeSectionName("appMode");
    if (!configData.isMember(appModeSectionName)) {
        WLOGE("Failed to find %{private}s section in config", appModeSectionName.c_str());
        return std::vector<std::string>{};
    }

    constexpr int pnameLen = 512;
    char pname[pnameLen + 1] = {0};
    bool res = GetProcessName(getpid(), pname, pnameLen);
    if (!res) {
        WLOGE("Failed to get process name");
        return std::vector<std::string>{};
    } else {
        WLOGD("GetProcessName() = %{public}s", pname);
    }

    Json::Value appModeSection = configData[appModeSectionName];

    return GetSystemLayersFromConfig(appModeSection, std::string(pname));
}

} //namespace OHOS
