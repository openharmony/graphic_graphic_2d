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
#include "rs_profiler.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_utils.h"
#include "rs_profiler_log.h"
#include "rs_profiler_pixelmap.h"

#include "transaction/rs_marshalling_helper.h"
#include "platform/common/rs_system_properties.h"

#ifdef ROSEN_OHOS
#include "lz4.h"
#endif

#include "image_packer.h"
#include "image_source.h"
#define ALPHA_OFFSET 4
#define SIZE_OF_PIXEL 4
#define RGB_OFFSET 3

namespace OHOS::Rosen {

ImageProperties::ImageProperties(PixelMap& map)
    : format(map.GetPixelFormat()), width(map.GetWidth()), height(map.GetHeight()),
      stride(map.GetRowStride())
{}

bool PixelMapStorage::IsSharedMemory(const PixelMap& map)
{
    return IsSharedMemory(const_cast<PixelMap&>(map).GetAllocatorType());
}

bool PixelMapStorage::IsSharedMemory(const PixelMemInfo& info)
{
    return IsSharedMemory(info.allocatorType);
}

bool PixelMapStorage::IsSharedMemory(AllocatorType type)
{
    return type == AllocatorType::SHARE_MEM_ALLOC;
}

bool PixelMapStorage::IsDmaMemory(const PixelMap& map)
{
    return IsDmaMemory(const_cast<PixelMap&>(map).GetAllocatorType());
}

bool PixelMapStorage::IsDmaMemory(const PixelMemInfo& info)
{
    return IsDmaMemory(info.allocatorType);
}

bool PixelMapStorage::IsDmaMemory(AllocatorType type)
{
    return type == AllocatorType::DMA_ALLOC;
}

bool PixelMapStorage::Fits(size_t size)
{
    constexpr size_t maxConsumption = 1024u * 1024u * 1024u;
    return (ImageCache::Consumption() + size) <= maxConsumption;
}

bool PixelMapStorage::Push(uint64_t id, PixelMap& map)
{
    if (!Fits(static_cast<size_t>(const_cast<PixelMap&>(map).GetCapacity()))) {
        return false;
    }

    if (IsDmaMemory(map)) {
        PushDmaMemory(id, map);
    } else if (IsSharedMemory(map)) {
        PushSharedMemory(id, map);
    }
    return true;
}

bool PixelMapStorage::Pull(uint64_t id, const ImageInfo& info, PixelMemInfo& memory, size_t& skipBytes)
{
    skipBytes = 0u;
    if (IsSharedMemory(memory)) {
        return PullSharedMemory(id, info, memory, skipBytes);
    } else if (IsDmaMemory(memory)) {
        return PullDmaMemory(id, info, memory, skipBytes);
    }
    return false;
}

bool PixelMapStorage::Push(uint64_t id, const ImageInfo& info, const PixelMemInfo& memory, size_t skipBytes)
{
    if (!Fits(static_cast<size_t>(memory.bufferSize))) {
        return false;
    }

    if (IsSharedMemory(memory)) {
        PushSharedMemory(id, info, memory, skipBytes);
    } else if (IsDmaMemory(memory)) {
        PushDmaMemory(id, info, memory, skipBytes);
    }
    return true;
}

bool PixelMapStorage::PullSharedMemory(uint64_t id, const ImageInfo& info, PixelMemInfo& memory, size_t& skipBytes)
{
    skipBytes = 0u;
    if (!ValidateBufferSize(memory)) {
        return false;
    }

    auto image = ImageCache::Get(id);
    if (!image) {
        return false;
    }

    memory.allocatorType = AllocatorType::HEAP_ALLOC;
    memory.base = reinterpret_cast<uint8_t*>(malloc(memory.bufferSize));
    if (!memory.base) {
        return false;
    }

    if (!CopyImageData(image, memory.base, memory.bufferSize)) {
        free(memory.base);
        memory.base = nullptr;
        return false;
    }

    memory.context = nullptr;
    skipBytes = image->parcelSkipBytes;
    return true;
}

void PixelMapStorage::PushSharedMemory(uint64_t id, const ImageInfo& info, const PixelMemInfo& memory, size_t skipBytes)
{
    PushImage(id, GenerateImageData(info, memory), skipBytes);
}

void PixelMapStorage::PushSharedMemory(uint64_t id, PixelMap& map)
{
    if (!map.GetFd()) {
        return;
    }

    constexpr size_t skipBytes = 24u;
    const auto size = static_cast<size_t>(const_cast<PixelMap&>(map).GetByteCount());
    const ImageProperties properties(map);
    if (auto image = MapImage(*reinterpret_cast<const int32_t*>(map.GetFd()), size, PROT_READ)) {
        PushImage(id, GenerateImageData(image, size, map), skipBytes, nullptr, &properties);
        UnmapImage(image, size);
    }
}

bool PixelMapStorage::PullDmaMemory(uint64_t id, const ImageInfo& info, PixelMemInfo& memory, size_t& skipBytes)
{
    skipBytes = 0u;
    if (!ValidateBufferSize(memory)) {
        return false;
    }

    auto image = ImageCache::Get(id);
    if (!image) {
        return false;
    }

    auto surfaceBuffer = SurfaceBuffer::Create();
    if (!surfaceBuffer) {
        return false;
    }

    const BufferRequestConfig config = { .width = image->dmaWidth,
        .height = image->dmaHeight,
        .strideAlignment = image->dmaStride,
        .format = image->dmaFormat,
        .usage = image->dmaUsage };
    surfaceBuffer->Alloc(config);

    memory.base = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    if (!CopyImageData(image, memory.base, image->dmaSize)) {
        return false;
    }

    memory.context = IncrementSurfaceBufferReference(surfaceBuffer);
    skipBytes = image->parcelSkipBytes;
    return true;
}

void PixelMapStorage::PushDmaMemory(uint64_t id, const ImageInfo& info, const PixelMemInfo& memory, size_t skipBytes)
{
    auto surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(memory.context);
    auto buffer = surfaceBuffer ? surfaceBuffer->GetBufferHandle() : nullptr;
    if (buffer) {
        const auto pixels = GenerateImageData(memory.base, buffer->size, memory.isAstc, GetBytesPerPixel(info));
        PushImage(id, pixels, skipBytes, buffer);
    }
}

void PixelMapStorage::PushDmaMemory(uint64_t id, PixelMap& map)
{
    const auto surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(map.GetFd());
    const auto buffer = surfaceBuffer ? surfaceBuffer->GetBufferHandle() : nullptr;
    if (!buffer) {
        return;
    }
    const ImageProperties properties(map);
    const auto pixels =
        GenerateImageData(reinterpret_cast<const uint8_t*>(surfaceBuffer->GetVirAddr()), buffer->size, map);
    MessageParcel parcel;
    surfaceBuffer->WriteToMessageParcel(parcel);
    PushImage(id, pixels, parcel.GetReadableBytes(), buffer, &properties);
}

int32_t PixelMapStorage::EncodeSeqLZ4(const ImageData& source, ImageData& dst)
{
#ifdef ROSEN_OHOS
    int32_t dstCap = LZ4_compressBound(source.size());
    ImageData buf;
    buf.reserve(dstCap);

    auto start = reinterpret_cast<const char*>(source.data());
    int32_t encodedSize = LZ4_compress_default(start, reinterpret_cast<char*>(buf.data()), source.size(), dstCap);
    if (encodedSize < 0) {
        HRPE("Error when encoding lz4");
        return -1;
    }

    dst.insert(dst.end(), buf.data(), buf.data() + encodedSize);
    return encodedSize;
#else
    HRPD("lz4 encoding is not supported on this platform");
    return -1;
#endif
}

int32_t PixelMapStorage::EncodeJpeg(const ImageData& source, ImageData& dst, const ImageProperties& properties)
{
    Media::InitializationOptions opts = { .size = { .width = properties.width, .height = properties.height },
        .srcPixelFormat = properties.format,
        .pixelFormat = Media::PixelFormat::RGBA_8888,
        .alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE,
        .srcRowStride = properties.stride };
    auto sourceRgb = Media::PixelMap::Create(reinterpret_cast<const uint32_t*>(source.data()), source.size(), opts);
    if (!sourceRgb) {
        HRPE("Failed to create source to encode to rgb");
        return -1;
    }
    uint32_t err = 0;
    Media::ImagePacker imagePacker;
    Media::PackOption option;
    option.format = "image/jpeg";
    ImageData dstRgb;
    dstRgb.reserve(source.size());
    err = imagePacker.StartPacking(static_cast<uint8_t*>(dstRgb.data()), source.size(), option);
    if (err != 0) {
        HRPE("Failed to start packing to jpeg, errcode %{public}u", err);
        return -1;
    }
    err = imagePacker.AddImage(*sourceRgb);
    if (err != 0) {
        HRPE("Failed to AddImage, errcode %{public}u", err);
        return -1;
    }
    int64_t encodedSize = 0;
    err = imagePacker.FinalizePacking(encodedSize);
    if (err != 0) {
        HRPE("Failed to FinalizePacking, errcode %{public}u", err);
        return -1;
    }
    dst.insert(dst.end(), dstRgb.data(), dstRgb.data() + encodedSize);
    return encodedSize;
}

void PixelMapStorage::ExtractAlpha(const ImageData& image, ImageData& alpha, const ImageProperties& properties)
{
    alpha.reserve(image.size() / ALPHA_OFFSET);
    for (int32_t row = 0, rStart = 0; row < properties.height; ++row, rStart += properties.stride) {
        for (int32_t pixIdx = 0, alphaIdx = rStart + RGB_OFFSET;
                pixIdx < properties.width; ++pixIdx, alphaIdx += ALPHA_OFFSET) {
            alpha.emplace_back(image[alphaIdx]);
        }
    }
}

void PixelMapStorage::PushImage(
    uint64_t id, const ImageData& data, size_t skipBytes, BufferHandle* buffer, const ImageProperties* properties)
{
    if (data.empty()) {
        return;
    }

    if (buffer && ((buffer->width == 0) || (buffer->height == 0))) {
        return;
    }

    Image image;
    image.parcelSkipBytes = skipBytes;
    if (buffer) {
        image.dmaSize = static_cast<size_t>(buffer->size);
        image.dmaWidth = buffer->width;
        image.dmaHeight = buffer->height;
        image.dmaStride = buffer->stride;
        image.dmaFormat = buffer->format;
        image.dmaUsage = buffer->usage;
    }

    EncodedType encodedType = EncodedType::NONE;
    if (RSProfiler::IsWriteEmulationMode() && (RSProfiler::GetTextureRecordType() == TextureRecordType::JPEG ||
        RSProfiler::GetTextureRecordType() == TextureRecordType::LZ4)) {
        // COMPRESS WITH LZ4 OR JPEG
        encodedType = TryEncodeTexture(properties, data, image);
    }
    if (encodedType == EncodedType::NONE) {
        // NO COMPRESION
        image.data = data;
    }

    ImageCache::Add(id, std::move(image));
}

EncodedType PixelMapStorage::TryEncodeTexture(const ImageProperties* properties, const ImageData& data, Image& image)
{
    EncodedType encodedType = EncodedType::NONE;

    image.data.resize(sizeof(TextureHeader));
    if (properties && RSProfiler::GetTextureRecordType() == TextureRecordType::JPEG &&
        (properties->format == Media::PixelFormat::RGBA_8888 || properties->format == Media::PixelFormat::BGRA_8888) &&
        static_cast<int32_t>(data.size()) == properties->stride * properties->height) {
        int32_t rgbEncodedSize = EncodeJpeg(data, image.data, *properties);
        if (rgbEncodedSize != -1) {
            ImageData alpha;
            ExtractAlpha(data, alpha, *properties);
            int32_t alphaEncodedSize = EncodeSeqLZ4(alpha, image.data);
            if (alphaEncodedSize != -1) {
                encodedType = EncodedType::JPEG;
                TextureHeader* header = reinterpret_cast<TextureHeader*>(image.data.data());
                header->magicNumber = 'JPEG';
                header->properties = *properties;
                header->totalOriginalSize = data.size();
                header->rgbEncodedSize = rgbEncodedSize;
                header->alphaOriginalSize = alpha.size();
                header->alphaEncodedSize = alphaEncodedSize;
            }
        }
    }
    if (encodedType == EncodedType::NONE) {
        int32_t encodedSize = EncodeSeqLZ4(data, image.data);
        if (encodedSize != -1) {
            encodedType = EncodedType::XLZ4;
            TextureHeader* header = reinterpret_cast<TextureHeader*>(image.data.data());
            header->magicNumber = 'XLZ4';
            header->totalOriginalSize = data.size();
        }
    }
    return encodedType;
}

bool PixelMapStorage::ValidateBufferSize(const PixelMemInfo& memory)
{
    return (memory.bufferSize > 0) && (static_cast<size_t>(memory.bufferSize) <= Image::maxSize);
}

uint32_t PixelMapStorage::GetBytesPerPixel(const ImageInfo& info)
{
    const auto rowPitch = PixelMap::GetRGBxRowDataSize(info);
    return (rowPitch > 0) ? static_cast<uint32_t>(rowPitch / info.size.width) : 0u;
}

uint8_t* PixelMapStorage::MapImage(int32_t file, size_t size, int32_t flags)
{
    auto image = ::mmap(nullptr, size, flags, MAP_SHARED, file, 0);
    return (image != MAP_FAILED) ? reinterpret_cast<uint8_t*>(image) : nullptr; // NOLINT
}

void PixelMapStorage::UnmapImage(void* image, size_t size)
{
    if (IsDataValid(image, size)) {
        ::munmap(image, size);
    }
}

SurfaceBuffer* PixelMapStorage::IncrementSurfaceBufferReference(sptr<SurfaceBuffer>& buffer)
{
    if (auto object = buffer.GetRefPtr()) {
        object->IncStrongRef(object);
        return object;
    }
    return nullptr;
}

bool PixelMapStorage::IsDataValid(const void* data, size_t size)
{
    return data && (size > 0);
}

bool PixelMapStorage::CopyImageData(const uint8_t* srcImage, size_t srcSize, uint8_t* dstImage, size_t dstSize)
{
    if (!srcImage || !dstImage || (srcSize == 0) || (dstSize == 0) || (srcSize > dstSize)) {
        return false;
    }

    if (dstSize == srcSize) {
        return Utils::Move(dstImage, dstSize, srcImage, srcSize);
    }

    for (size_t offset = 0; offset < dstSize;) {
        const size_t size = std::min(dstSize - offset, srcSize);
        if (!Utils::Move(dstImage + offset, size, srcImage, size)) {
            return false;
        }
        offset += size;
    }

    return true;
}

bool PixelMapStorage::CopyImageData(const ImageData& data, uint8_t* dstImage, size_t dstSize)
{
    return CopyImageData(data.data(), data.size(), dstImage, dstSize);
}

void PixelMapStorage::ReplaceAlpha(ImageData& image, ImageData& alpha, const ImageProperties& properties)
{
    int32_t i = 0;
    for (int32_t row = 0, rStart = 0; row < properties.height; ++row, rStart += properties.stride) {
        for (int32_t pixIdx = 0, alphaIdx = rStart + RGB_OFFSET; pixIdx < properties.width;
                ++pixIdx, alphaIdx += ALPHA_OFFSET) {
            image[alphaIdx] = alpha[i++];
        }
    }
}

int32_t PixelMapStorage::MakeStride(
    ImageData& noPadding, ImageData& dst, const ImageProperties& properties, int32_t pixelBytes)
{
    int32_t padding = properties.stride - properties.width * pixelBytes;
    for (int32_t row = 0; row < properties.height; row++) {
        int32_t rowStart = properties.width * pixelBytes * row;
        int32_t nextRowStart = properties.width * pixelBytes + rowStart;
        dst.insert(dst.end(), noPadding.begin() + rowStart, noPadding.begin() + nextRowStart);
        dst.insert(dst.end(), padding, 0);
    }

    return dst.size();
}

int32_t PixelMapStorage::DecodeSeqLZ4(const char* source, ImageData& dst, int32_t sourceSize, int32_t originalSize)
{
#ifdef ROSEN_OHOS
    dst.resize(originalSize);
    int cnt = LZ4_decompress_safe_partial(
        source, reinterpret_cast<char*>(dst.data()), sourceSize, originalSize, originalSize);
    return cnt;
#else
    HRPE("lz4 encoding is not supported on this platform");
    return -1;
#endif
}

int32_t PixelMapStorage::DecodeJpeg(
    const char* source, ImageData& dst, int32_t sourceSize, const ImageProperties& properties)
{
    Media::SourceOptions opts = { .formatHint = "image/jpeg",
        .pixelFormat = properties.format,
        .size = { .width = properties.width, .height = properties.height } };
    uint32_t err;
    auto src = Media::ImageSource::CreateImageSource(reinterpret_cast<const uint8_t*>(source), sourceSize, opts, err);
    if (!src || err != 0) {
        HRPE("Error when creating source, errcode %{public}u", err);
        return -1;
    }
    Media::DecodeOptions dopts;
    auto pmap = src->CreatePixelMap(dopts, err);
    if (!pmap || err != 0) {
        HRPE("Error when creating pixelmap, errcode %{public}u", err);
        return -1;
    }
    ImageData noPadding;
    noPadding.reserve(properties.width * properties.height * SIZE_OF_PIXEL);
    err = pmap->ReadPixels(pmap->GetByteCount(), noPadding.data());
    if (err != 0) {
        HRPE("Error when reading pixels, errcode %{public}u", err);
        return -1;
    }

    return MakeStride(noPadding, dst, properties, ALPHA_OFFSET);
}

bool PixelMapStorage::CopyImageData(Image* image, uint8_t* dstImage, size_t dstSize)
{
    if (!image) {
        return false;
    }

    TextureHeader* header = reinterpret_cast<TextureHeader*>(image->data.data());
    const char* srcStart = reinterpret_cast<const char*>(image->data.data()) + sizeof(TextureHeader);
    ImageData result;

    if (header->magicNumber == 'JPEG' && image->data.size() >= sizeof(TextureHeader)) {
        int32_t decodedTotalBytes = DecodeJpeg(srcStart, result, header->rgbEncodedSize, header->properties);
        if (decodedTotalBytes == header->totalOriginalSize) {
            ImageData alpha;
            const char* alphaStart = srcStart + header->rgbEncodedSize;
            int32_t decodedAlphaBytes =
                DecodeSeqLZ4(alphaStart, alpha, header->alphaEncodedSize, header->alphaOriginalSize);
            if (decodedAlphaBytes == header->alphaOriginalSize) {
                ReplaceAlpha(result, alpha, header->properties);
                image->data.clear();
                image->data.insert(image->data.end(), result.begin(), result.end());
            } else {
                HRPE("Error when decoding alpha got %{public}d bytes, expected %{public}d bytes", decodedAlphaBytes,
                    header->alphaOriginalSize);
            }
        } else {
            HRPE("Error when decoding rgb got %{public}d bytes, expected %{public}d bytes", decodedTotalBytes,
                header->totalOriginalSize);
        }
    } else if (header->magicNumber == 'XLZ4' && image->data.size() >= sizeof(TextureHeader)) {
        int32_t sourceSize = image->data.size() - sizeof(TextureHeader);
        int32_t decodedTotalBytes = DecodeSeqLZ4(srcStart, result, sourceSize, header->totalOriginalSize);
        if (decodedTotalBytes == header->totalOriginalSize) {
            image->data.clear();
            image->data.insert(image->data.end(), result.begin(), result.end());
        } else {
            HRPE("Error when decoding lz4 got %{public}d bytes, expected %{public}d bytes", decodedTotalBytes,
                header->totalOriginalSize);
        }
    } else {
        // assume image was not encoded, do nothing
    }

    return CopyImageData(image->data, dstImage, dstSize);
}

ImageData PixelMapStorage::GenerateRawCopy(const uint8_t* data, size_t size)
{
    ImageData out;
    if (IsDataValid(data, size)) {
        out.insert(out.end(), data, data + size);
    }
    return out;
}

ImageData PixelMapStorage::GenerateMiniatureAstc(const uint8_t* data, size_t size)
{
    constexpr uint32_t astcBytesPerPixel = 16u;
    return GenerateRawCopy(data, astcBytesPerPixel);
}

ImageData PixelMapStorage::GenerateMiniature(const uint8_t* data, size_t size, uint32_t pixelBytes)
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

