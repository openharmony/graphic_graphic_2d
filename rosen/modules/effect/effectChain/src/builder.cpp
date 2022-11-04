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
    char newpath[PATH_MAX + 1] = { 0x00 };
    if (strlen(path.c_str()) > PATH_MAX || realpath(path.c_str(), newpath) == NULL) {
        return nullptr;
    }
    
    std::ifstream configFile;
    configFile.open(newpath);
    std::stringstream JFilterParamsStream;
    JFilterParamsStream << configFile.rdbuf();
    configFile.close();
    std::string JFilterParamsString = JFilterParamsStream.str();
    auto overallData = std::shared_ptr<cJSON>(cJSON_Parse(JFilterParamsString.c_str()));
    if (overallData != nullptr) {
        filters_ = std::shared_ptr<cJSON>(cJSON_GetObjectItem(overallData.get(), "filters"));
        if (filters_ != nullptr) {
            AnalyseFilters(filters_.get());
        }
        connections_ = std::shared_ptr<cJSON>(cJSON_GetObjectItem(overallData.get(), "connections"));
        if (connections_ != nullptr) {
            ConnectPipeline(connections_.get());
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
        auto item = std::shared_ptr<cJSON>(cJSON_GetArrayItem(filters, i));
        auto type = std::shared_ptr<cJSON>(cJSON_GetObjectItem(item.get(), "type"));
        auto name = std::shared_ptr<cJSON>(cJSON_GetObjectItem(item.get(), "name"));
        auto params = std::shared_ptr<cJSON>(cJSON_GetObjectItem(item.get(), "params"));
        if (type != nullptr && name != nullptr) {
            nameType_[name->valuestring] = type->valuestring;
            auto tempFilter = filterFactory->GetFilter(type->valuestring);
            if (tempFilter != nullptr) {
                ParseParams(tempFilter, params.get());
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
    auto childParam = std::shared_ptr<cJSON>(params->child);
    while (childParam != nullptr) {
        if (cJSON_IsArray(childParam.get())) {
            ParseArray(filter, childParam.get());
        } else if (cJSON_IsNumber(childParam.get())) {
            std::shared_ptr<float> tempValue = std::make_shared<float>(childParam->valuedouble);
            filter->SetValue(childParam->string, tempValue, 1);
        } else if (cJSON_IsString(childParam.get())) {
            std::string tempString = childParam->valuestring;
            std::shared_ptr<std::string> tempValue = std::make_shared<std::string>(tempString);
            filter->SetValue(childParam->string, tempValue, 1);
        } else {
            LOGE("Invalid input parameters!");
        }
        childParam = std::shared_ptr<cJSON>(childParam->next);
    }
}

void Builder::ParseArray(std::shared_ptr<Filter> filter, cJSON* childParam)
{
    if (filter == nullptr || childParam == nullptr) {
        return;
    }
    int arrayLength = cJSON_GetArraySize(childParam);
    std::shared_ptr<std::vector<float>> tempArray = std::make_shared<std::vector<float>>(arrayLength, 0);
    for (int i = 0; i < arrayLength; i++) {
        auto arrayItem = std::shared_ptr<cJSON>(cJSON_GetArrayItem(childParam, i));
        if (cJSON_IsNumber(arrayItem.get())) {
            (*tempArray.get())[i] = arrayItem->valuedouble;
        }
    }
    filter->SetValue(childParam->string, tempArray, arrayLength);
}

void Builder::ConnectPipeline(cJSON* connections)
{
    int size = cJSON_GetArraySize(connections);
    for (int i = 0; i < size; i++) {
        auto item = std::shared_ptr<cJSON>(cJSON_GetArrayItem(connections, i));
        auto from = std::shared_ptr<cJSON>(cJSON_GetObjectItem(item.get(), "from"));
        auto to = std::shared_ptr<cJSON>(cJSON_GetObjectItem(item.get(), "to"));
        std::shared_ptr<Filter> fFilter = nullptr;
        std::shared_ptr<Filter> tFilter = nullptr;
        if (from != nullptr && to != nullptr) {
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
