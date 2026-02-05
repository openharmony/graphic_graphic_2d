/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef TYPEFACE_H
#define TYPEFACE_H

#include <functional>
#include <memory>
#include <cstdint>
#include <vector>

#include "impl_interface/typeface_impl.h"
#include "text/font_arguments.h"
#include "text/font_style.h"
#include "utils/data.h"
#include "utils/memory_stream.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct DRAWING_API MappedFile {
    const void* data = nullptr;
    size_t size = 0;
    int fd = -1;

    MappedFile() = default;
    explicit MappedFile(const std::string& path);
    ~MappedFile();
    MappedFile(const MappedFile&) = delete;
    MappedFile& operator=(const MappedFile&) = delete;
};

const uint8_t NO_REGISTER = 0;
const uint8_t REGISTERING = 1;
const uint8_t REGISTERED = 2;
using TypefaceRegisterCallback = std::function<int32_t(std::shared_ptr<Typeface>)>;
using GetByUniqueIdCallback = std::function<std::shared_ptr<Typeface>(uint64_t)>;
struct SharedTypeface;
class DRAWING_API Typeface {
public:
    explicit Typeface(std::shared_ptr<TypefaceImpl> typefaceImpl) noexcept;
    virtual ~Typeface() = default;

    static std::shared_ptr<Typeface> MakeDefault();
    static std::shared_ptr<Typeface> MakeFromFile(const char path[], int index = 0);
    static std::shared_ptr<Typeface> MakeFromFile(const char path[], const FontArguments& fontArguments);
    static std::shared_ptr<Typeface> MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index = 0);
    static std::shared_ptr<Typeface> MakeFromStream(std::unique_ptr<MemoryStream> memoryStream,
        const FontArguments& fontArguments);
    static std::shared_ptr<Typeface> MakeFromAshmem(int32_t fd, uint32_t size, uint32_t hash = 0, uint32_t index = 0);
    static std::shared_ptr<Typeface> MakeFromAshmem(
        int32_t fd, uint32_t size, uint32_t hash, const FontArguments& fontArguments);
    static std::shared_ptr<Typeface> MakeFromAshmem(SharedTypeface& sharedTypeface);
    static std::shared_ptr<Typeface> MakeFromAshmem(
        const uint8_t* data, uint32_t size, uint32_t hash, const std::string& name, uint32_t index = 0);
    static std::shared_ptr<Typeface> MakeFromAshmem(
        const uint8_t* data, uint32_t size, uint32_t hash, const std::string& name, const FontArguments& fontArguments);
    static std::shared_ptr<Typeface> MakeFromAshmem(std::unique_ptr<MemoryStream> memoryStream, uint32_t index = 0);
    static std::shared_ptr<Typeface> MakeFromAshmem(std::unique_ptr<MemoryStream> memoryStream,
        const FontArguments& fontArguments);

    static std::shared_ptr<Typeface> MakeFromName(const char familyName[], FontStyle fontStyle);
    static void RegisterCallBackFunc(TypefaceRegisterCallback func);
    static TypefaceRegisterCallback& GetTypefaceRegisterCallBack();
    static void RegisterOnTypefaceDestroyed(std::function<void(uint32_t)> cb);
    static void RegisterUniqueIdCallBack(std::function<std::shared_ptr<Typeface>(uint64_t)> cb);
    static std::function<std::shared_ptr<Typeface>(uint64_t)> GetUniqueIdCallBack();
    static std::vector<std::shared_ptr<Typeface>> GetSystemFonts();

    /**
     * @brief   Get the familyName for this typeface.
     * @return  FamilyName.
     */
    std::string GetFamilyName() const;

    std::string GetFontPath() const;
    int32_t GetFontIndex() const;

    /**
     * @brief   Get the fontStyle for this typeface.
     * @return  FontStyle.
     */
    FontStyle GetFontStyle() const;

    /**
     * @brief      Get the size of its contents for the given tag.
     * @param tag  The given table tag.
     * @return     If not present, return 0.
     */
    size_t GetTableSize(uint32_t tag) const;

    /**
     * @brief         Get the size of its contents for the given tag.
     * @param tag     The given table tag.
     * @param offset  The offset in bytes into the table's contents where the copy should start from.
     * @param length  The number of bytes.
     * @param data    Storage address.
     * @return        The number of bytes actually copied into data.
     */
    size_t GetTableData(uint32_t tag, size_t offset, size_t length, void* data) const;

    /**
     * @brief   Get fontStyle is bold.
     * @return  If fontStyle is bold, return true.
     */
    bool GetBold() const;

    /**
     * @brief   Get fontStyle is italic.
     * @return  If fontStyle is italic, return true.
     */
    bool GetItalic() const;

    /**
     * @brief   Get a 32bit value for this typeface, unique for the underlying font data.
     * @return  UniqueID.
     */
    uint32_t GetUniqueID() const;

    int32_t GetUnitsPerEm() const;

    std::shared_ptr<Typeface> MakeClone(const FontArguments&) const;

    bool IsCustomTypeface() const;
    bool IsThemeTypeface() const;

    std::shared_ptr<Data> Serialize() const;
    static std::shared_ptr<Typeface> Deserialize(const void* data, size_t size);

    template<typename T>
    T* GetImpl() const
    {
        if (typefaceImpl_) {
            return typefaceImpl_->DowncastingTo<T>();
        }
        return nullptr;
    }

    /**
     * @brief   Get a 32bit hash for this typeface, unique for the underlying font data.
     * @return  process independent hash
     */
    uint32_t GetHash() const;

    /**
     * @brief Set a 32bit hash for this typeface, unique for the underlying font data.
     */
    void SetHash(uint32_t hash);

    /**
     * @brief   Get serialized data size of typeface. Firstly set size before GetSize().
     * @return  serialized data size
     */
    uint32_t GetSize();

    /**
     * @brief Set serialized data size of typeface.
     */
    void SetSize(uint32_t size);

    /**
     * @brief   Get ashmem fd for this typeface.
     * @return  ashmem fd
     */
    int32_t GetFd() const;

    /**
     * @brief Set a ashmem fd for this typeface.
     */
    void SetFd(int32_t fd);

    /**
     * @brief   Calculate hash for this typeface.
     * @return  hash
     */
    static uint32_t CalculateHash(const uint8_t* data, size_t size, uint32_t index = 0);

    /**
     * @brief   Update stream for this typeface.
     */
    void UpdateStream(std::unique_ptr<MemoryStream> stream);

    int GetVariationDesignPosition(FontArguments::VariationPosition::Coordinate coordinates[],
        int coordinateCount) const;

    uint32_t GetIndex() const;

    static uint32_t CalculateFontArgsHash(const FontArguments::VariationPosition& coords);

    static uint64_t AssembleFullHash(uint32_t fontArgsHash, uint32_t baseHash);

    void SetFullHash(uint64_t fullHash);

    uint64_t GetFullHash() const;
private:
    std::shared_ptr<TypefaceImpl> typefaceImpl_;
    uint32_t size_ = 0;
    uint32_t index_ = 0;
    uint64_t fullHash_ = 0;
};

struct SharedTypeface {
    uint64_t id_ = 0;
    uint32_t size_ = 0;
    int32_t fd_ = -1;
    uint32_t hash_ = 0;
    uint32_t index_ = 0;
    bool hasFontArgs_ = false;
    std::vector<FontArguments::VariationPosition::Coordinate> coords_;
    SharedTypeface() {};
    SharedTypeface(uint64_t id, std::shared_ptr<Typeface>& typeface) : id_(id), size_(typeface->GetSize()),
        fd_(typeface->GetFd()), hash_(typeface->GetHash()), index_(typeface->GetIndex()) {
            int coordsCount = typeface->GetVariationDesignPosition(nullptr, 0);
            if (coordsCount > 0) {
                hasFontArgs_ = true;
                coords_.resize(coordsCount);
                typeface->GetVariationDesignPosition(coords_.data(), coordsCount);
            }
        }
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif