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

#include "rs_profiler_cache.h"

#include "rs_profiler_archive.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

std::atomic_uint64_t ImageCache::id_ = 0u;
std::mutex ImageCache::mutex_;
std::map<uint64_t, Image> ImageCache::cache_;
std::atomic_size_t ImageCache::consumption_;

bool Image::IsValid() const
{
    return (!data.empty()) && (data.size() < maxSize);
}

size_t Image::Size() const
{
    return data.size();
}

void Image::Serialize(Archive& archive)
{
    archive.Serialize(data);
    archive.Serialize(parcelSkipBytes);
    archive.Serialize(dmaSize);
    archive.Serialize(dmaWidth);
    archive.Serialize(dmaHeight);
    archive.Serialize(dmaStride);
    archive.Serialize(dmaFormat);
    archive.Serialize(dmaUsage);
}

// ImageCache
uint64_t ImageCache::New()
{
    return Utils::ComposeNodeId(Utils::GetPid(), id_++);
}

bool ImageCache::Exists(uint64_t id)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    return (cache_.count(id) > 0);
}

bool ImageCache::Add(uint64_t id, Image&& image)
{
    if (image.IsValid() && !Exists(id)) {
        consumption_ += image.Size();
        const std::lock_guard<std::mutex> guard(mutex_);
        cache_.insert({ id, image });
        return true;
    }
    return false;
}

Image* ImageCache::Get(uint64_t id)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    const auto item = cache_.find(id);
    return (item != cache_.end()) ? &item->second : nullptr;
}

size_t ImageCache::Size()
{
    const std::lock_guard<std::mutex> guard(mutex_);
    return cache_.size();
}

size_t ImageCache::Consumption()
{
    return consumption_;
}

void ImageCache::Reset()
{
    id_ = 0;
    consumption_ = 0u;
    const std::lock_guard<std::mutex> guard(mutex_);
    cache_.clear();
}

void ImageCache::Serialize(Archive& archive)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    uint32_t count = cache_.size();
    archive.Serialize(count);
    for (auto& item : cache_) {
        archive.Serialize(const_cast<uint64_t&>(item.first));
        item.second.Serialize(archive);
    }
}

// temporary: code has to be moved to Serialize due to Archive's architecture
void ImageCache::Deserialize(Archive& archive)
{
    Reset();

    const std::lock_guard<std::mutex> guard(mutex_);
    uint32_t count = 0u;
    archive.Serialize(count);
    for (uint32_t i = 0; i < count; i++) {
        uint64_t id = 0u;
        archive.Serialize(id);

        Image image {};
        image.Serialize(archive);
        cache_.insert({ id, image });
    }
}

// deprecated
void ImageCache::Serialize(FILE* file)
{
    FileWriter archive(file);
    Serialize(archive);
}

// deprecated
void ImageCache::Deserialize(FILE* file)
{
    FileReader archive(file);
    Deserialize(archive);
}

// deprecated
void ImageCache::Serialize(std::stringstream& stream)
{
    StringStreamWriter archive(stream);
    Serialize(archive);
}

// deprecated
void ImageCache::Deserialize(std::stringstream& stream)
{
    StringStreamReader archive(stream);
    Deserialize(archive);
}

std::string ImageCache::Dump()
{
    std::string out;

    const std::lock_guard<std::mutex> guard(mutex_);
    for (const auto& item : cache_) {
        out += std::to_string(Utils::ExtractPid(item.first)) + ":" + std::to_string(Utils::ExtractNodeId(item.first)) +
               " ";
    }

    return out;
}

} // namespace OHOS::Rosen