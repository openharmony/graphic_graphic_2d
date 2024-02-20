/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "rs_profiler_pixelmap.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <securec.h>
#include <surface.h>
#include <sys/mman.h>

#include "ipc_file_descriptor.h"
#include "rs_profiler_base.h"

#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif
#include "image_type.h"
#include "parcel.h"
#ifdef IMAGE_PURGEABLE_PIXELMAP
#include "purgeable_mem_base.h"
#include "purgeable_mem_builder.h"
#endif

namespace OHOS::Media {
using TransColorProc = bool (*)(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
using CustomFreePixelMap = void (*)(void* addr, void* context, uint32_t size);

struct BuildParam;
struct TransInfos;

struct TransformData {
    float scaleX;
    float scaleY;
    float rotateD;
    float cropLeft;
    float cropTop;
    float cropWidth;
    float cropHeight;
    float translateX;
    float translateY;
    bool flipX;
    bool flipY;
};

struct PixelMapError {
    uint32_t errorCode = 0;
    std::string errorInfo;
};

class PixelMap : public Parcelable, public PixelMapError {
private:
    static constexpr size_t MAX_IMAGEDATA_SIZE = 128 * 1024 * 1024; // 128M
    static constexpr size_t MIN_IMAGEDATA_SIZE = 32 * 1024;         // 32k
    friend class ImageSource;

public:
    static std::atomic<uint32_t> currentId; // NOLINT
    PixelMap()
    {
        uniqueId_ = currentId.fetch_add(1, std::memory_order_relaxed);
    }
    ~PixelMap() override;

    NATIVEEXPORT virtual uint32_t SetImageInfo(ImageInfo& info);
    NATIVEEXPORT virtual uint32_t SetImageInfo(ImageInfo& info, bool isReused);
    NATIVEEXPORT virtual const uint8_t* GetPixel(int32_t x, int32_t y);
    NATIVEEXPORT virtual const uint8_t* GetPixel8(int32_t x, int32_t y);
    NATIVEEXPORT virtual const uint16_t* GetPixel16(int32_t x, int32_t y);
    NATIVEEXPORT virtual const uint32_t* GetPixel32(int32_t x, int32_t y);
    NATIVEEXPORT virtual bool GetARGB32Color(int32_t x, int32_t y, uint32_t& color);
    NATIVEEXPORT virtual void SetPixelsAddr(
        void* addr, void* context, uint32_t size, AllocatorType type, CustomFreePixelMap func);
    NATIVEEXPORT virtual int32_t GetPixelBytes();
    NATIVEEXPORT virtual int32_t GetRowBytes();
    NATIVEEXPORT virtual int32_t GetByteCount();
    NATIVEEXPORT virtual int32_t GetWidth();
    NATIVEEXPORT virtual int32_t GetHeight();
    NATIVEEXPORT void GetAstcRealSize(Size& size);
    NATIVEEXPORT void SetAstcRealSize(Size size);
    NATIVEEXPORT void GetTransformData(TransformData& transformData);
    NATIVEEXPORT void SetTransformData(TransformData transformData);
    NATIVEEXPORT virtual int32_t GetBaseDensity();
    NATIVEEXPORT virtual void scale(float xAxis, float yAxis);                                   // NOLINT
    NATIVEEXPORT virtual void scale(float xAxis, float yAxis, const AntiAliasingOption& option); // NOLINT
    NATIVEEXPORT virtual bool resize(float xAxis, float yAxis);                                  // NOLINT
    NATIVEEXPORT virtual void translate(float xAxis, float yAxis);                               // NOLINT
    NATIVEEXPORT virtual void rotate(float degrees);                                             // NOLINT
    NATIVEEXPORT virtual void flip(bool xAxis, bool yAxis);                                      // NOLINT
    NATIVEEXPORT virtual uint32_t crop(const Rect& rect);                                        // NOLINT
    NATIVEEXPORT virtual void GetImageInfo(ImageInfo& imageInfo);
    NATIVEEXPORT virtual PixelFormat GetPixelFormat();
    NATIVEEXPORT virtual ColorSpace GetColorSpace();
    NATIVEEXPORT virtual AlphaType GetAlphaType();
    NATIVEEXPORT virtual uint32_t SetAlpha(float);
    NATIVEEXPORT virtual const uint8_t* GetPixels();
    NATIVEEXPORT virtual uint8_t GetARGB32ColorA(uint32_t color);
    NATIVEEXPORT virtual uint8_t GetARGB32ColorR(uint32_t color);
    NATIVEEXPORT virtual uint8_t GetARGB32ColorG(uint32_t color);
    NATIVEEXPORT virtual uint8_t GetARGB32ColorB(uint32_t color);
    // Config the pixel map parameter
    NATIVEEXPORT virtual bool IsSameImage(const PixelMap& other);
    NATIVEEXPORT virtual uint32_t ReadPixels(
        const uint64_t& bufferSize, const uint32_t& offset, const uint32_t& stride, const Rect& region, uint8_t* dst);
    NATIVEEXPORT virtual uint32_t ReadPixels(const uint64_t& bufferSize, uint8_t* dst);
    NATIVEEXPORT virtual uint32_t ReadPixel(const Position& pos, uint32_t& dst);
    NATIVEEXPORT virtual uint32_t ResetConfig(const Size& size, const PixelFormat& format);
    NATIVEEXPORT virtual bool SetAlphaType(const AlphaType& alphaType);
    NATIVEEXPORT virtual uint32_t WritePixel(const Position& pos, const uint32_t& color);
    NATIVEEXPORT virtual uint32_t WritePixels(const uint8_t* source, const uint64_t& bufferSize, const uint32_t& offset,
        const uint32_t& stride, const Rect& region);
    NATIVEEXPORT virtual uint32_t WritePixels(const uint8_t* source, const uint64_t& bufferSize);
    NATIVEEXPORT virtual bool WritePixels(const uint32_t& color);
    NATIVEEXPORT virtual void FreePixelMap();
    NATIVEEXPORT bool IsStrideAlignment();
    NATIVEEXPORT virtual AllocatorType GetAllocatorType();
    NATIVEEXPORT virtual void* GetFd() const;
    NATIVEEXPORT virtual void SetFreePixelMapProc(CustomFreePixelMap func);
    NATIVEEXPORT virtual void SetTransformered(bool isTransformered);
    NATIVEEXPORT void SetPixelMapError(uint32_t code, std::string info);
    NATIVEEXPORT virtual void SetRowStride(uint32_t stride);
    NATIVEEXPORT virtual int32_t GetRowStride();
    NATIVEEXPORT virtual uint32_t GetCapacity();
    NATIVEEXPORT virtual bool IsEditable();
    NATIVEEXPORT virtual bool IsTransformered();
    NATIVEEXPORT virtual bool IsSourceAsResponse();
    NATIVEEXPORT virtual void* GetWritablePixels() const;
    NATIVEEXPORT virtual uint32_t GetUniqueId() const;
    NATIVEEXPORT bool Marshalling(Parcel& data) const override;
    NATIVEEXPORT virtual bool EncodeTlv(std::vector<uint8_t>& buff) const;

#ifdef IMAGE_COLORSPACE_FLAG
    NATIVEEXPORT void InnerSetColorSpace(const OHOS::ColorManager::ColorSpace& grColorSpace);
    NATIVEEXPORT OHOS::ColorManager::ColorSpace InnerGetGrColorSpace();
    NATIVEEXPORT std::shared_ptr<OHOS::ColorManager::ColorSpace> InnerGetGrColorSpacePtr();
    NATIVEEXPORT uint32_t ApplyColorSpace(const OHOS::ColorManager::ColorSpace& grColorSpace);
#endif

#ifdef IMAGE_PURGEABLE_PIXELMAP
    NATIVEEXPORT bool IsPurgeable() const;
    NATIVEEXPORT std::shared_ptr<PurgeableMem::PurgeableMemBase> GetPurgeableMemPtr() const;
    NATIVEEXPORT void SetPurgeableMemPtr(std::shared_ptr<PurgeableMem::PurgeableMemBase> pmPtr);
#endif

    NATIVEEXPORT bool IsAstc();
    NATIVEEXPORT void SetAstc(bool isAstc)
    {
        isAstc_ = isAstc;
    }

    bool GetPixelFormatDetail(PixelFormat format);
    bool CheckPixelsInput(const uint8_t* dst, const uint64_t& bufferSize, const uint32_t& offset,
        const uint32_t& stride, const Rect& region);
    void ReleaseSharedMemory(void* addr, void* context, uint32_t size);
    bool WriteInfoToParcel(Parcel& parcel) const;
    bool WriteTransformDataToParcel(Parcel& parcel) const;
    bool ReadTransformData(Parcel& parcel, PixelMap* pixelMap);
    bool WriteAstcRealSizeToParcel(Parcel& parcel) const;
    bool ReadAstcRealSize(Parcel& parcel, PixelMap* pixelMap);
    uint32_t SetRowDataSizeForImageInfo(ImageInfo info);
    void SetEditable(bool editable)
    {
        editable_ = editable;
    }
    void ResetPixelMap();
    bool CheckValidParam(int32_t x, int32_t y);
    bool WriteImageData(Parcel& parcel, size_t size) const;
    bool ReadImageInfo(Parcel& parcel, ImageInfo& imgInfo);
    bool WriteImageInfo(Parcel& parcel) const;
    void WriteUint8(std::vector<uint8_t>& buff, uint8_t value) const;
    uint8_t GetVarintLen(int32_t value) const;
    void WriteVarint(std::vector<uint8_t>& buff, int32_t value) const;
    void WriteData(std::vector<uint8_t>& buff, const uint8_t* data, const int32_t& height, const int32_t& rowDataSize,
        const int32_t& rowStride) const;
    bool DoTranslation(TransInfos& infos, const AntiAliasingOption& option = AntiAliasingOption::NONE);
    void UpdateImageInfo();

private:
    uint8_t* data_ = nullptr;
    // this info SHOULD be the final info for decoded pixelmap, not the original image info
    ImageInfo imageInfo_;
    int32_t rowDataSize_ = 0;
    int32_t rowStride_ = 0;
    int32_t pixelBytes_ = 0;
    TransColorProc colorProc_ = nullptr;
    void* context_ = nullptr;
    CustomFreePixelMap custFreePixelMap_ = nullptr;

public:
    CustomFreePixelMap freePixelMapProc_ = nullptr; // NOLINT

private:
    AllocatorType allocatorType_ = AllocatorType::SHARE_MEM_ALLOC;
    uint32_t pixelsSize_ = 0;
    bool editable_ = false;
    bool useSourceAsResponse_ = false;
    bool isTransformered_ = false;
    std::shared_ptr<std::mutex> transformMutex_ = std::make_shared<std::mutex>();

    // only used by rosen backend
    uint32_t uniqueId_ = 0;
    bool isAstc_ = false;
    TransformData transformData_ = { 1, 1, 0, 0, 0, 0, 0, 0, 0, false, false };
    Size astcrealSize_;

#ifdef IMAGE_COLORSPACE_FLAG
    std::shared_ptr<OHOS::ColorManager::ColorSpace> grColorSpace_ = nullptr;
#else
    std::shared_ptr<uint8_t> grColorSpace_ = nullptr;
#endif

#ifdef IMAGE_PURGEABLE_PIXELMAP
    std::shared_ptr<PurgeableMem::PurgeableMemBase> purgeableMemPtr_ = nullptr;
#else
    std::shared_ptr<uint8_t> purgeableMemPtr_ = nullptr;
#endif
};
} // namespace OHOS::Media

namespace OHOS::Rosen {

static int32_t ImageUtilsGetPixelBytes(const OHOS::Media::PixelFormat& pixelFormat)
{
    constexpr int32_t alphA8Bytes = 1;
    constexpr int32_t rgB565Bytes = 2;
    constexpr int32_t rgB888Bytes = 3;
    constexpr int32_t argB8888Bytes = 4;
    constexpr uint8_t rgbaF16Bytes = 8;
    constexpr int32_t astc4X4Bytes = 1;
    constexpr int32_t nV21Bytes = 2; // Each pixel is sorted on 3/2 bytes.

    int pixelBytes = 0;
    switch (pixelFormat) {
        case OHOS::Media::PixelFormat::ARGB_8888:
        case OHOS::Media::PixelFormat::BGRA_8888:
        case OHOS::Media::PixelFormat::RGBA_8888:
        case OHOS::Media::PixelFormat::CMYK:
            pixelBytes = argB8888Bytes;
            break;
        case OHOS::Media::PixelFormat::ALPHA_8:
            pixelBytes = alphA8Bytes;
            break;
        case OHOS::Media::PixelFormat::RGB_888:
            pixelBytes = rgB888Bytes;
            break;
        case OHOS::Media::PixelFormat::RGB_565:
            pixelBytes = rgB565Bytes;
            break;
        case OHOS::Media::PixelFormat::RGBA_F16:
            pixelBytes = rgbaF16Bytes;
            break;
        case OHOS::Media::PixelFormat::NV21:
        case OHOS::Media::PixelFormat::NV12:
            pixelBytes = nV21Bytes; // perl pixel 1.5 Bytes but return int so return 2
            break;
        case OHOS::Media::PixelFormat::ASTC_4x4:
        case OHOS::Media::PixelFormat::ASTC_6x6:
        case OHOS::Media::PixelFormat::ASTC_8x8:
            pixelBytes = astc4X4Bytes;
            break;
        default:
            break;
    }
    return pixelBytes;
}

static int32_t ImageUtilsSurfaceBufferReference(void* buffer)
{
    if (buffer == nullptr) {
        constexpr int32_t errCode = (30 << 21) | (1 << 16) | 79; // ERR_SURFACEBUFFER_REFERENCE_FAILED
        return errCode;
    }
    auto* ref = reinterpret_cast<OHOS::RefBase*>(buffer);
    ref->IncStrongRef(ref);
    return 0 /*SUCCESS*/;
}

static int32_t ImageUtilsSurfaceBufferUnreference(void* buffer)
{
    if (buffer == nullptr) {
        return -1; // ERR_SURFACEBUFFER_UNREFERENCE_FAILED;
    }
    auto* ref = reinterpret_cast<OHOS::RefBase*>(buffer);
    ref->DecStrongRef(ref);
    return 0;
}

static void ReleaseMemory(OHOS::Media::AllocatorType allocType, void* addr, void* context, uint32_t size)
{
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocType == OHOS::Media::AllocatorType::SHARE_MEM_ALLOC) {
        if (context != nullptr) {
            int* fd = static_cast<int*>(context);
            if (addr != nullptr) {
                ::munmap(addr, size);
            }
            if (fd != nullptr) {
                ::close(*fd);
            }
            context = nullptr;
            addr = nullptr;
        }
    } else if (allocType == OHOS::Media::AllocatorType::HEAP_ALLOC) {
        if (addr != nullptr) {
            free(addr);
            addr = nullptr;
        }
    } else if (allocType == OHOS::Media::AllocatorType::DMA_ALLOC) {
        if (context != nullptr) {
            ImageUtilsSurfaceBufferUnreference(context);
        }
        context = nullptr;
        addr = nullptr;
    }
#else
    if (addr != nullptr) {
        free(addr);
        addr = nullptr;
    }
#endif
}

static int ReadFileDescriptor(Parcel& parcel)
{
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    sptr<IPCFileDescriptor> descriptor = parcel.ReadObject<IPCFileDescriptor>();
    if (descriptor == nullptr) {
        return -1;
    }
    int fd = descriptor->GetFd();
    if (fd < 0) {
        return -1;
    }
    return dup(fd);
#else
    return -1;
#endif
}

static uint8_t* ReadImageDataFromParcel(Parcel& parcel, int32_t bufferSize)
{
    uint8_t* base = nullptr;
    if (bufferSize <= 0) {
        return nullptr;
    }

    const uint8_t* ptr = parcel.ReadUnpadBuffer(bufferSize);
    if (ptr == nullptr) {
        return nullptr;
    }

    base = static_cast<uint8_t*>(malloc(bufferSize));
    if (base == nullptr) {
        return nullptr;
    }
    if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
        free(base);
        base = nullptr;
        return nullptr;
    }
    return base;
}

