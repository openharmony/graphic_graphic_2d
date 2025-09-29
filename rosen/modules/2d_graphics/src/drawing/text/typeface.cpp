/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "text/typeface.h"

#include "ashmem.h"
#include "static_factory.h"

#include "impl_interface/typeface_impl.h"
#include "static_factory.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
Typeface::Typeface(std::shared_ptr<TypefaceImpl> typefaceImpl) noexcept : typefaceImpl_(typefaceImpl) {}

std::shared_ptr<Typeface> Typeface::MakeDefault()
{
    return StaticFactory::MakeDefault();
}

std::shared_ptr<Typeface> Typeface::MakeFromFile(const char path[], int index)
{
    return StaticFactory::MakeFromFile(path, index);
}

std::shared_ptr<Typeface> Typeface::MakeFromFile(const char path[], const FontArguments& fontArguments)
{
    return StaticFactory::MakeFromFile(path, fontArguments);
}

std::vector<std::shared_ptr<Typeface>> Typeface::GetSystemFonts()
{
    return StaticFactory::GetSystemFonts();
}

void Typeface::RegisterOnTypefaceDestroyed(std::function<void(uint32_t)> cb)
{
    return StaticFactory::RegisterOnTypefaceDestroyed(cb);
}

std::shared_ptr<Typeface> Typeface::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index)
{
    return StaticFactory::MakeFromStream(std::move(memoryStream), index);
}

std::shared_ptr<Typeface> Typeface::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream,
    const FontArguments& fontArguments)
{
    return StaticFactory::MakeFromStream(std::move(memoryStream), fontArguments);
}

std::shared_ptr<Typeface> Typeface::MakeFromAshmem(int32_t fd, uint32_t size, uint32_t hash)
{
    auto ashmem = std::make_unique<Ashmem>(fd, size);
    bool mapResult = ashmem->MapReadOnlyAshmem();
    const void* ptr = ashmem->ReadFromAshmem(size, 0);
    if (!mapResult || ptr == nullptr) {
        return nullptr;
    }
    auto stream = std::make_unique<MemoryStream>(
        ptr, size, [](const void* ptr, void* context) { delete reinterpret_cast<Ashmem*>(context); }, ashmem.release());
    auto tf = Typeface::MakeFromStream(std::move(stream));
    if (tf == nullptr) {
        return nullptr;
    }
    tf->SetFd(fd);
    if (hash == 0) {
        hash = CalculateHash(reinterpret_cast<const uint8_t*>(ptr), size);
    }
    tf->SetHash(hash);
    tf->SetSize(size);
    return tf;
}

std::shared_ptr<Typeface> Typeface::MakeFromAshmem(
    const uint8_t* data, uint32_t size, uint32_t hash, const std::string& name)
{
    int32_t fd = OHOS::AshmemCreate(name.c_str(), size);
    auto ashmem = std::make_unique<Ashmem>(fd, size);
    bool mapResult = ashmem->MapReadAndWriteAshmem();
    bool writeResult = ashmem->WriteToAshmem(data, size, 0);
    const void* ptr = ashmem->ReadFromAshmem(size, 0);
    if (!mapResult || !writeResult || ptr == nullptr) {
        return nullptr;
    }
    auto stream = std::make_unique<MemoryStream>(
        ptr, size, [](const void* ptr, void* context) { delete reinterpret_cast<Ashmem*>(context); }, ashmem.release());
    auto tf = Typeface::MakeFromStream(std::move(stream));
    if (tf == nullptr) {
        return nullptr;
    }
    tf->SetFd(fd);
    if (hash == 0) {
        hash = CalculateHash(data, size);
    }
    tf->SetHash(hash);
    tf->SetSize(size);
    return tf;
}

std::shared_ptr<Typeface> Typeface::MakeFromName(const char familyName[], FontStyle fontStyle)
{
    return StaticFactory::MakeFromName(familyName, fontStyle);
}

std::string Typeface::GetFamilyName() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetFamilyName();
    }
    return std::string();
}

std::string Typeface::GetFontPath() const
{
    return (typefaceImpl_ == nullptr) ? "" : typefaceImpl_->GetFontPath();
}

FontStyle Typeface::GetFontStyle() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetFontStyle();
    }
    return FontStyle();
}

size_t Typeface::GetTableSize(uint32_t tag) const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetTableSize(tag);
    }
    return 0;
}

size_t Typeface::GetTableData(uint32_t tag, size_t offset, size_t length, void* data) const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetTableData(tag, offset, length, data);
    }
    return 0;
}

bool Typeface::GetItalic() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetItalic();
    }
    return false;
}

uint32_t Typeface::GetUniqueID() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetUniqueID();
    }
    return 0;
}

int32_t Typeface::GetUnitsPerEm() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetUnitsPerEm();
    }
    return 0;
}

