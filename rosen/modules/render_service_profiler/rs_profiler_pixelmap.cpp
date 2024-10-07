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

#include <message_parcel.h>
#include <securec.h>
#include <surface_buffer.h>
#include <sys/mman.h>

#include "media_errors.h"
#include "pixel_map.h"
#ifdef RS_PROFILER_SUPPORTS_PIXELMAP_YUV_EXT
#include "pixel_yuv_ext.h"
#else
#include "pixel_yuv.h"
#endif
#include "rs_profiler.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_log.h"
#include "rs_profiler_utils.h"
#include "rs_profiler_log.h"

#include "transaction/rs_marshalling_helper.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS::Media {

static SurfaceBuffer* IncrementSurfaceBufferReference(sptr<SurfaceBuffer>& buffer)
{
    if (auto object = buffer.GetRefPtr()) {
        auto ref = reinterpret_cast<OHOS::RefBase*>(object);
        ref->IncStrongRef(ref);
        return object;
    }
    return nullptr;
}

static bool IsDataValid(const void* data, size_t size)
{
    return data && (size > 0);
}

static std::vector<uint8_t> GenerateRawCopy(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> out;
    if (IsDataValid(data, size)) {
        out.insert(out.end(), data, data + size);
    }
    return out;
}

static std::vector<uint8_t> GenerateMiniatureAstc(const uint8_t* data, size_t size)
{
    constexpr uint32_t astcBytesPerPixel = 16u;
    return GenerateRawCopy(data, astcBytesPerPixel);
}

static std::vector<uint8_t> GenerateMiniature(const uint8_t* data, size_t size, uint32_t pixelBytes)
{
    if (!IsDataValid(data, size)) {
        return {};
    }

    constexpr uint32_t rgbaBytesPerPixel = 4u;
    constexpr uint32_t pixelBytesThreshold = 256u; // in case the pixelBytes field in the map has invalid value
    const uint32_t bytesPerPixel =
        ((pixelBytes > 0) && (pixelBytes < pixelBytesThreshold)) ? pixelBytes : rgbaBytesPerPixel;

    const auto pixelCount = size / bytesPerPixel;

    std::vector<uint64_t> averageValue(bytesPerPixel, 0);
    constexpr uint32_t sampleCount = 100u;
    for (uint32_t sample = 0; sample < sampleCount; sample++) {
        for (uint32_t channel = 0; channel < bytesPerPixel; channel++) {
            const size_t dataIdx = (sample * pixelCount / sampleCount) * bytesPerPixel + channel;
            averageValue[channel] += (dataIdx < size) ? data[dataIdx] : 0;
        }
    }

    std::vector<uint8_t> out(bytesPerPixel, 0);
    for (uint32_t i = 0; i < bytesPerPixel; i++) {
        out[i] = static_cast<uint8_t>(averageValue[i] / sampleCount);
    }
    return out;
}

static std::vector<uint8_t> GenerateImageData(const uint8_t* data, size_t size, bool isAstc, uint32_t pixelBytes)
{
    if (!Rosen::RSProfiler::IsBetaRecordEnabled()) {
        return GenerateRawCopy(data, size);
    }

    return isAstc ? GenerateMiniatureAstc(data, size) : GenerateMiniature(data, size, pixelBytes);
}

static std::vector<uint8_t> GenerateImageData(const uint8_t* data, size_t size, PixelMap& map)
{
    return GenerateImageData(data, size, map.IsAstc(), map.GetPixelBytes());
}

static bool CopyImageData(const uint8_t* srcImage, size_t srcSize, uint8_t* dstImage, size_t dstSize)
{
    if (!srcImage || !dstImage || (srcSize == 0) || (dstSize == 0) || (srcSize > dstSize)) {
        return false;
    }

    if (dstSize == srcSize) {
        return Rosen::Utils::Move(dstImage, dstSize, srcImage, srcSize);
    }

    for (size_t offset = 0; offset < dstSize;) {
        const size_t size = std::min(dstSize - offset, srcSize);
        if (!Rosen::Utils::Move(dstImage + offset, size, srcImage, size)) {
            return false;
        }
        offset += size;
    }

    return true;
}

