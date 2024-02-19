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

#include "rs_profiler_base.h"
#include "rs_profiler_pixelmap.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <utility>
#include <vector>

#include "ipc_file_descriptor.h"
#include "message_parcel.h"

#include "command/rs_base_node_command.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

static SpecParseMode g_mode;
static std::vector<pid_t> g_pidList;
static pid_t g_pidValue = 0;
static NodeId g_parentNode = 0;
static std::map<uint64_t, ReplayImageCacheRecord> g_imageMap;
static std::atomic<uint32_t> g_parsedCmdCount = 0;
static std::atomic<uint32_t> g_lastImagemapCount = 0;

static uint64_t g_pauseAfterTime = 0;
static uint64_t g_pauseCumulativeTime = 0;

constexpr uint32_t bits32 = 32;
constexpr uint32_t bits30 = 30;

constexpr ::OHOS::HiviewDFX::HiLogLabel label = { LOG_CORE, 0xD001400, "FrameCollector" };

std::map<uint64_t, ReplayImageCacheRecord>& RSProfilerBase::ImageMapGet()
{
    return g_imageMap;
}

uint32_t RSProfilerBase::ImagesAddedCountGet()
{
    const uint32_t retValue = g_imageMap.size() - g_lastImagemapCount;
    g_lastImagemapCount = g_imageMap.size();
    return retValue;
}

uint32_t RSProfilerBase::ParsedCmdCountGet()
{
    const uint32_t retValue = g_parsedCmdCount;
    g_parsedCmdCount = 0;
    return retValue;
}

void RSProfilerBase::ParsedCmdCountAdd(uint32_t addon)
{
    g_parsedCmdCount += addon;
}

bool RSProfilerBase::IsParcelMock(const Parcel& parcel)
{
    const volatile auto intPtr =
        reinterpret_cast<intptr_t>(&parcel); // gcc C++ optimization error, not working without volatile
    return ((intPtr & 1) != 0);
}

std::shared_ptr<MessageParcel> RSProfilerBase::CopyParcel(const MessageParcel& parcel)
{
    if (IsParcelMock(parcel)) {
        auto* buffer = new uint8_t[sizeof(MessageParcel) + 1];
        auto* mpPtr = new (buffer + 1) MessageParcel;
        return std::shared_ptr<MessageParcel>(mpPtr, [](MessageParcel* ptr) {
            ptr->~MessageParcel();
            auto* allocPtr = reinterpret_cast<uint8_t*>(ptr);
            allocPtr--;
            delete allocPtr;
        });
    }

    return std::make_shared<MessageParcel>();
}

uint64_t RSProfilerBase::ReadTime(Parcel& parcel)
{
    return parcel.ReadInt64();
}

NodeId RSProfilerBase::ReadNodeId(Parcel& parcel)
{
    return PatchPlainNodeId(parcel, static_cast<NodeId>(parcel.ReadUint64()));
}

pid_t RSProfilerBase::ReadProcessId(Parcel& parcel)
{
    return PatchPlainProcessId(parcel, static_cast<pid_t>(parcel.ReadUint32()));
}

NodeId RSProfilerBase::PatchPlainNodeId(const Parcel& parcel, NodeId id)
{
    if ((g_mode != SpecParseMode::READ) || !IsParcelMock(parcel)) {
        return id;
    }

    return id | (static_cast<uint64_t>(1) << (bits30 + bits32));
}

pid_t RSProfilerBase::PatchPlainProcessId(const Parcel& parcel, pid_t pid)
{
    if ((g_mode != SpecParseMode::READ) || !IsParcelMock(parcel)) {
        return pid;
    }

    return pid | (1 << bits30);
}

void RSProfilerBase::SpecParseModeSet(SpecParseMode mode)
{
    g_mode = mode;
    if (g_mode == SpecParseMode::NONE) {
        g_pauseAfterTime = 0;
        g_pauseCumulativeTime = 0;
    }
}

SpecParseMode RSProfilerBase::SpecParseModeGet()
{
    return g_mode;
}

void RSProfilerBase::SpecParseReplacePIDSet(
    const std::vector<pid_t>& replacePidList, pid_t replacePidValue, NodeId parentNode)
{
    g_pidList = replacePidList;
    g_pidValue = replacePidValue;
    g_parentNode = parentNode;
}

NodeId RSProfilerBase::SpecParseParentNodeGet()
{
    return g_parentNode;
}

const std::vector<pid_t>& RSProfilerBase::SpecParsePidListGet()
{
    return g_pidList;
}

pid_t RSProfilerBase::SpecParsePidReplaceGet()
{
    return g_pidValue;
}

void RSProfilerBase::ReplayImageAdd(uint64_t uniqueId, void* image, uint32_t imageSize, uint32_t skipBytes)
{
    if (g_mode != SpecParseMode::WRITE) {
        return;
    }

    if (g_imageMap.count(uniqueId) > 0) {
        return;
    }

    const std::shared_ptr<void> imageSaveData(new uint8_t[imageSize]);
    memmove(imageSaveData.get(), image, imageSize);

    ReplayImageCacheRecord record;
    record.image = imageSaveData;
    record.imageSize = imageSize;
    record.skipBytes = skipBytes;

    g_imageMap.insert({ uniqueId, record });
}

ReplayImageCacheRecord* RSProfilerBase::ReplayImageGet(uint64_t uniqueId)
{
    if (g_mode != SpecParseMode::READ) {
        return nullptr;
    }

    if (g_imageMap.count(uniqueId) == 0) {
        return nullptr;
    }

    return &g_imageMap[uniqueId];
}

std::string RSProfilerBase::ReplayImagePrintList()
{
    if (g_mode != SpecParseMode::READ) {
        return "";
    }

    std::string out;
    constexpr uint64_t mask32Bits = 0xFFffFFff;
    for (const auto& it : g_imageMap) {
        out += std::to_string(it.first >> bits32) + ":" + std::to_string(it.first & mask32Bits) + " ";
    }

    return out;
}

uint64_t RSProfilerBase::TimePauseApply(uint64_t curTime)
{
    if (g_mode != SpecParseMode::READ) {
        return curTime;
    }

    if (curTime >= g_pauseAfterTime && g_pauseAfterTime > 0) {
        return g_pauseAfterTime - g_pauseCumulativeTime;
    }
    return curTime - g_pauseCumulativeTime;
}

void RSProfilerBase::TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
        }
    }
    g_pauseAfterTime = newPauseAfterTime;
}

void RSProfilerBase::TimePauseResume(uint64_t curTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
        }
    }
    g_pauseAfterTime = 0;
}

void RSProfilerBase::TimePauseClear()
{
    g_pauseCumulativeTime = 0;
    g_pauseAfterTime = 0;
}

static int32_t ImageUtilsGetPixelBytes(const OHOS::Media::PixelFormat &pixelFormat)
{
    constexpr int32_t alphA8Bytes = 1;
    constexpr int32_t rgB565Bytes = 2;
    constexpr int32_t rgB888Bytes = 3;
    constexpr int32_t argB8888Bytes = 4;
    constexpr uint8_t rgbaF16Bytes = 8;
    constexpr int32_t astc4X4Bytes = 1;
    constexpr int32_t nV21Bytes = 2;  // Each pixel is sorted on 3/2 bytes.

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
            pixelBytes = nV21Bytes;  // perl pixel 1.5 Bytes but return int so return 2
            break;
        case OHOS::Media::PixelFormat::ASTC_4x4:
        case OHOS::Media::PixelFormat::ASTC_6x6:
        case OHOS::Media::PixelFormat::ASTC_8x8:
            pixelBytes = astc4X4Bytes;
            break;
        default:
            HiviewDFX::HiLog::Error(label, "[ImageUtil]get pixel bytes failed, pixelFormat:%{public}d.",
                static_cast<int32_t>(pixelFormat));
            break;
    }
    return pixelBytes;
}

