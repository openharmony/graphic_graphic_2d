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

#ifndef RENDER_PROFILER_APPLICATION
#include <securec.h>

#include "rs_profiler_log.h"
#else
#include "rs_adapt.h"
#endif

namespace OHOS::Rosen {

class RSB_EXPORT Archive {
public:
    bool IsReading() const
    {
        return reader_;
    }

    bool Good() const
    {
        return !corrupted_;
    }

    explicit operator bool() const
    {
        return Good();
    }

    bool operator!() const
    {
        return !Good();
    }

    Archive& Serialize(char& value);
    Archive& Serialize(float& value);
    Archive& Serialize(double& value);

    Archive& Serialize(int8_t& value);
    Archive& Serialize(int16_t& value);
    Archive& Serialize(int32_t& value);
    Archive& Serialize(int64_t& value);

    Archive& Serialize(uint8_t& value);
    Archive& Serialize(uint16_t& value);
    Archive& Serialize(uint32_t& value);
    Archive& Serialize(uint64_t& value);

    Archive& Serialize(std::string& value);

    template<typename T>
    Archive& Serialize(std::vector<T>& vector)
    {
        if (SerializeVectorBase(vector)) {
            Serialize(vector.data(), vector.size());
        }
        return *this;
    }

    template<typename T>
    Archive& Serialize(std::vector<T>& vector, void (*serializer)(Archive&, T&))
    {
        if (serializer && SerializeVectorBase(vector)) {
            for (T& value : vector) {
                serializer(*this, value);
            }
        }
        return *this;
    }

    template<typename T>
    Archive& SerializeNonFlat(std::vector<T>& vector)
    {
        if (SerializeVectorBase(vector)) {
            for (T& value : vector) {
                value.Serialize(*this);
            }
        }
        return *this;
    }

    Archive& Serialize(void* data, size_t size);

protected:
    explicit Archive(bool reader) : reader_(reader) {}

    virtual ~Archive() = default;

    template<typename T>
    Archive& SerializeVectorBase(std::vector<T>& vector)
    {
        size_t size = vector.size();
        Serialize(size);

        if (IsReading() && Good()) {
            constexpr auto maxSize = 128u * 1024u * 1024u;
            MarkCorruptedIf(size >= maxSize);
            vector.resize(Good() ? size : 0);
        }
        return *this;
    }

    void MarkCorruptedIf(bool condition)
    {
        corrupted_ |= condition;
    }

    virtual bool Read(void* data, size_t size) = 0;
    virtual bool Write(const void* data, size_t size) = 0;

private:
    bool reader_ = true;
    bool corrupted_ = false;
};

// Data archives
template<bool Reader>
class DataArchive final : public Archive {
public:
    explicit DataArchive(const std::vector<char>& data) : Archive(Reader), data_(const_cast<std::vector<char>&>(data))
    {}

protected:
    bool Read(void* data, size_t size) override
    {
        if (Good() && (offset_ + size <= data_.size()) && Utils::Move(data, size, data_.data() + offset_, size)) {
            offset_ += size;
            return true;
        }
        return false;
    }

    bool Write(const void* data, size_t size) override
    {
        if (!Good() || (size > data_.max_size() - data_.size())) {
            return false;
        }

        data_.resize(data_.size() + size);
        if (Utils::Move(data_.data() + offset_, size, data, size)) {
            offset_ += size;
            return true;
        }
        return false;
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
            HRPE("FileArchive: Cannot open '%s' for %s", path.data(), (Reader ? "reading" : "writing"));
        }
    }

    ~FileArchive() override
    {
        if (!external_) {
            Utils::FileClose(file_);
        }
    }

protected:
    bool Read(void* data, size_t size) override
    {
        return Good() && Utils::FileRead(file_, data, size);
    }

    bool Write(const void* data, size_t size) override
    {
        return Good() && Utils::FileWrite(file_, data, size);
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
    bool Read(void* data, size_t size) override
    {
        return Good() && !!stream_.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(size));
    }

    bool Write(const void* data, size_t size) override
    {
        return Good() && !!stream_.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
    }

private:
    std::stringstream& stream_;
};

using StringStreamReader = StringStreamArchive<true>;
using StringStreamWriter = StringStreamArchive<false>;

} // namespace OHOS::Rosen

#endif // RS_PROFILER_ARCHIVE_H