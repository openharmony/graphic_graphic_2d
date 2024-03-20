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

#include "rs_profiler_capturedata.h"

#include "rs_profiler_archive.h"

namespace OHOS::Rosen {

RSCaptureData::RSCaptureData() = default;

RSCaptureData::~RSCaptureData() = default;

void RSCaptureData::Reset()
{
    time_ = 0.0f;
    properties_.clear();
}

void RSCaptureData::SetTime(float time)
{
    time_ = time;
}

float RSCaptureData::GetTime() const
{
    return time_;
}

void RSCaptureData::SetProperty(const std::string& name, const std::string& value)
{
    properties_[name] = value;
}

const std::string& RSCaptureData::GetProperty(const std::string& name) const
{
    static const std::string DEFAULT_STR = "0";
    const auto found = properties_.find(name);
    return (found != properties_.end()) ? found->second : DEFAULT_STR;
}

float RSCaptureData::GetPropertyFloat(const std::string& name) const
{
    return std::stof(GetProperty(name));
}

double RSCaptureData::GetPropertyDouble(const std::string& name) const
{
    return std::stod(GetProperty(name));
}

int8_t RSCaptureData::GetPropertyInt8(const std::string& name) const
{
    return static_cast<int8_t>(std::stoi(GetProperty(name)));
}

uint8_t RSCaptureData::GetPropertyUint8(const std::string& name) const
{
    return static_cast<uint8_t>(std::stoul(GetProperty(name)));
}

int16_t RSCaptureData::GetPropertyInt16(const std::string& name) const
{
    return static_cast<int16_t>(std::stoi(GetProperty(name)));
}

uint16_t RSCaptureData::GetPropertyUint16(const std::string& name) const
{
    return static_cast<uint16_t>(std::stoul(GetProperty(name)));
}

int32_t RSCaptureData::GetPropertyInt32(const std::string& name) const
{
    return static_cast<int32_t>(std::stol(GetProperty(name)));
}

uint32_t RSCaptureData::GetPropertyUint32(const std::string& name) const
{
    return static_cast<uint32_t>(std::stoul(GetProperty(name)));
}

int64_t RSCaptureData::GetPropertyInt64(const std::string& name) const
{
    return static_cast<int64_t>(std::stoll(GetProperty(name)));
}

uint64_t RSCaptureData::GetPropertyUint64(const std::string& name) const
{
    return static_cast<uint64_t>(std::stoull(GetProperty(name)));
}

void RSCaptureData::Serialize(std::vector<char>& out)
{
    DataWriter archive(out);
    Serialize(archive);
}

void RSCaptureData::Deserialize(const std::vector<char>& in)
{
    Reset();

    DataReader archive(in);
    Serialize(archive);
}

void RSCaptureData::Serialize(Archive& archive)
{
    archive.Serialize(time_);

    size_t size = properties_.size();
    archive.Serialize(size);

    if (archive.IsReading()) {
        std::string first;
        std::string second;

        for (size_t i = 0; i < size; i++) {
            archive.Serialize(first);
            archive.Serialize(second);

            properties_[first] = second;
        }
    } else {
        for (auto& pair : properties_) {
            archive.Serialize(const_cast<std::string&>(pair.first));
            archive.Serialize(const_cast<std::string&>(pair.second));
        }
    }
}

} // namespace OHOS::Rosen