static constexpr int32_t PIXEL_MAP_MAX_RAM_SIZE = 600 * 1024 * 1024;
static uint8_t* ReadImageDataFromSharedMem(Parcel& parcel, int32_t bufferSize)
{
    uint8_t* base = nullptr;
    int fd = ReadFileDescriptor(parcel);
    if (fd < 0) {
        return nullptr;
    }
    if (bufferSize <= 0 || bufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        return nullptr;
    }

    void* ptr = ::mmap(nullptr, bufferSize, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) { // NOLINT
        // do not close fd here. fd will be closed in FileDescriptor, ::close(fd)
        return nullptr;
    }

    base = static_cast<uint8_t*>(malloc(bufferSize));
    if (base == nullptr) {
        ::munmap(ptr, bufferSize);
        return nullptr;
    }
    if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
        ::munmap(ptr, bufferSize);
        free(base);
        base = nullptr;
        return nullptr;
    }

    ReleaseMemory(OHOS::Media::AllocatorType::SHARE_MEM_ALLOC, ptr, &fd, bufferSize);
    return base;
}

static constexpr size_t MIN_IMAGEDATA_SIZE = 32 * 1024; // 32k
static uint8_t* ReadImageData(Parcel& parcel, int32_t bufferSize)
{
    uint8_t* base = nullptr;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (static_cast<unsigned int>(bufferSize) <= MIN_IMAGEDATA_SIZE) {
        base = ReadImageDataFromParcel(parcel, bufferSize);
    } else {
        base = ReadImageDataFromSharedMem(parcel, bufferSize);
    }
#else
    if (bufferSize <= 0) {
        return nullptr;
    }

    const uint8_t* ptr = parcel.ReadUnpadBuffer(bufferSize);
    if (ptr == nullptr) {
        return nullptr;
    }

    base = static_cast<uint8_t*>(malloc(bufferSize));
    if (base == nullptr) {
        return nullptr;
    }
    if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
        free(base);
        base = nullptr;
        return nullptr;
    }
