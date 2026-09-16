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
std::map<uint64_t, std::shared_ptr<Image>> ImageCache::cache_;
std::atomic_size_t ImageCache::consumption_;

static std::shared_ptr<Image> ReadImage(Archive& archive)
{
    auto image = std::make_shared<Image>();
    return (image && image->Serialize(archive)) ? image : nullptr;
}

bool Image::IsValid() const
{
    return (!data.empty()) && (data.size() < maxSize);
}

size_t Image::Size() const
{
    return data.size();
}

bool Image::Serialize(Archive& archive)
{
    return archive.Serialize(data)
        .Serialize(parcelSkipBytes)
        .Serialize(dmaSize)
        .Serialize(dmaWidth)
        .Serialize(dmaHeight)
        .Serialize(dmaStride)
        .Serialize(dmaFormat)
        .Serialize(dmaUsage)
        .Good();
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

bool ImageCache::Add(uint64_t id, const std::shared_ptr<Image>& image)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    return Insert(id, image);
}

bool ImageCache::Add(uint64_t id, Image&& image)
{
    return Add(id, std::make_shared<Image>(std::move(image)));
}

bool ImageCache::Insert(uint64_t id, const std::shared_ptr<Image>& image)
{
    if (image && image->IsValid() && Fits(image->Size())) {
        if (cache_.insert({ id, image }).second) {
            consumption_ += image->Size();
            return true;
        }
    }
    return false;
}

std::shared_ptr<Image> ImageCache::Get(uint64_t id)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    const auto item = cache_.find(id);
    return (item != cache_.end()) ? item->second : nullptr;
}

Image ImageCache::Copy(uint64_t id)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    const auto item = cache_.find(id);
    return (item != cache_.end()) ? *item->second : Image {};
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

bool ImageCache::Fits(size_t size)
{
    constexpr size_t maxConsumption = 1024u * 1024u * 1024u;
    return (Consumption() + size) <= maxConsumption;
}

void ImageCache::Reset()
{
    id_ = 0;
    const std::lock_guard<std::mutex> guard(mutex_);
    Clear();
}

void ImageCache::Clear()
{
    consumption_ = 0u;
    cache_.clear();
}

bool ImageCache::Serialize(Archive& archive)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    uint32_t count = cache_.size();
    if (!archive.Serialize(count)) {
        return false;
    }

    for (auto& item : cache_) {
        if (!archive.Serialize(const_cast<uint64_t&>(item.first)) || !item.second->Serialize(archive)) {
            return false;
        }
    }
    return true;
}

bool ImageCache::Deserialize(Archive& archive)
{
    const std::lock_guard<std::mutex> guard(mutex_);
    Clear();

    uint32_t count = 0u;
    if (!archive.Serialize(count)) {
        return false;
    }

    for (uint32_t i = 0; i < count; i++) {
        uint64_t id = 0u;
        if (!archive.Serialize(id) || !Insert(id, ReadImage(archive))) {
            Clear();
            return false;
        }
    }
    return true;
}

// deprecated
bool ImageCache::Serialize(FILE* file)
{
    FileWriter archive(file);
    return Serialize(archive);
}

// deprecated
bool ImageCache::Deserialize(FILE* file)
{
    FileReader archive(file);
    return Deserialize(archive);
}

// deprecated
bool ImageCache::Serialize(std::stringstream& stream)
{
    StringStreamWriter archive(stream);
    return Serialize(archive);
}

// deprecated
bool ImageCache::Deserialize(std::stringstream& stream)
{
    StringStreamReader archive(stream);
    return Deserialize(archive);
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