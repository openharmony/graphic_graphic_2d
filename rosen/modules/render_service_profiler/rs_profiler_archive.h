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

#ifndef RS_PROFILER_ARCHIVE_H
#define RS_PROFILER_ARCHIVE_H

#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "rs_profiler_utils.h"

#ifndef REPLAY_TOOL_CLIENT
#include <securec.h>

#include "platform/common/rs_log.h"
#else
#include "rs_adapt.h"
#endif

namespace OHOS::Rosen {

class RSB_EXPORT Archive {
public:
    bool IsReading() const
    {
        return isReading_;
    }

    void Serialize(char& value);
    void Serialize(float& value);
    void Serialize(double& value);

    void Serialize(int8_t& value);
    void Serialize(int16_t& value);
    void Serialize(int32_t& value);
    void Serialize(int64_t& value);

    void Serialize(uint8_t& value);
    void Serialize(uint16_t& value);
    void Serialize(uint32_t& value);
    void Serialize(uint64_t& value);

    void Serialize(std::string& value);

    template<typename T>
    void Serialize(std::vector<T>& vector)
    {
        SerializeVectorBase(vector);
        Serialize(vector.data(), vector.size());
    }

    template<typename T>
    void Serialize(std::vector<T>& vector, void (*serializer)(Archive&, T&))
    {
        if (!serializer) {
            return;
        }

        SerializeVectorBase(vector);
        for (T& value : vector) {
            serializer(*this, value);
        }
    }

    template<typename T>
    void SerializeNonFlat(std::vector<T>& vector)
    {
        SerializeVectorBase(vector);
        for (T& value : vector) {
            value.Serialize(*this);
        }
    }

    void Serialize(void* data, size_t size);

protected:
    explicit Archive(bool reader) : isReading_(reader) {}

    virtual ~Archive() = default;

    template<typename T>
    void SerializeVectorBase(std::vector<T>& vector)
    {
        size_t size = vector.size();
        Serialize(size);

        if (IsReading()) {
            vector.resize(size);
        }
    }

    virtual void Read(void* data, size_t size) = 0;
    virtual void Write(const void* data, size_t size) = 0;

private:
    bool isReading_ = true;
};

// Data archives
template<bool Reader>
class DataArchive final : public Archive {
public:
    explicit DataArchive(const std::vector<char>& data) : Archive(Reader), data_(const_cast<std::vector<char>&>(data))
    {}

protected:
    void Read(void* data, size_t size) override
    {
        if ((offset_ + size <= data_.size()) && Utils::Move(data, size, data_.data() + offset_, size)) {
            offset_ += size;
        }
    }

    void Write(const void* data, size_t size) override
    {
        data_.resize(data_.size() + size);
        if (Utils::Move(data_.data() + offset_, size, data, size)) {
            offset_ += size;
        }
    }

protected:
    std::vector<char>& data_;
    size_t offset_ = 0;
};

using DataReader = DataArchive<true>;
using DataWriter = DataArchive<false>;

// File archives
template<bool Reader>
class FileArchive final : public Archive {
public:
    explicit FileArchive(FILE* file) : Archive(Reader), file_(file), external_(true) {}

    explicit FileArchive(const std::string& path) : Archive(Reader)
    {
        file_ = Utils::FileOpen(path, Reader ? "rb" : "wb");
        if (!file_) {
            RS_LOGE("FileArchive: File %s cannot be opened for %s", path.data(), // NOLINT
                (Reader ? "reading" : "writing"));
        }
    }

    ~FileArchive() override
    {
        if (!external_) {
            Utils::FileClose(file_);
        }
    }

protected:
    void Read(void* data, size_t size) override
    {
        Utils::FileRead(file_, data, size);
    }

    void Write(const void* data, size_t size) override
    {
        Utils::FileWrite(file_, data, size);
    }

protected:
    FILE* file_ = nullptr;
    bool external_ = false;
};

using FileReader = FileArchive<true>;
using FileWriter = FileArchive<false>;

// Stream archive
template<bool Reader>
class StringStreamArchive final : public Archive {
public:
    explicit StringStreamArchive(const std::stringstream& stream)
        : Archive(Reader), stream_(const_cast<std::stringstream&>(stream))
    {}

protected:
    void Read(void* data, size_t size) override
    {
        stream_.read(reinterpret_cast<char*>(data), size);
    }

    void Write(const void* data, size_t size) override
    {
        stream_.write(reinterpret_cast<const char*>(data), size);
    }

private:
    std::stringstream& stream_;
};

using StringStreamReader = StringStreamArchive<true>;
using StringStreamWriter = StringStreamArchive<false>;

} // namespace OHOS::Rosen

#endif // RS_PROFILER_ARCHIVE_H