#endif
    return base;
}

static OHOS::Media::PixelMap* PixelMapUnmarshalling(uint64_t id, Parcel& parcel)
{
    auto* pixelMap = new OHOS::Media::PixelMap();
    if (pixelMap == nullptr) {
        return nullptr;
    }

    OHOS::Media::ImageInfo imgInfo;
    if (!pixelMap->ReadImageInfo(parcel, imgInfo)) {
        delete pixelMap;
        return nullptr;
    }

    bool isEditable = parcel.ReadBool();
    pixelMap->SetEditable(isEditable);

    bool isAstc = parcel.ReadBool();
    pixelMap->SetAstc(isAstc);

    auto allocType = static_cast<OHOS::Media::AllocatorType>(parcel.ReadInt32());
    int32_t csm = parcel.ReadInt32();
    if (csm != -1 /*OHOS::Media::ERR_MEDIA_INVALID_VALUE*/) {
    }
    int32_t rowDataSize = parcel.ReadInt32();
    int32_t bufferSize = parcel.ReadInt32();
    int32_t bytesPerPixel = ImageUtilsGetPixelBytes(imgInfo.pixelFormat);
    if (bytesPerPixel == 0) {
        delete pixelMap;
        return nullptr;
    }
    if ((!isAstc) && bufferSize != rowDataSize * imgInfo.size.height) {
        delete pixelMap;
        return nullptr;
    }
    uint8_t* base = nullptr;
    void* context = nullptr;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    constexpr int headerLength = 24;
    if (allocType == OHOS::Media::AllocatorType::SHARE_MEM_ALLOC) {
        int fd = ReadFileDescriptor(parcel);
        if (fd < 0) {
            ReplayImageCacheRecord* ptr = RSProfilerBase::ReplayImageGet(id);

            if (ptr != nullptr && static_cast<uint32_t>(bufferSize) == ptr->imageSize) {
                // get image from file
                base = static_cast<uint8_t*>(malloc(bufferSize));
                memmove(base, ptr->image.get(), bufferSize);
                context = nullptr;

                parcel.SkipBytes(headerLength);
            } else {
                delete pixelMap;
                return nullptr;
            }
        } else {
            void* ptr = ::mmap(nullptr, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED) {
                ptr = ::mmap(nullptr, bufferSize, PROT_READ, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED) {
                    ::close(fd);
                    delete pixelMap;
                    return nullptr;
                }
            }

            RSProfilerBase::ReplayImageAdd(id, ptr, bufferSize, headerLength);

            context = new int32_t();
            if (context == nullptr) {
                ::munmap(ptr, bufferSize);
                ::close(fd);
                delete pixelMap;
                return nullptr;
            }
            *static_cast<int32_t*>(context) = fd;
            base = static_cast<uint8_t*>(ptr);
        }
    } else if (allocType == OHOS::Media::AllocatorType::DMA_ALLOC) {
        ReplayImageCacheRecord* ptr = RSProfilerBase::ReplayImageGet(id);

        if (ptr == nullptr) {
            const size_t size1 = parcel.GetReadableBytes();

            // original code
            sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
            surfaceBuffer->ReadFromMessageParcel(static_cast<MessageParcel&>(parcel));
            auto* virAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
            void* nativeBuffer = surfaceBuffer.GetRefPtr();
            ImageUtilsSurfaceBufferReference(nativeBuffer);
            base = virAddr;
            context = nativeBuffer;

            const size_t size2 = parcel.GetReadableBytes();

            RSProfilerBase::ReplayImageAdd(id, virAddr, bufferSize, size1 - size2);

        } else {
            allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

            parcel.SkipBytes(ptr->skipBytes);

            base = static_cast<uint8_t*>(malloc(bufferSize));
            memmove(base, ptr->image.get(), bufferSize);
            context = nullptr;
        }

    } else {
        base = ReadImageData(parcel, bufferSize);
        if (base == nullptr) {
            delete pixelMap;
            return nullptr;
        }
    }
#else
    base = ReadImageData(parcel, bufferSize);
    if (base == nullptr) {
        delete pixelMap;
        return nullptr;
    }
#endif

    uint32_t ret = pixelMap->SetImageInfo(imgInfo);
    if (ret != 0 /*SUCCESS*/) {
        if (pixelMap->freePixelMapProc_ != nullptr) {
            pixelMap->freePixelMapProc_(base, context, bufferSize);
        }
        ReleaseMemory(allocType, base, context, bufferSize);
        if (allocType == OHOS::Media::AllocatorType::SHARE_MEM_ALLOC && context != nullptr) {
            delete static_cast<int32_t*>(context);
        }
        delete pixelMap;
        return nullptr;
    }
    pixelMap->SetPixelsAddr(base, context, bufferSize, allocType, nullptr);
    if (!pixelMap->ReadTransformData(parcel, pixelMap)) {
        delete pixelMap;
        return nullptr;
    }
    if (!pixelMap->ReadAstcRealSize(parcel, pixelMap)) {
        delete pixelMap;
        return nullptr;
    }
    return pixelMap;
}

OHOS::Media::PixelMap* RSProfilerPixelMap::Unmarshalling(Parcel& parcel)
{
    constexpr uint32_t bits32 = 32u;

    const uint32_t high = parcel.ReadInt32();
    const uint32_t low = parcel.ReadInt32();
    const uint64_t id = (static_cast<uint64_t>(high) << bits32) | low;

    return PixelMapUnmarshalling(id, parcel);
}

} // namespace OHOS::Rosen