std::shared_ptr<Typeface> Typeface::MakeClone(const FontArguments& args) const
{
    if (typefaceImpl_) {
        return typefaceImpl_->MakeClone(args);
    }
    return nullptr;
}

bool Typeface::IsCustomTypeface() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->IsCustomTypeface();
    }
    return false;
}

bool Typeface::IsThemeTypeface() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->IsThemeTypeface();
    }
    return false;
}

std::shared_ptr<Data> Typeface::Serialize() const
{
    if (!typefaceImpl_) {
        return nullptr;
    }
    return typefaceImpl_->Serialize();
}

std::shared_ptr<Typeface> Typeface::Deserialize(const void* data, size_t size)
{
    auto typeface = StaticFactory::DeserializeTypeface(data, size);
    if (typeface != nullptr) {
        typeface->SetSize(size);
    }
    return typeface;
}

TypefaceRegisterCallback Typeface::registerTypefaceCallBack_ = nullptr;
void Typeface::RegisterCallBackFunc(TypefaceRegisterCallback func)
{
    registerTypefaceCallBack_ = func;
}

TypefaceRegisterCallback& Typeface::GetTypefaceRegisterCallBack()
{
    return registerTypefaceCallBack_;
}

std::function<std::shared_ptr<Typeface>(uint64_t)> Typeface::uniqueIdCallBack_ = nullptr;
void Typeface::RegisterUniqueIdCallBack(std::function<std::shared_ptr<Typeface>(uint64_t)> cb)
{
    uniqueIdCallBack_ = cb;
}

std::function<std::shared_ptr<Typeface>(uint64_t)> Typeface::GetUniqueIdCallBack()
{
    return uniqueIdCallBack_;
}

uint32_t Typeface::GetHash() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetHash();
    }
    return 0;
}

void Typeface::SetHash(uint32_t hash)
{
    if (typefaceImpl_) {
        typefaceImpl_->SetHash(hash);
    }
}

uint32_t Typeface::GetSize()
{
    if (size_ != 0) {
        return size_;
    }
    if (!typefaceImpl_) {
        return 0;
    }
    auto data = typefaceImpl_->Serialize();
    if (!data) {
        return 0;
    }
    size_ = data->GetSize();
    return size_;
}

void Typeface::SetSize(uint32_t size)
{
    size_ = size;
}

void Typeface::SetFd(int32_t fd)
{
    if (typefaceImpl_) {
        typefaceImpl_->SetFd(fd);
    }
}

int32_t Typeface::GetFd() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetFd();
    }
    return -1;
}

void Typeface::UpdateStream(std::unique_ptr<MemoryStream> stream)
{
    if (typefaceImpl_) {
        typefaceImpl_->UpdateStream(std::move(stream));
    }
}

// Opentype font table constants
constexpr size_t MIN_HEADER_LEN = 6;            // first four bytes tell the type, two subsequent bytes toc size
constexpr size_t TABLE_COUNT = 4;               // Tables count is defined in fourth and fifth bytes
constexpr size_t STATIC_HEADER_LEN = 12;        // tables are listed after 12 bytes
constexpr size_t TABLE_ENTRY_LEN = 16;          // table entry contains 16 bytes of data
constexpr size_t COLLECTION_HEADER_LEN = 16;    // font collection header contains minimum 16 bytes of data
constexpr size_t NUM_FONTS_SHIFT = 8;           // eight bytes from header begin
constexpr size_t COLLECTION_TABLE_OFFSET = 12;  // tables are listed after 12 bytes
// bit shift, read data as bytes to avoid possible endianess problems
constexpr size_t ONE_BYTE_SHIFT = 8;            // one byte requires 8 bit shift

template<typename T>
T read(const uint8_t* data, size_t size = sizeof(T))
{
    T result = 0;
    for (auto ii = size; ii > 0; ii--) {
        result += (data[size - ii] << ((ii - 1) * ONE_BYTE_SHIFT));
    }
    return result;
}

uint32_t Typeface::CalculateHash(const uint8_t* data, size_t datalen)
{
    uint32_t hash = 0;
    size_t extraOffset = 0;
    if (datalen >= MIN_HEADER_LEN) {
        // if the font data points to font collection instead single font
        if (datalen > COLLECTION_HEADER_LEN && strncmp(reinterpret_cast<const char*>(data), "ttfc", 4) == 0) {
            if (read<uint32_t>(data + NUM_FONTS_SHIFT) > 0) {
                extraOffset = read<uint32_t>(data + COLLECTION_TABLE_OFFSET);
            } else {
                return hash;
            }
        }
        size_t size =
            extraOffset + STATIC_HEADER_LEN + TABLE_ENTRY_LEN * read<uint16_t>(data + extraOffset + TABLE_COUNT);
#ifdef USE_M133_SKIA
        hash ^= SkChecksum::Hash32(data, size, datalen);
#else
        hash ^= SkOpts::hash(data, size, datalen);
#endif
    }
    return hash;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
