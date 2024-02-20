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

#ifndef INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_H_
#define INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_H_

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif
#include "image_type.h"
#include "parcel.h"
#ifdef IMAGE_PURGEABLE_PIXELMAP
#include "purgeable_mem_base.h"
#include "purgeable_mem_builder.h"
#endif

namespace OHOS {
namespace Media {
using TransColorProc = bool (*)(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
using CustomFreePixelMap = void (*)(void* addr, void* context, uint32_t size);

typedef struct {
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
} TransformData;

struct InitializationOptions {
    Size size;
    PixelFormat srcPixelFormat = PixelFormat::BGRA_8888;
    PixelFormat pixelFormat = PixelFormat::UNKNOWN;
    AlphaType alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    ScaleMode scaleMode = ScaleMode::FIT_TARGET_SIZE;
    bool editable = false;
    bool useSourceIfMatch = false;
};
struct TransInfos;

// Build ARGB_8888 pixel value
constexpr uint8_t ARGB_MASK = 0xFF;
constexpr uint8_t ARGB_A_SHIFT = 24;
constexpr uint8_t ARGB_R_SHIFT = 16;
constexpr uint8_t ARGB_G_SHIFT = 8;
constexpr uint8_t ARGB_B_SHIFT = 0;
// Define pixel map malloc max size 600MB
constexpr int32_t PIXEL_MAP_MAX_RAM_SIZE = 600 * 1024 * 1024;

typedef struct PixelMapError {
    uint32_t errorCode = 0;
    std::string errorInfo = "";
} PIXEL_MAP_ERR;

typedef struct BuildParam {
    int32_t offset_ = 0;
    int32_t stride_ = 0;
    bool flag_ = true;
} BUILD_PARAM;

class PixelMap : public Parcelable, public PIXEL_MAP_ERR {
public:
    static std::atomic<uint32_t> currentId;
    PixelMap()
    {
        uniqueId_ = currentId.fetch_add(1, std::memory_order_relaxed);
    }
    virtual ~PixelMap();
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(
        const uint32_t* colors, uint32_t colorLength, const InitializationOptions& opts);
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(const uint32_t* colors, uint32_t colorLength, int32_t offset,
        int32_t stride, const InitializationOptions& opts);
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(const uint32_t* colors, uint32_t colorLength, int32_t offset,
        int32_t stride, const InitializationOptions& opts, bool useCustomFormat);
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(const uint32_t* colors, uint32_t colorLength,
        BUILD_PARAM& info, const InitializationOptions& opts, int& errorCode);
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(const InitializationOptions& opts);
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(PixelMap& source, const InitializationOptions& opts);
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(
        PixelMap& source, const Rect& srcRect, const InitializationOptions& opts);
    NATIVEEXPORT static std::unique_ptr<PixelMap> Create(
        PixelMap& source, const Rect& srcRect, const InitializationOptions& opts, int32_t& errorCode);

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
    NATIVEEXPORT void GetAstcRealSize(Size& size)
    {
        size = astcrealSize_;
    }
    NATIVEEXPORT void SetAstcRealSize(Size size)
    {
        astcrealSize_ = size;
    }
    NATIVEEXPORT void GetTransformData(TransformData& transformData);
    NATIVEEXPORT void SetTransformData(TransformData transformData);
    NATIVEEXPORT virtual int32_t GetBaseDensity();
    NATIVEEXPORT virtual void scale(float xAxis, float yAxis);
    NATIVEEXPORT virtual void scale(float xAxis, float yAxis, const AntiAliasingOption& option);
    NATIVEEXPORT virtual bool resize(float xAxis, float yAxis);
    NATIVEEXPORT virtual void translate(float xAxis, float yAxis);
    NATIVEEXPORT virtual void rotate(float degrees);
    NATIVEEXPORT virtual void flip(bool xAxis, bool yAxis);
    NATIVEEXPORT virtual uint32_t crop(const Rect& rect);
    NATIVEEXPORT virtual void GetImageInfo(ImageInfo& imageInfo);
    NATIVEEXPORT virtual PixelFormat GetPixelFormat();
    NATIVEEXPORT virtual ColorSpace GetColorSpace();
    NATIVEEXPORT virtual AlphaType GetAlphaType();
    NATIVEEXPORT virtual uint32_t SetAlpha(const float percent);
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
    NATIVEEXPORT void SetPixelMapError(uint32_t code, std::string info)
    {
        errorCode = code;
        errorInfo = info;
    }

    NATIVEEXPORT static void ConstructPixelMapError(PIXEL_MAP_ERR& err, uint32_t code, std::string info)
    {
        err.errorCode = code;
        err.errorInfo = info;
    }

    NATIVEEXPORT virtual void SetRowStride(uint32_t stride);
    NATIVEEXPORT virtual int32_t GetRowStride()
    {
        return rowStride_;
    }
    NATIVEEXPORT virtual uint32_t GetCapacity()
    {
        return pixelsSize_;
    }

    NATIVEEXPORT virtual bool IsEditable()
    {
        return editable_;
    }

    NATIVEEXPORT virtual bool IsTransformered()
    {
        return isTransformered_;
    }

    // judgement whether create pixelmap use source as result
    NATIVEEXPORT virtual bool IsSourceAsResponse()
    {
        return useSourceAsResponse_;
    }

    NATIVEEXPORT virtual void* GetWritablePixels() const
    {
        return static_cast<void*>(data_);
    }

    NATIVEEXPORT virtual uint32_t GetUniqueId() const
    {
        return uniqueId_;
    }

    NATIVEEXPORT virtual bool Marshalling(Parcel& data) const override;
    NATIVEEXPORT static PixelMap* Unmarshalling(Parcel& data);
    NATIVEEXPORT static PixelMap* Unmarshalling(Parcel& parcel, PIXEL_MAP_ERR& error);
    NATIVEEXPORT virtual bool EncodeTlv(std::vector<uint8_t>& buff) const;
    NATIVEEXPORT static PixelMap* DecodeTlv(std::vector<uint8_t>& buff);

#ifdef IMAGE_COLORSPACE_FLAG
    // -------[inner api for ImageSource/ImagePacker codec] it will get a colorspace object pointer----begin----
    NATIVEEXPORT void InnerSetColorSpace(const OHOS::ColorManager::ColorSpace& grColorSpace);
    NATIVEEXPORT OHOS::ColorManager::ColorSpace InnerGetGrColorSpace();
    NATIVEEXPORT std::shared_ptr<OHOS::ColorManager::ColorSpace> InnerGetGrColorSpacePtr()
    {
        return grColorSpace_;
    }
    NATIVEEXPORT uint32_t ApplyColorSpace(const OHOS::ColorManager::ColorSpace& grColorSpace);
    // -------[inner api for ImageSource/ImagePacker codec] it will get a colorspace object pointer----end-------
#endif

#ifdef IMAGE_PURGEABLE_PIXELMAP
    NATIVEEXPORT bool IsPurgeable() const
    {
        return purgeableMemPtr_ != nullptr;
    }

    NATIVEEXPORT std::shared_ptr<PurgeableMem::PurgeableMemBase> GetPurgeableMemPtr() const
    {
        return purgeableMemPtr_;
    }

    NATIVEEXPORT void SetPurgeableMemPtr(std::shared_ptr<PurgeableMem::PurgeableMemBase> pmPtr)
    {
        purgeableMemPtr_ = pmPtr;
    }
#endif

    NATIVEEXPORT bool IsAstc()
    {
        return isAstc_;
    }

    NATIVEEXPORT void SetAstc(bool isAstc)
    {
        isAstc_ = isAstc;
    }

public:
    static constexpr uint8_t TLV_VARINT_BITS = 7;
    static constexpr uint8_t TLV_VARINT_MASK = 0x7F;
    static constexpr uint8_t TLV_VARINT_MORE = 0x80;
    static constexpr uint8_t TLV_END = 0x00;
    static constexpr uint8_t TLV_IMAGE_WIDTH = 0x01;
    static constexpr uint8_t TLV_IMAGE_HEIGHT = 0x02;
    static constexpr uint8_t TLV_IMAGE_PIXELFORMAT = 0x03;
    static constexpr uint8_t TLV_IMAGE_COLORSPACE = 0x04;
    static constexpr uint8_t TLV_IMAGE_ALPHATYPE = 0x05;
    static constexpr uint8_t TLV_IMAGE_BASEDENSITY = 0x06;
    static constexpr uint8_t TLV_IMAGE_ALLOCATORTYPE = 0x07;
    static constexpr uint8_t TLV_IMAGE_DATA = 0x08;
    static constexpr size_t MAX_IMAGEDATA_SIZE = 128 * 1024 * 1024; // 128M
    static constexpr size_t MIN_IMAGEDATA_SIZE = 32 * 1024;         // 32k
    friend class ImageSource;
    static bool ALPHA8ToARGB(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
    static bool RGB565ToARGB(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
    static bool ARGB8888ToARGB(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
    static bool RGBA8888ToARGB(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
    static bool BGRA8888ToARGB(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
    static bool RGB888ToARGB(const uint8_t* in, uint32_t inCount, uint32_t* out, uint32_t outCount);
    static bool CheckParams(const uint32_t* colors, uint32_t colorLength, int32_t offset, int32_t stride,
        const InitializationOptions& opts);
    static void UpdatePixelsAlpha(
        const AlphaType& alphaType, const PixelFormat& pixelFormat, uint8_t* dstPixels, PixelMap dstPixelMap);
    static void InitDstImageInfo(
        const InitializationOptions& opts, const ImageInfo& srcImageInfo, ImageInfo& dstImageInfo);
    static bool CopyPixMapToDst(PixelMap& source, void*& dstPixels, int& fd, uint32_t bufferSize);
    static bool CopyPixelMap(PixelMap& source, PixelMap& dstPixelMap, int32_t& error);
    static bool CopyPixelMap(PixelMap& source, PixelMap& dstPixelMap);
    static bool SourceCropAndConvert(PixelMap& source, const ImageInfo& srcImageInfo, const ImageInfo& dstImageInfo,
        const Rect& srcRect, PixelMap& dstPixelMap);
    static bool IsSameSize(const Size& src, const Size& dst);
    static bool ScalePixelMap(
        const Size& targetSize, const Size& dstSize, const ScaleMode& scaleMode, PixelMap& dstPixelMap);
    bool GetPixelFormatDetail(const PixelFormat format);
    bool CheckPixelsInput(const uint8_t* dst, const uint64_t& bufferSize, const uint32_t& offset,
        const uint32_t& stride, const Rect& region);
    void ReleaseSharedMemory(void* addr, void* context, uint32_t size);
    static void ReleaseBuffer(AllocatorType allocatorType, int fd, uint64_t dataSize, void** buffer);
    static void* AllocSharedMemory(const uint64_t bufferSize, int& fd, uint32_t uniqueId);
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

    void ResetPixelMap()
    {
        rowDataSize_ = 0;
        pixelBytes_ = 0;
        colorProc_ = nullptr;
    }

    bool CheckValidParam(int32_t x, int32_t y)
    {
        return (data_ == nullptr) || (x >= imageInfo_.size.width) || (x < 0) || (y >= imageInfo_.size.height) ||
                       (y < 0) || (pixelsSize_ < static_cast<uint64_t>(rowDataSize_) * imageInfo_.size.height)
                   ? false
                   : true;
    }

    static void ReleaseMemory(AllocatorType allocType, void* addr, void* context, uint32_t size);
    bool WriteImageData(Parcel& parcel, size_t size) const;
    static uint8_t* ReadImageData(Parcel& parcel, int32_t size);
    static int ReadFileDescriptor(Parcel& parcel);
    static bool WriteFileDescriptor(Parcel& parcel, int fd);
    bool ReadImageInfo(Parcel& parcel, ImageInfo& imgInfo);
    bool WriteImageInfo(Parcel& parcel) const;
    void WriteUint8(std::vector<uint8_t>& buff, uint8_t value) const;
    static uint8_t ReadUint8(std::vector<uint8_t>& buff, int32_t& cursor);
    uint8_t GetVarintLen(int32_t value) const;
    void WriteVarint(std::vector<uint8_t>& buff, int32_t value) const;
    static int32_t ReadVarint(std::vector<uint8_t>& buff, int32_t& cursor);
    void WriteData(std::vector<uint8_t>& buff, const uint8_t* data, const int32_t& height, const int32_t& rowDataSize,
        const int32_t& rowStride) const;
    static uint8_t* ReadData(std::vector<uint8_t>& buff, int32_t size, int32_t& cursor);
    static void ReadTlvAttr(std::vector<uint8_t>& buff, ImageInfo& info, int32_t& type, int32_t& size, uint8_t** data);
    bool DoTranslation(TransInfos& infos, const AntiAliasingOption& option = AntiAliasingOption::NONE);
    void UpdateImageInfo();

    uint8_t* data_ = nullptr;
    // this info SHOULD be the final info for decoded pixelmap, not the original image info
    ImageInfo imageInfo_;
    int32_t rowDataSize_ = 0;
    int32_t rowStride_ = 0;
    int32_t pixelBytes_ = 0;
    TransColorProc colorProc_ = nullptr;
    void* context_ = nullptr;
    CustomFreePixelMap custFreePixelMap_ = nullptr;
    CustomFreePixelMap freePixelMapProc_ = nullptr;
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
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_H_