    ImageData out(bytesPerPixel, 0);
    for (uint32_t i = 0; i < bytesPerPixel; i++) {
        out[i] = static_cast<uint8_t>(averageValue[i] / sampleCount);
    }
    return out;
}

ImageData PixelMapStorage::GenerateImageData(const uint8_t* data, size_t size, const PixelMap& map)
{
    const auto bytesPerPixel = static_cast<uint32_t>(const_cast<PixelMap&>(map).GetPixelBytes());
    return GenerateImageData(data, size, const_cast<PixelMap&>(map).IsAstc(), bytesPerPixel);
}

ImageData PixelMapStorage::GenerateImageData(const ImageInfo& info, const PixelMemInfo& memory)
{
    return GenerateImageData(
        memory.base, static_cast<size_t>(memory.bufferSize), memory.isAstc, GetBytesPerPixel(info));
}

ImageData PixelMapStorage::GenerateImageData(const uint8_t* data, size_t size, bool isAstc, uint32_t pixelBytes)
{
    if (RSProfiler::GetTextureRecordType() != TextureRecordType::ONE_PIXEL) {
        return GenerateRawCopy(data, size);
    }

    return isAstc ? GenerateMiniatureAstc(data, size) : GenerateMiniature(data, size, pixelBytes);
}

// Profiler