static bool CopyImageData(const std::vector<uint8_t>& data, uint8_t* dstImage, size_t dstSize)
{
    return CopyImageData(data.data(), data.size(), dstImage, dstSize);
}

static bool CopyImageData(const Rosen::Image* image, uint8_t* dstImage, size_t dstSize)
{
    return image ? CopyImageData(image->data, dstImage, dstSize) : false;
}

struct UnmarshallingContext {
public:
    static constexpr int headerLength = 24; // NOLINT

public:
    explicit UnmarshallingContext(Parcel& parcel) : parcel(parcel) {}

    bool GatherImageFromFile(const Rosen::Image* image)
    {
        if ((size <= 0) || (size > Rosen::Image::maxSize)) {
            return false;
        }

        base = new uint8_t[size];
        if (!base) {
            return false;
        }

        if (!CopyImageData(image, base, size)) {
            delete[] base;
            base = nullptr;
            return false;
        }

        context = nullptr;
        return true;
    }

    bool GatherDmaImageFromFile(const Rosen::Image* image)
    {
        if ((size <= 0) || (size > Rosen::Image::maxSize)) {
            return false;
        }

        sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
        if (!surfaceBuffer) {
            return false;
        }

        const BufferRequestConfig config = { .width = image->dmaWidth,
            .height = image->dmaHeight,
            .strideAlignment = image->dmaStride,
            .format = image->dmaFormat,
            .usage = image->dmaUsage };
        surfaceBuffer->Alloc(config);

        base = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
        if (base && CopyImageData(image, base, image->dmaSize)) {
            context = IncrementSurfaceBufferReference(surfaceBuffer);
            return true;
        }
        return false;
    }

    bool IsAshmemSizeValid(int32_t file) const
    {
        return astc || (static_cast<int32_t>(size) == AshmemGetSize(file));
    }

    bool IsYUV() const
    {
        return (info.pixelFormat == PixelFormat::NV12) || (info.pixelFormat == PixelFormat::NV21) ||
               (info.pixelFormat == PixelFormat::YCBCR_P010) || (info.pixelFormat == PixelFormat::YCRCB_P010);
    }

    bool IsASTC() const
    {
        return (info.pixelFormat == PixelFormat::ASTC_4x4) || (info.pixelFormat == PixelFormat::ASTC_6x6) ||
               (info.pixelFormat == PixelFormat::ASTC_8x8);
    }

    bool IsRGBA() const
    {
        return (info.pixelFormat == PixelFormat::ARGB_8888) || (info.pixelFormat == PixelFormat::BGRA_8888) ||
               (info.pixelFormat == PixelFormat::RGBA_8888) || (info.pixelFormat == PixelFormat::RGBA_1010102) ||
               (info.pixelFormat == PixelFormat::CMYK) || (info.pixelFormat == PixelFormat::RGBA_F16) ||
               (info.pixelFormat == PixelFormat::RGBA_U16);
    }

    bool IsRGB() const
    {
        return (info.pixelFormat == PixelFormat::RGB_888) || (info.pixelFormat == PixelFormat::RGB_565);
    }

    bool IsR8() const
    {
        return (info.pixelFormat == PixelFormat::ALPHA_8);
    }

    bool IsFormatValid() const
    {
        return IsR8() || IsRGB() || IsRGBA() || IsASTC() || IsYUV();
    }

    bool IsSizeValid() const
    {
        const auto rawSize = static_cast<size_t>(rowPitch * info.size.height);
        return (astc || IsYUV() || (size == rawSize) || (info.pixelFormat == PixelFormat::RGBA_F16));
    }

public:
    Parcel& parcel;
    std::unique_ptr<PixelMap> map;
    ImageInfo info;
    bool editable = false;
    bool astc = false;
    int32_t csm = 0;
    uint32_t versionId = 0u;
    AllocatorType allocType = AllocatorType::DEFAULT;
    size_t rowPitch = 0;
    size_t size = 0;
    uint8_t* base = nullptr;
    void* context = nullptr;
};

// This class has to be 'reimplemented' here to get access to the PixelMap's private functions.
// It works ONLY thanks to the 'friend class ImageSource' in PixelMap.
class ImageSource {
public:
    static PixelMap* Unmarshal(Parcel& parcel);
    static bool Marshal(Parcel& parcel, PixelMap& map);

private:
    static void CacheImage(
        uint64_t id, const std::vector<uint8_t>& data, size_t skipBytes, BufferHandle* bufferHandle = nullptr);
    static Rosen::Image* GetCachedImage(uint64_t id);

    static uint8_t* MapImage(int32_t file, size_t size, int32_t flags);
    static void UnmapImage(void* image, size_t size);

    static void OnClientMarshalling(PixelMap& map, uint64_t id);

    static bool InitUnmarshalling(UnmarshallingContext& context);
    static bool UnmarshalFromSharedMemory(UnmarshallingContext& context, uint64_t id);
    static bool UnmarshalFromDMA(UnmarshallingContext& context, uint64_t id);
    static bool UnmarshalFromData(UnmarshallingContext& context);
    static PixelMap* FinalizeUnmarshalling(UnmarshallingContext& context);
};

void ImageSource::CacheImage(
    uint64_t id, const std::vector<uint8_t>& data, size_t skipBytes, BufferHandle* bufferHandle)
{
    if (data.empty()) {
        return;
    }

    if (bufferHandle && ((bufferHandle->width == 0) || (bufferHandle->height == 0))) {
        return;
    }

    Rosen::Image image;
    image.data = data;

    if (bufferHandle) {
        image.dmaSize = static_cast<size_t>(bufferHandle->size);
        image.dmaWidth = bufferHandle->width;
        image.dmaHeight = bufferHandle->height;
        image.dmaStride = bufferHandle->stride;
        image.dmaFormat = bufferHandle->format;
        image.dmaUsage = bufferHandle->usage;
    }

    image.parcelSkipBytes = skipBytes;
    Rosen::ImageCache::Add(id, std::move(image));
}

Rosen::Image* ImageSource::GetCachedImage(uint64_t id)
{
    return Rosen::ImageCache::Get(id);
}

uint8_t* ImageSource::MapImage(int32_t file, size_t size, int32_t flags)
{
    auto image = ::mmap(nullptr, size, flags, MAP_SHARED, file, 0);
    return (image != MAP_FAILED) ? reinterpret_cast<uint8_t*>(image) : nullptr; // NOLINT
}

void ImageSource::UnmapImage(void* image, size_t size)
{
    if (IsDataValid(image, size)) {
        ::munmap(image, size);
    }
}

bool ImageSource::InitUnmarshalling(UnmarshallingContext& context)
{
    if (!PixelMap::ReadImageInfo(context.parcel, context.info)) {
        HRPE("Unmarshal: PixelMap::ReadImageInfo failed");
        return false;
    }

    if (context.IsYUV()) {
#ifdef RS_PROFILER_SUPPORTS_PIXELMAP_YUV_EXT
        context.map = std::make_unique<PixelYuvExt>();
        HRPI("Unmarshal: PixelYuvExt creation in progress...");
#else
        context.map = std::make_unique<PixelYuv>();
        HRPI("Unmarshal: PixelYuv creation in progress...");
#endif
    } else {
        context.map = std::make_unique<PixelMap>();
        HRPI("Unmarshal: PixelMap creation in progress...");
    }

    if (!context.map) {
        HRPE("Unmarshal: Pixel map creation failed");
        return false;
    }

    context.editable = context.parcel.ReadBool();
    context.astc = context.parcel.ReadBool();
    context.allocType = static_cast<AllocatorType>(context.parcel.ReadInt32());
    context.csm = context.parcel.ReadInt32();
    context.rowPitch = static_cast<size_t>(context.parcel.ReadInt32());
    context.versionId = context.parcel.ReadUint32();
    context.size = static_cast<size_t>(context.parcel.ReadInt32());

    if (!context.IsFormatValid()) {
        HRPE("Unmarshal: Invalid pixel format");
        return false;
    }

    if (!context.IsSizeValid()) {
        HRPE("Unmarshal: Invalid size");
        return false;
    }

    context.map->SetEditable(context.editable);
    context.map->SetAstc(context.astc);
    context.map->SetVersionId(context.versionId);
    return true;
}

bool ImageSource::UnmarshalFromSharedMemory(UnmarshallingContext& context, uint64_t id)
{
    constexpr int32_t invalidFile = -1;
    const auto file =
        Rosen::RSProfiler::IsParcelMock(context.parcel) ? invalidFile : PixelMap::ReadFileDescriptor(context.parcel);
    if (file < 0) {
        if (auto image = GetCachedImage(id)) {
            if (context.GatherImageFromFile(image)) {
                context.parcel.SkipBytes(UnmarshallingContext::headerLength);
                return true;
            }
        }
        HRPE("Unmarshal: SharedMemory: Cached image fetch failed");
        return false;
    }

    if (!context.IsAshmemSizeValid(file)) {
        ::close(file);
        HRPE("Unmarshal: SharedMemory: Invalid file size");
        return false;
    }

    auto image = MapImage(file, context.size, PROT_READ | PROT_WRITE);
    if (!image) {
        image = MapImage(file, context.size, PROT_READ);
    }

    if (!image) {
        ::close(file);
        HRPE("Unmarshal: SharedMemory: Cannot map an image from a file");
        return false;
    }

    const auto imageData = GenerateImageData(image, context.size, *context.map);
    CacheImage(id, imageData, UnmarshallingContext::headerLength);

    context.context = new int32_t();
    if (!context.context) {
        UnmapImage(image, context.size);
        ::close(file);
        HRPE("Unmarshal: SharedMemory: Cannot allocate memory for a file handle");
        return false;
    }
    *static_cast<int32_t*>(context.context) = file;
    context.base = image;
    return true;
}

bool ImageSource::UnmarshalFromDMA(UnmarshallingContext& context, uint64_t id)
{
    auto image = Rosen::RSProfiler::IsParcelMock(context.parcel) ? GetCachedImage(id) : nullptr;
    if (image) {
        // REPLAY IMAGE
        context.parcel.SkipBytes(image->parcelSkipBytes);
        return context.GatherDmaImageFromFile(image);
    }

    const size_t readPosition = context.parcel.GetReadPosition();

    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->ReadFromMessageParcel(static_cast<MessageParcel&>(context.parcel));
    context.base = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    context.context = IncrementSurfaceBufferReference(surfaceBuffer);

    if (auto bufferHandle = surfaceBuffer->GetBufferHandle()) {
        // RECORD IMAGE
        const auto imageData = GenerateImageData(context.base, bufferHandle->size, *context.map);
        CacheImage(id, imageData, context.parcel.GetReadPosition() - readPosition, bufferHandle);
    }

    return true;
}

bool ImageSource::UnmarshalFromData(UnmarshallingContext& context)
{
    context.base = PixelMap::ReadImageData(context.parcel, static_cast<int32_t>(context.size));
    if (!context.base) {
        HRPE("Unmarshal: PixelMap::ReadImageData failed");
        return false;
    }
    return true;
}

PixelMap* ImageSource::FinalizeUnmarshalling(UnmarshallingContext& context)
{
    if (context.map->SetImageInfo(context.info) != SUCCESS) {
        if (context.map->freePixelMapProc_) {
            context.map->freePixelMapProc_(context.base, context.context, context.size);
        }
        PixelMap::ReleaseMemory(context.allocType, context.base, context.context, context.size);
        if (context.context && (context.allocType == AllocatorType::SHARE_MEM_ALLOC)) {
            delete static_cast<int32_t*>(context.context);
        }

        HRPE("Unmarshal: PixelMap::SetImageInfo failed");
        return nullptr;
    }

    context.map->SetPixelsAddr(context.base, context.context, context.size, context.allocType, nullptr);

    if (!context.map->ReadTransformData(context.parcel, context.map.get())) {
        HRPE("Unmarshal: PixelMap::ReadTransformData failed");
        return nullptr;
    }

    if (!context.map->ReadAstcRealSize(context.parcel, context.map.get())) {
        HRPE("Unmarshal: PixelMap::ReadAstcRealSize failed");
        return nullptr;
    }

    if (!context.map->ReadYuvDataInfoFromParcel(context.parcel, context.map.get())) {
        HRPE("Unmarshal: PixelMap::ReadYuvDataInfoFromParcel failed");
        return nullptr;
    }

    HRPI("Unmarshal: Done");
    return context.map.release();
}

