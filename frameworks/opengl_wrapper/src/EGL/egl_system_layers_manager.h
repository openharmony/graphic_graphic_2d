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

#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_SYSTEM_LAYERS_MANAGER_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_SYSTEM_LAYERS_MANAGER_H

#include <string>
#include <vector>
#include <unistd.h>

#include "json/json.h"

/*
Example of json config:

{
    "enableAppMode" : true,

    "appMode" : {
        "enableDefaultAppMode" : true,

        "default" : ["layer1", "layer2"],

        "com.hiawei.app" : ["layer1", "layer2"]
    }
}
*/

namespace OHOS {

class EglSystemLayersManager {
public:
    EglSystemLayersManager() = default;
    ~EglSystemLayersManager() = default;

    std::vector<std::string> GetSystemLayers();

private:
    bool GetProcessName(pid_t pid, char *pname, int len);
    bool GetJsonConfig(Json::Value &configData);
    std::vector<std::string> GetSystemLayersFromConfig(Json::Value &appModeSection,
        const std::string &processName);
    std::vector<std::string> GetStringVectorFromJson(const Json::Value &jsonVector);
};

} //namespace OHOS

#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_SYSTEM_LAYERS_MANAGER_H