bool RSProfiler::SkipPixelMap(Parcel& parcel)
{
    if (IsEnabled() && IsWriteMode()) {
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

    const bool profilerEnabled = RSSystemProperties::GetProfilerEnabled();
    if (!parcel.WriteBool(profilerEnabled)) {
        HRPE("MarshalPixelMap: Unable to write profilerEnabled");
        return false;
    }

    if (!profilerEnabled) {
        return map->Marshalling(parcel);
    }

    const uint64_t id = ImageCache::New();
    if (!parcel.WriteUint64(id) || !map->Marshalling(parcel)) {
        HRPE("MarshalPixelMap: Unable to write id");
        return false;
    }

    if (IsSharedMemoryEnabled()) {
        return true;
    }

    if (!IsRecordAbortRequested() && (IsWriteMode() || IsWriteEmulationMode())) {
        if (!PixelMapStorage::Push(id, *map)) {
            RequestRecordAbort();
        }
    }
    return true;
}

Media::PixelMap* RSProfiler::UnmarshalPixelMap(Parcel& parcel,
    std::function<int(Parcel& parcel, std::function<int(Parcel&)> readFdDefaultFunc)> readSafeFdFunc)
{
    bool profilerEnabled = false;
    if (!parcel.ReadBool(profilerEnabled)) {
        HRPE("UnmarshalPixelMap: Unable to read profilerEnabled");
        return nullptr;
    }

    if (!profilerEnabled) {
        return PixelMap::Unmarshalling(parcel, readSafeFdFunc);
    }

    const uint64_t id = parcel.ReadUint64();

    if (IsRecordAbortRequested()) {
        return PixelMap::Unmarshalling(parcel, readSafeFdFunc);
    }

    ImageInfo info;
    PixelMemInfo memory;
    PIXEL_MAP_ERR error;
    auto map = PixelMap::StartUnmarshalling(parcel, info, memory, error);

    if (IsReadMode() || IsReadEmulationMode()) {
        size_t skipBytes = 0u;
        if (PixelMapStorage::Pull(id, info, memory, skipBytes)) {
            parcel.SkipBytes(skipBytes);
            return PixelMap::FinishUnmarshalling(map, parcel, info, memory, error);
        }
    }

    const auto parcelPosition = parcel.GetReadPosition();
    if (map && !PixelMap::ReadMemInfoFromParcel(parcel, memory, error, readSafeFdFunc)) {
        delete map;
        return nullptr;
    }

    if (IsWriteMode() || IsWriteEmulationMode()) {
        if (!PixelMapStorage::Push(id, info, memory, parcel.GetReadPosition() - parcelPosition)) {
            RequestRecordAbort();
        }
    }
    return PixelMap::FinishUnmarshalling(map, parcel, info, memory, error);
}

} // namespace OHOS::Rosen