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

#include "rs_profiler_settings.h"

#include <string>

namespace OHOS::Rosen {

std::mutex SystemParameter::mutex_;
std::vector<SystemParameter*> SystemParameter::registry_;

SystemParameter::SystemParameter(std::string name) : name_(std::move(name))
{
    if (!name_.empty() && !Exists(name_)) {
        const std::lock_guard<std::mutex> guard(mutex_);
        registry_.emplace_back(this);
    }
}

SystemParameter::~SystemParameter()
{
    const std::lock_guard<std::mutex> guard(mutex_);
    const auto position = std::find(registry_.begin(), registry_.end(), this);
    if (position != registry_.end()) {
        registry_.erase(position);
    }
}

SystemParameter& SystemParameter::operator=(const SystemParameter& parameter)
{
    if (this != &parameter) {
        FromString(parameter.ToString());
    }
    return *this;
}

SystemParameter* SystemParameter::Find(const std::string& name)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    const auto parameter = std::find_if(
        registry_.begin(), registry_.end(), [&name](SystemParameter* parameter) { return parameter->Name() == name; });
    return (parameter != registry_.end()) ? *parameter : nullptr;
}

bool SystemParameter::Exists(const std::string& name)
{
    return Find(name) != nullptr;
}

bool SystemParameter::Set(const std::string& name, const std::string& value)
{
    if (auto parameter = Find(name)) {
        parameter->FromString(value);
        return true;
    }
    return false;
}

std::string SystemParameter::Get(const std::string& name)
{
    static const std::string DUMMY;
    const auto parameter = Find(name);
    return parameter ? parameter->ToString() : DUMMY;
}

std::string SystemParameter::Dump()
{
    std::string out;
    const std::lock_guard<std::mutex> guard(mutex_);
    for (const auto parameter : registry_) {
        out += parameter->Name() + " = " + parameter->ToString() + "\n";
    }
    return out;
}

} // namespace OHOS::Rosen