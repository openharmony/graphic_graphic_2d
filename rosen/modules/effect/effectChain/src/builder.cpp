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

#include "filter_factory.h"
#include "ec_log.h"
#include "builder.h"

namespace OHOS {
namespace Rosen {
ImageChain* Builder::CreateFromConfig(std::string path)
{
    std::ifstream configFile;
    // open files
    configFile.open(path.c_str());
    std::stringstream JFilterParamsStream;
    // read file's buffer contents into streams
    JFilterParamsStream << configFile.rdbuf();
    // close file handlers
    configFile.close();
    // convert stream into string
    std::string JFilterParamsString = JFilterParamsStream.str();
    cJSON* overallData = cJSON_Parse(JFilterParamsString.c_str());
    if (overallData != nullptr) {
        filters_ = cJSON_GetObjectItem(overallData, "filters");
        if (filters_ != nullptr) {
            AnalyseFilters(filters_);
        }
        connections_ = cJSON_GetObjectItem(overallData, "connections");
        if (connections_ != nullptr) {
            ConnectPipeline(connections_);
        }
    } else {
        LOGE("The json file fails to compile.");
        return nullptr;
    }
    if (inputs_.size() != 0) {
        return new ImageChain(inputs_);
    } else {
        LOGE("No input.");
        return nullptr;
    }
}

void Builder::AnalyseFilters(cJSON* filters)
{
    int size = cJSON_GetArraySize(filters);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(filters, i);
        cJSON* type = cJSON_GetObjectItem(item, "type");
        cJSON* name = cJSON_GetObjectItem(item, "name");
        cJSON* params = cJSON_GetObjectItem(item, "params");
        if (type != nullptr && name != nullptr) {
            nameType_[name->valuestring] = type->valuestring;
            auto tempFilter = algoFilterFactory->GetFilter(type);
            if (tempFilter != nullptr) {
                ParseParams(tempFilter, params);
                nameFilter_[name->valuestring] = tempFilter;
            }
        }
    }
}

void Builder::ParseParams(std::shared_ptr<Filter> filter, cJSON* params)
{
    if (params == nullptr) {
        return;
    }
    cJSON* childParam = params->child;
    while (childParam != nullptr) {
        if (cJSON_IsArray(childParam)) {
            int arrayLength = cJSON_GetArraySize(childParam);
            std::vector<float> tempArray(arrayLength, 0);
            for (int i = 0; i < arrayLength; i++) {
                cJSON* arrayItem = cJSON_GetArrayItem(childParam, i);
                tempArray[i] = arrayItem->valuedouble;
            }
            filter->SetValue(childParam->string, &tempArray[0], arrayLength);
        } else if (cJSON_IsNumber(childParam)) {
            float tempValue = childParam->valuedouble;
            filter->SetValue(childParam->string, &tempValue, 1);
        } else if (cJSON_IsString(childParam)) {
            filter->SetValue(childParam->string, childParam->valuestring, 1);
        } else {
            LOGE("Invalid input parameters!");
        }
        childParam = childParam->next;
    }
}

void Builder::ConnectPipeline(cJSON* connections)
{
    int size = cJSON_GetArraySize(connections);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(connections, i);
        cJSON* from = cJSON_GetObjectItem(item, "from");
        cJSON* to = cJSON_GetObjectItem(item, "to");
        std::shared_ptr<Filter> fFilter = nullptr;
        std::shared_ptr<Filter> tFilter = nullptr;
        if (from != nullptr && to != nullptr) {
            std::string fTypeName = nameType_[from->valuestring];
            auto itFrom = nameFilter_.find(from->valuestring);
            if (itFrom != nameFilter_.end()) {
                fFilter = itFrom->second;
                if (fFilter->GetFilterType() == FILTER_TYPE::INPUT) {
                    inputs_.push_back(std::static_pointer_cast<Input>(fFilter));
                }
            } else {
                LOGE("The from filter %{public}s fails to be connected", from->valuestring);
            }
            auto itTo = nameFilter_.find(to->valuestring);
            if (itTo != nameFilter_.end()) {
                tFilter = itTo->second;
            } else {
                LOGE("The to filter %{public}s fails to be connected", to->valuestring);
            }
            if (fFilter != nullptr && tFilter != nullptr) {
                fFilter->AddNextFilter(tFilter);
            }
        }
    }
}
} // namespcae Rosen
} // namespace OHOS