static int32_t ImageUtilsSurfaceBufferReference(void* buffer)
{
    if (buffer == nullptr) {
        HiviewDFX::HiLog::Error(label, "parameter error, please check input parameter");
        constexpr int32_t errCode = (30 << 21) | (1 << 16) | 79; // ERR_SURFACEBUFFER_REFERENCE_FAILED
        return errCode;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(buffer);
    ref->IncStrongRef(ref);
    return 0/*SUCCESS*/;
}

static int32_t ImageUtilsSurfaceBufferUnreference(void* buffer)
{
    if (buffer == nullptr) {
        HiviewDFX::HiLog::Error(label, "parameter error, please check input parameter");
        return -1; // ERR_SURFACEBUFFER_UNREFERENCE_FAILED;
    }
    auto* ref = reinterpret_cast<OHOS::RefBase*>(buffer);
    ref->DecStrongRef(ref);
    return 0;
}

static void ReleaseMemory(OHOS::Media::AllocatorType allocType, void* addr, void* context, uint32_t size)
{
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    if (allocType == OHOS::Media::AllocatorType::SHARE_MEM_ALLOC) {
        if (context != nullptr) {
            int *fd = static_cast<int *>(context);
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
            ImageUtilsSurfaceBufferUnreference(static_cast<SurfaceBuffer*>(context));
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

static int ReadFileDescriptor(Parcel &parcel)
{
#if !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
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

static constexpr size_t minImagedataSize = 32 * 1024;         // 32k
static constexpr int32_t pixelMapMaxRamSize = 600 * 1024 * 1024;

static uint8_t* ReadImageDataFromParcel(Parcel& parcel, int32_t bufferSize)
{
    uint8_t* base = nullptr;
    if (bufferSize <= 0) {
        HiviewDFX::HiLog::Error(label, "malloc parameter bufferSize:[%{public}d] error.", bufferSize);
        return nullptr;
    }

    const uint8_t* ptr = parcel.ReadUnpadBuffer(bufferSize);
    if (ptr == nullptr) {
        HiviewDFX::HiLog::Error(label, "read buffer from parcel failed, read buffer addr is null");
        return nullptr;
    }

    base = static_cast<uint8_t*>(malloc(bufferSize));
    if (base == nullptr) {
        HiviewDFX::HiLog::Error(label, "alloc output pixel memory size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
    if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
        free(base);
        base = nullptr;
        HiviewDFX::HiLog::Error(label, "memcpy pixel data size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
    return base;
}

static uint8_t* ReadImageDataFromSharedMem(Parcel& parcel, int32_t bufferSize)
{
    uint8_t* base = nullptr;
    int fd = ReadFileDescriptor(parcel);
    if (fd < 0) {
        HiviewDFX::HiLog::Error(label, "read fd :[%{public}d] error", fd);
        return nullptr;
    }
    if (bufferSize <= 0 || bufferSize > pixelMapMaxRamSize) {
        HiviewDFX::HiLog::Error(label, "malloc parameter bufferSize:[%{public}d] error.", bufferSize);
        return nullptr;
    }

    void* ptr = ::mmap(nullptr, bufferSize, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        // do not close fd here. fd will be closed in FileDescriptor, ::close(fd)
        HiviewDFX::HiLog::Error(label, "ReadImageData map failed, errno:%{public}d", errno);
        return nullptr;
    }

    base = static_cast<uint8_t*>(malloc(bufferSize));
    if (base == nullptr) {
        ::munmap(ptr, bufferSize);
        HiviewDFX::HiLog::Error(label, "alloc output pixel memory size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
    if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
        ::munmap(ptr, bufferSize);
        free(base);
        base = nullptr;
        HiviewDFX::HiLog::Error(label, "memcpy pixel data size:[%{public}d] error.", bufferSize);
        return nullptr;
    }

    ReleaseMemory(OHOS::Media::AllocatorType::SHARE_MEM_ALLOC, ptr, &fd, bufferSize);
    return base;
}

static uint8_t* ReadImageData(Parcel& parcel, int32_t bufferSize)
{
    uint8_t *base = nullptr;
#if !defined(_WIN32) && !defined(_APPLE) &&!defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    if (static_cast<unsigned int>(bufferSize) <= minImagedataSize) {
        base = ReadImageDataFromParcel(parcel, bufferSize);
    } else {
        base = ReadImageDataFromSharedMem(parcel, bufferSize);
    }
#else
    if (bufferSize <= 0) {
        HiviewDFX::HiLog::Error(LABEL, "malloc parameter bufferSize:[%{public}d] error.", bufferSize);
        return nullptr;
    }

    const uint8_t *ptr = parcel.ReadUnpadBuffer(bufferSize);
    if (ptr == nullptr) {
        HiviewDFX::HiLog::Error(LABEL, "read buffer from parcel failed, read buffer addr is null");
        return nullptr;
    }

    base = static_cast<uint8_t *>(malloc(bufferSize));
    if (base == nullptr) {
        HiviewDFX::HiLog::Error(LABEL, "alloc output pixel memory size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
    if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
        free(base);
        base = nullptr;
        HiviewDFX::HiLog::Error(LABEL, "memcpy pixel data size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
#endif
    return base;
}

uint32_t RSProfilerBase::GenerateUniqueImageId()
{
    static uint32_t uniqueImageId = 0u;
    return uniqueImageId++;
}

OHOS::Media::PixelMap* RSProfilerBase::PixelMapUnmarshalling(Parcel& parcel)
{
    [[maybe_unused]] const uint32_t replayUniqueIdHi = parcel.ReadInt32();
    [[maybe_unused]] const uint32_t replayUniqueIdLo = parcel.ReadInt32();
    [[maybe_unused]] const uint64_t replayUniqueId = (static_cast<uint64_t>(replayUniqueIdHi) << bits32) | replayUniqueIdLo;

    //return Media::PixelMap::Unmarshalling(parcel);
    return RSProfilerBase::PixelMapUnmarshallingLo(replayUniqueId, parcel);
}

OHOS::Media::PixelMap *RSProfilerBase::PixelMapUnmarshallingLo(uint64_t replayUniqueId, Parcel &parcel)
{
    OHOS::Media::PIXEL_MAP_ERR error;
    OHOS::Media::PixelMap* dstPixelMap = PixelMapUnmarshallingLoWithErrCode(replayUniqueId, parcel, error);
    if (dstPixelMap == nullptr || error.errorCode != 0/*OHOS::Media::SUCCESS*/) {
        HiviewDFX::HiLog::Error(label, "unmarshalling failed errorCode:%{public}d, errorInfo:%{public}s",
            error.errorCode, error.errorInfo.c_str());
    }
    return dstPixelMap;
}

OHOS::Media::PixelMap* RSProfilerBase::PixelMapUnmarshallingLoWithErrCode(
    uint64_t replayUniqueId, Parcel& parcel, OHOS::Media::PixelMapError& error)
{
    OHOS::Media::PixelMap *pixelMap = new OHOS::Media::PixelMap();
    if (pixelMap == nullptr) {
        constexpr uint32_t errCode = (30 << 21) | (1 << 16) | 82; // OHOS::Media::ERR_IMAGE_PIXELMAP_CREATE_FAILED
        OHOS::Media::PixelMap::ConstructPixelMapError(error, errCode, "pixelmap create failed");
        return nullptr;
    }

    OHOS::Media::ImageInfo imgInfo;
    if (!pixelMap->ReadImageInfo(parcel, imgInfo)) {
        HiviewDFX::HiLog::Error(label, "read imageInfo fail");
        delete pixelMap;
        constexpr uint32_t errCode = (30 << 21) | (1 << 16) | 150; // OHOS::Media::ERR_IMAGE_READ_PIXELMAP_FAILED
        OHOS::Media::PixelMap::ConstructPixelMapError(error, errCode, "read pixelmap failed");
        return nullptr;
    }

    bool isEditable = parcel.ReadBool();
    pixelMap->SetEditable(isEditable);

    bool isAstc = parcel.ReadBool();
    pixelMap->SetAstc(isAstc);

    OHOS::Media::AllocatorType allocType = static_cast<OHOS::Media::AllocatorType>(parcel.ReadInt32());
    int32_t csm = parcel.ReadInt32();
    if (csm != -1/*OHOS::Media::ERR_MEDIA_INVALID_VALUE*/) {
        //OHOS::ColorManager::ColorSpaceName colorSpaceName = static_cast<OHOS::ColorManager::ColorSpaceName>(csm);
        //OHOS::ColorManager::ColorSpace grColorSpace = OHOS::ColorManager::ColorSpace(colorSpaceName);
        //pixelMap->InnerSetColorSpace(grColorSpace);
    }
    int32_t rowDataSize = parcel.ReadInt32();
    int32_t bufferSize = parcel.ReadInt32();
    int32_t bytesPerPixel = ImageUtilsGetPixelBytes(imgInfo.pixelFormat);
    if (bytesPerPixel == 0) {
        delete pixelMap;
        HiviewDFX::HiLog::Error(label, "unmarshalling get bytes by per pixel fail.");
        return nullptr;
    }
    if ((!isAstc) && bufferSize != rowDataSize * imgInfo.size.height) {
        delete pixelMap;
        HiviewDFX::HiLog::Error(label, "unmarshalling bufferSize parcelling error");
        constexpr uint32_t errCode = (30 << 21) | (1 << 16) | 9; // ERR_IMAGE_BUFFER_SIZE_PARCEL_ERROR
        OHOS::Media::PixelMap::ConstructPixelMapError(error, errCode,
            "unmarshalling bufferSize parcelling error");
        return nullptr;
    }
    uint8_t *base = nullptr;
    void *context = nullptr;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    constexpr int headerLength = 24;
    if (allocType == OHOS::Media::AllocatorType::SHARE_MEM_ALLOC) {
        int fd = ReadFileDescriptor(parcel);
        if (fd < 0) {
            ReplayImageCacheRecord* ptr = RSProfilerBase::ReplayImageGet(replayUniqueId);

            if (ptr != nullptr && static_cast<uint32_t>(bufferSize) == ptr->imageSize) {
                // get image from file
                base = static_cast<uint8_t*>(malloc(bufferSize)); 
                memmove(base, ptr->image.get(), bufferSize);
                context = nullptr;
                
                parcel.SkipBytes(headerLength);
            } else {
                // original
                HiviewDFX::HiLog::Error(label, "fd < 0");
                delete pixelMap;
                constexpr uint32_t errCode = (30 << 21) | (1 << 16) | 84; // ERR_IMAGE_GET_FD_BAD
                OHOS::Media::PixelMap::ConstructPixelMapError(error, errCode, "fd acquisition failed");
                return nullptr;
            }
        }
        else {
            void* ptr = ::mmap(nullptr, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED) {
                ptr = ::mmap(nullptr, bufferSize, PROT_READ, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED) {
                    ::close(fd);
                    delete pixelMap;
                    HiviewDFX::HiLog::Error(label, "shared memory map in memalloc failed, errno:%{public}d", errno);
                    constexpr uint32_t errCode = (30 << 21) | (1 << 16) | 84; // ERR_IMAGE_GET_FD_BAD
                    OHOS::Media::PixelMap::ConstructPixelMapError(error, errCode, "shared memory map in memalloc failed");
                    return nullptr;
                }
            }

            RSProfilerBase::ReplayImageAdd(replayUniqueId, ptr, bufferSize, headerLength);

            context = new int32_t();
            if (context == nullptr) {
                ::munmap(ptr, bufferSize);
                ::close(fd);
                delete pixelMap;
                return nullptr;
            }
            *static_cast<int32_t *>(context) = fd;
            base = static_cast<uint8_t *>(ptr);
        }
    } else if (allocType == OHOS::Media::AllocatorType::DMA_ALLOC) {
        ReplayImageCacheRecord* ptr = RSProfilerBase::ReplayImageGet(replayUniqueId);

        if (ptr == nullptr) {
            const size_t size1 = parcel.GetReadableBytes();

            // original code
            sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
            surfaceBuffer->ReadFromMessageParcel(static_cast<MessageParcel&>(parcel));
            uint8_t* virAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
            void* nativeBuffer = surfaceBuffer.GetRefPtr();
            ImageUtilsSurfaceBufferReference(nativeBuffer);
            base = virAddr;
            context = nativeBuffer;

            const size_t size2 = parcel.GetReadableBytes();

            RSProfilerBase::ReplayImageAdd(replayUniqueId, virAddr, bufferSize, size1 - size2);

            // NOLINTNEXTLINE
            RS_LOGD("RSMainThread::MainLoop Server REPLAY DMA ReplayImageAdd replay_unique_id=%lu bufferSize=%d " 
                    "skip_size=%d", 
                replayUniqueId, bufferSize, (int)(size1 - size2));
        } else {
            RS_LOGD("RSMainThread::MainLoop Server REPLAY DMA SKIP skip_size=%d", ptr->skipBytes); 

            allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

            parcel.SkipBytes(ptr->skipBytes);

            base = static_cast<uint8_t*>(malloc(bufferSize)); 
            memmove(base, ptr->image.get(), bufferSize);
            context = nullptr;
        }

    } else {
        base = ReadImageData(parcel, bufferSize);
        if (base == nullptr) {
            HiviewDFX::HiLog::Error(label, "get pixel memory size:[%{public}d] error.", bufferSize);
            delete pixelMap;
            constexpr uint32_t errCode = (30 << 21) | (1 << 16) | 9; // ERR_IMAGE_GET_DATA_ABNORMAL
            OHOS::Media::PixelMap::ConstructPixelMapError(error, errCode, "ReadImageData failed");
            return nullptr;
        }
    }
#else
    base = ReadImageData(parcel, bufferSize);
    if (base == nullptr) {
        HiviewDFX::HiLog::Error(LABEL, "get pixel memory size:[%{public}d] error.", bufferSize);
        delete pixelMap;
        return nullptr;
    }
#endif

    uint32_t ret = pixelMap->SetImageInfo(imgInfo);
    if (ret != 0/*SUCCESS*/) {
        if (pixelMap->freePixelMapProc_ != nullptr) {
            pixelMap->freePixelMapProc_(base, context, bufferSize);
        }
        ReleaseMemory(allocType, base, context, bufferSize);
        if (allocType == OHOS::Media::AllocatorType::SHARE_MEM_ALLOC && context != nullptr) {
            delete static_cast<int32_t *>(context);
        }
        delete pixelMap;
        HiviewDFX::HiLog::Error(label, "create pixel map from parcel failed, set image info error.");
        return nullptr;
    }
    pixelMap->SetPixelsAddr(base, context, bufferSize, allocType, nullptr);
    if (!pixelMap->ReadTransformData(parcel, pixelMap)) {
        HiviewDFX::HiLog::Error(label, "read transformData fail");
        delete pixelMap;
        return nullptr;
    }
    if (!pixelMap->ReadAstcRealSize(parcel, pixelMap)) {
        HiviewDFX::HiLog::Error(label, "read astcrealSize fail");
        delete pixelMap;
        return nullptr;
    }
    return pixelMap;
}

Vector4f RSProfilerBase::GetScreenRect()
{
    // (user): Make it relevant via a display info request
    static const Vector4f SCREEN_RECT(0.0, 0.0, 1344.0, 2772.0);
    return SCREEN_RECT;
}

void RSProfilerBase::TransactionDataOnProcess(RSContext& context)
{
    if (RSProfilerBase::SpecParseModeGet() != SpecParseMode::READ) { 
        return;
    }

    auto& nodeMap = context.GetNodeMap();
    std::shared_ptr<RSRenderNode> baseNode = nullptr;
    NodeId baseNodeId = 0;
    const Vector4f screenRect = RSProfilerBase::GetScreenRect();
    for (auto item : RSProfilerBase::SpecParsePidListGet()) {
        const NodeId nodeId = (static_cast<NodeId>(item) << bits32) | 1 | (static_cast<NodeId>(1) << (bits30 + bits32));
        auto node = nodeMap.GetRenderNode(nodeId);
        if (node) {
            baseNode = node;
            baseNodeId = nodeId;
            RSProperties& properties = node->GetMutableRenderProperties();
            properties.SetBounds(screenRect);
            properties.SetFrame(screenRect);
            auto parentNode = nodeMap.GetRenderNode(RSProfilerBase::SpecParseParentNodeGet());
            if (parentNode) {
                RSProperties& parentProperties = parentNode->GetMutableRenderProperties();
                parentProperties.SetBounds(screenRect);
                parentProperties.SetFrame(screenRect);
            }
        }
    }

    if (baseNode) {
        BaseNodeCommandHelper::ClearChildren(context, RSProfilerBase::SpecParseParentNodeGet());
        BaseNodeCommandHelper::AddChild(context, RSProfilerBase::SpecParseParentNodeGet(), baseNodeId, 0);
    }
}

} // namespace OHOS::Rosen