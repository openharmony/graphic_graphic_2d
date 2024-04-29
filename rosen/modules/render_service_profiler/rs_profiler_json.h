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

#ifndef RENDER_SERVICE_PROFILER_JSONWRAPPER_H
#define RENDER_SERVICE_PROFILER_JSONWRAPPER_H

#include <sstream>

namespace OHOS::Rosen {
class JsonWriter {
public:
    JsonWriter() = default;
    ~JsonWriter() = default;

    void PushObject()
    {
        stream_ << "{";
    }

    void PopObject()
    {
        char last;
        stream_.seekg(-1, std::ios::end);
        stream_ >> last;
        if (last == ',') {
            stream_.seekp(-1, std::ios::end);
        }
        stream_ << "},";
    }

    void PushArray()
    {
        stream_ << "[";
    }

    void PopArray()
    {
        char last;
        stream_.seekg(-1, std::ios::end);
        stream_ >> last;
        if (last == ',') {
            stream_.seekp(-1, std::ios::end);
        }
        stream_ << "],";
    }

    template<typename T>
    void Append(const T& value)
    {
        Write(value);
        stream_ << ",";
    }

    JsonWriter& operator[](const std::string& key)
    {
        stream_ << "\"" << key << "\":";
        return *this;
    }

    template<typename T>
    JsonWriter& operator=(const T& value)
    {
        Write(value);
        stream_ << ",";
        return *this;
    }

    template<typename T>
    JsonWriter& operator=(std::initializer_list<T>&& values)
    {
        PushArray();
        for (const auto& value : values) {
            Write(value);
            stream_ << ",";
        }
        PopArray();
        return *this;
    }

    std::string GetDumpString()
    {
        char last;
        stream_.seekg(-1, std::ios::end);
        stream_ >> last;
        if (last == ',') {
            stream_.seekp(-1, std::ios::end);
            stream_ << '\0';
        }
        return stream_.str();
    }

private:
    template<typename T>
    void Write(const T& value)
    {
        stream_ << value;
    }

    template<>
    void Write(const float& value)
    {
        if (std::isinf(value) || std::isnan(value)) {
            stream_ << "null";
        } else {
            stream_ << value;
        }
    }

    template<>
    void Write(const std::string& value)
    {
        stream_ << "\"" << value << "\"";
    }

private:
    std::stringstream stream_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_PROFILER_JSONWRAPPER_H