PixelMap* ImageSource::Unmarshal(Parcel& parcel)
{
    const uint64_t id = parcel.ReadUint64();
    UnmarshallingContext context { parcel };

    if (!InitUnmarshalling(context)) {
        return nullptr;
    }

#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (context.allocType == AllocatorType::SHARE_MEM_ALLOC) {
        if (!UnmarshalFromSharedMemory(context, id)) {
            return nullptr;
        }
    } else if (context.allocType == AllocatorType::DMA_ALLOC) {
        if (!UnmarshalFromDMA(context, id)) {
            return nullptr;
        }
    } else {
        if (!UnmarshalFromData(context)) {
            return nullptr;
        }
    }
#else
    if (!UnmarshalFromData(context)) {
        return nullptr;
    }
#endif

    return FinalizeUnmarshalling(context);
}

void ImageSource::OnClientMarshalling(Media::PixelMap& map, uint64_t id)
{
    if (Rosen::RSProfiler::IsSharedMemoryEnabled()) {
        return;
    }

    const auto descriptor = map.GetFd();
    if (!descriptor) {
        return;
    }

    if (map.GetAllocatorType() == AllocatorType::DMA_ALLOC) {
        auto surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(descriptor);
        if (auto bufferHandle = surfaceBuffer->GetBufferHandle()) {
            const auto imageData = GenerateImageData(
                reinterpret_cast<const uint8_t*>(surfaceBuffer->GetVirAddr()), bufferHandle->size, map);
            MessageParcel parcel2;
            surfaceBuffer->WriteToMessageParcel(parcel2);
            size_t bufferHandleSize = parcel2.GetReadableBytes();
            CacheImage(id, imageData, bufferHandleSize, bufferHandle);
        }
    } else {
        const size_t size = map.isAstc_ ? map.pixelsSize_ :
            static_cast<size_t>(map.rowDataSize_ * map.imageInfo_.size.height);
        if (auto image = MapImage(*reinterpret_cast<const int32_t*>(map.GetFd()), size, PROT_READ)) {
            const auto imageData = GenerateImageData(image, size, map);
            CacheImage(id, imageData, UnmarshallingContext::headerLength);
            UnmapImage(image, size);
        }
    }
}

bool ImageSource::Marshal(Parcel& parcel, Media::PixelMap& map)
{
    const uint64_t id = Rosen::ImageCache::New();
    if (!parcel.WriteUint64(id) || !map.Marshalling(parcel)) {
        return false;
    }
    OnClientMarshalling(map, id);
    return true;
}

} // namespace OHOS::Media

namespace OHOS::Rosen {

using PixelMapHelper = Media::ImageSource;

Media::PixelMap* RSProfiler::UnmarshalPixelMap(Parcel& parcel)
{
    bool isClientEnabled = false;
    if (!parcel.ReadBool(isClientEnabled)) {
        HRPE("Unable to read is_client_enabled for image");
        return nullptr;
    }
    if (!isClientEnabled) {
        return Media::PixelMap::Unmarshalling(parcel);
    }

    return PixelMapHelper::Unmarshal(parcel);
}

bool RSProfiler::SkipPixelMap(Parcel& parcel)
{
    if (RSProfiler::IsEnabled() && RSProfiler::GetMode() == Mode::WRITE) {
        std::shared_ptr<Media::PixelMap> pixelMap;
        RSMarshallingHelper::Unmarshalling(parcel, pixelMap);
        return true;
    }
    return false;
}

bool RSProfiler::MarshalPixelMap(Parcel& parcel, const std::shared_ptr<Media::PixelMap>& map)
{
    if (!map) {
        return false;
    }

    bool isClientEnabled = RSSystemProperties::GetProfilerEnabled();
    if (!parcel.WriteBool(isClientEnabled)) {
        HRPE("Unable to write is_client_enabled for image");
        return false;
    }

    if (!isClientEnabled) {
        return map->Marshalling(parcel);
    }

    return PixelMapHelper::Marshal(parcel, *map);
}

} // namespace OHOS::Rosen