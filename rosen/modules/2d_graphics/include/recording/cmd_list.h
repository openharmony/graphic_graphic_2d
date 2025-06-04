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

#ifndef CMD_LIST_H
#define CMD_LIST_H

#include <map>
#include <optional>
#include <vector>

#include "draw/canvas.h"
#include "recording/op_item.h"
#include "recording/mem_allocator.h"
#include "recording/recording_handle.h"
#include "utils/drawing_macros.h"
#include "utils/extend_object.h"
#ifdef ROSEN_OHOS
#include "surface_buffer.h"
#include "sync_fence.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
using CmdListData = std::pair<const void*, size_t>;
using NodeId = uint64_t;
constexpr size_t MAX_OPITEMSIZE = 170000;
constexpr size_t RECORD_CMD_MAX_DEPTH = 800;
constexpr size_t RECORD_CMD_MAX_SIZE = 100000;

class DRAWING_API ExtendImageObject {
public:
    virtual ~ExtendImageObject() = default;
    virtual void Playback(Canvas& canvas, const Rect& rect,
        const SamplingOptions& sampling, bool isBackground = false) = 0;
    virtual void SetNodeId(NodeId id) {};
    virtual void SetPaint(Paint paint) {};
    virtual void Purge() {};
    virtual bool IsValid() {return false;};
    virtual void Dump(std::string& dump) {};
    virtual NodeId GetNodeId() const
    {
        return 0;
    }
};

class DRAWING_API ExtendImageBaseObj {
public:
    virtual ~ExtendImageBaseObj() = default;
    virtual void Playback(Canvas& canvas, const Rect& rect, const SamplingOptions& sampling,
        SrcRectConstraint constraint = SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) = 0;
    virtual void SetNodeId(NodeId id) {};
    virtual void Purge() {};
};

class DRAWING_API ExtendImageNineObject {
public:
    virtual ~ExtendImageNineObject() = default;
    virtual void Playback(Canvas& canvas, const RectI& center, const Rect& dst,
        FilterMode filterMode = FilterMode::NEAREST) = 0;
    virtual void SetNodeId(NodeId id) {};
    virtual void Purge() {};
};

class DRAWING_API ExtendImageLatticeObject {
public:
    virtual ~ExtendImageLatticeObject() = default;
    virtual void Playback(Canvas& canvas, const Lattice& lattice, const Rect& dst,
        FilterMode filterMode = FilterMode::NEAREST) = 0;
    virtual void SetNodeId(NodeId id) {};
    virtual void Purge() {};
};

class DRAWING_API ExtendDrawFuncObj {
public:
    virtual ~ExtendDrawFuncObj () = default;
    virtual void Playback(Canvas* canvas, const Rect* rect) = 0;
};

#ifdef ROSEN_OHOS
struct SurfaceBufferEntry {
    SurfaceBufferEntry() = default;
    SurfaceBufferEntry(const sptr<SurfaceBuffer> surfaceBuffer, const sptr<SyncFence> acquireFence)
        : surfaceBuffer_(surfaceBuffer), acquireFence_(acquireFence)
    {}
    sptr<SurfaceBuffer> surfaceBuffer_ = nullptr;
    sptr<SyncFence> acquireFence_ = nullptr;
};
#endif

class DRAWING_API CmdList {
public:
    enum Type : uint32_t {
        CMD_LIST = 0,
        DRAW_CMD_LIST,
        MASK_CMD_LIST,
    };

    CmdList() = default;
    explicit CmdList(const CmdListData& cmdListData);
    virtual ~CmdList();

    virtual uint32_t GetType() const
    {
        return Type::CMD_LIST;
    }

    /**
     * @brief       Add OpItem to CmdList.
     * @param T     The name of OpItem class.
     * @param Args  Constructs arguments to the OpItem.
     */
    template<typename T, typename... Args>
    void AddOp(Args&&... args)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        T* op = opAllocator_.Allocate<T>(std::forward<Args>(args)...);
        if (op == nullptr) {
            return;
        }

        size_t offset = opAllocator_.AddrToOffset(op);
        if (lastOpItemOffset_.has_value()) {
#ifdef CROSS_PLATFORM
            auto* lastOpItem = static_cast<OpItem*>(
                opAllocator_.OffsetToAddr(lastOpItemOffset_.__get(), sizeof(OpItem)));
#else
            auto* lastOpItem = static_cast<OpItem*>(
                opAllocator_.OffsetToAddr(lastOpItemOffset_.value(), sizeof(OpItem)));
#endif
            if (lastOpItem != nullptr) {
                lastOpItem->SetNextOpItemOffset(offset);
            }
        }
        lastOpItemOffset_.emplace(offset);
        opCnt_++;
    }

    /**
     * @brief       Add a contiguous buffers to the CmdList.
     * @param data  A contiguous buffers.
     * @return      Returns the offset of the contiguous buffers and CmdList head point.
     */
    size_t AddCmdListData(const CmdListData& data);

    const void* GetCmdListData(size_t offset, size_t size) const;

    /**
     * @brief   Gets the contiguous buffers of CmdList.
     */
    CmdListData GetData() const;

    // using for recording, should to remove after using shared memory
    bool SetUpImageData(const void* data, size_t size);
    size_t AddImageData(const void* data, size_t size);
    const void* GetImageData(size_t offset, size_t size) const;
    CmdListData GetAllImageData() const;

    OpDataHandle AddImage(const Image& image);
    std::shared_ptr<Image> GetImage(const OpDataHandle& imageHandle);

    size_t AddBitmapData(const void* data, size_t size);
    const void* GetBitmapData(size_t offset, size_t size) const;
    bool SetUpBitmapData(const void* data, size_t size);
    CmdListData GetAllBitmapData() const;

    /*
     * @brief  return ExtendObject index. UINT32_MAX is error.
     */
    uint32_t AddExtendObject(const std::shared_ptr<ExtendObject>& object);

    /*
     * @brief  get ExtendObject by index.
     */
    std::shared_ptr<ExtendObject> GetExtendObject(uint32_t index);

    /*
     * @brief  return ExtendObject size, 0 is no ExtendObject.
     */
    uint32_t GetAllExtendObject(std::vector<std::shared_ptr<ExtendObject>>& objectList);

    /*
     * @brief  return real setup ExtendObject size.
     */
    uint32_t SetupExtendObject(const std::vector<std::shared_ptr<ExtendObject>>& objectList);

    /*
     * @brief  return RecordCmd index. UINT32_MAX is error.
     */
    uint32_t AddRecordCmd(const std::shared_ptr<RecordCmd>& recordCmd);

    /*
     * @brief  get RecordCmd by index.
     */
    std::shared_ptr<RecordCmd> GetRecordCmd(uint32_t index);

    /*
     * @brief  return real setup RecordCmd size.
     */
    uint32_t SetupRecordCmd(std::vector<std::shared_ptr<RecordCmd>>& recordCmdList);

    /*
     * @brief  return RecordCmd size, 0 is no RecordCmd.
     */
    uint32_t GetAllRecordCmd(std::vector<std::shared_ptr<RecordCmd>>& recordCmdList);

    /*
     * @brief  return imageObject index, negative is error.
     */
    uint32_t AddImageObject(const std::shared_ptr<ExtendImageObject>& object);

    /*
     * @brief  get imageObject by index.
     */
    std::shared_ptr<ExtendImageObject> GetImageObject(uint32_t id);

    /*
     * @brief  return imageObject size, 0 is no imageObject.
     */
    uint32_t GetAllObject(std::vector<std::shared_ptr<ExtendImageObject>>& objectList);

    /*
     * @brief  return real setup imageObject size.
     */
    uint32_t SetupObject(const std::vector<std::shared_ptr<ExtendImageObject>>& objectList);

     /*
     * @brief  return imageBaseObj index, negative is error.
     */
    uint32_t AddImageBaseObj(const std::shared_ptr<ExtendImageBaseObj>& object);

    /*
     * @brief  get imageBaseObj by index.
     */
    std::shared_ptr<ExtendImageBaseObj> GetImageBaseObj(uint32_t id);

    /*
     * @brief  return imageBaseObj size, 0 is no imageBaseObj.
     */
    uint32_t GetAllBaseObj(std::vector<std::shared_ptr<ExtendImageBaseObj>>& objectList);

    /*
     * @brief  return real setup imageBaseObj size.
     */
    uint32_t SetupBaseObj(const std::vector<std::shared_ptr<ExtendImageBaseObj>>& objectList);

     /*
     * @brief  return imageNineObject index, negative is error.
     */
    uint32_t AddImageNineObject(const std::shared_ptr<ExtendImageNineObject>& object);

    /*
     * @brief  get imageNineObject by index.
     */
    std::shared_ptr<ExtendImageNineObject> GetImageNineObject(uint32_t id);

    /*
     * @brief  return imageNineObject size, 0 is no imageNineObject.
     */
    uint32_t GetAllImageNineObject(std::vector<std::shared_ptr<ExtendImageNineObject>>& objectList);

    /*
     * @brief  return real setup imageNineObject size.
     */
    uint32_t SetupImageNineObject(const std::vector<std::shared_ptr<ExtendImageNineObject>>& objectList);

     /*
     * @brief  return imageLatticeObject index, negative is error.
     */
    uint32_t AddImageLatticeObject(const std::shared_ptr<ExtendImageLatticeObject>& object);

    /*
     * @brief  get imageLatticeObject by index.
     */
    std::shared_ptr<ExtendImageLatticeObject> GetImageLatticeObject(uint32_t id);

    /*
     * @brief  return imageLatticeObject size, 0 is no imageLatticeObject.
     */
    uint32_t GetAllImageLatticeObject(std::vector<std::shared_ptr<ExtendImageLatticeObject>>& objectList);

    /*
     * @brief  return real setup imageLatticeObject size.
     */
    uint32_t SetupImageLatticeObject(const std::vector<std::shared_ptr<ExtendImageLatticeObject>>& objectList);

     /*
     * @brief  return DrawFuncObj index, negative is error.
     */
    uint32_t AddDrawFuncOjb(const std::shared_ptr<ExtendDrawFuncObj>& object);

    /*
     * @brief  get DrawFuncObj by index.
     */
    std::shared_ptr<ExtendDrawFuncObj> GetDrawFuncObj(uint32_t id);

    /*
     * @brief  copy object vec to another CmdList.
     */
    void CopyObjectTo(CmdList& other) const;

    /*
     * @brief  return recording op count.
     */
    uint32_t GetOpCnt() const;

    CmdList(CmdList&&) = delete;
    CmdList(const CmdList&) = delete;
    CmdList& operator=(CmdList&&) = delete;
    CmdList& operator=(const CmdList&) = delete;

#ifdef ROSEN_OHOS
    /*
     * @brief  return surfaceBufferEntry index, negative is error.
     */
    uint32_t AddSurfaceBufferEntry(const std::shared_ptr<SurfaceBufferEntry>& surfaceBufferEntry);

    /*
     * @brief  get surfaceBufferEntry by index.
     */
    std::shared_ptr<SurfaceBufferEntry> GetSurfaceBufferEntry(uint32_t id);

    /*
     * @brief  return surfaceBufferEntry size, 0 is no surfaceBuffer.
     */
    uint32_t GetAllSurfaceBufferEntry(std::vector<std::shared_ptr<SurfaceBufferEntry>>& objectList);

    /*
     * @brief  return real setup surfaceBufferEntry size.
     */
    uint32_t SetupSurfaceBufferEntry(const std::vector<std::shared_ptr<SurfaceBufferEntry>>& objectList);
#endif

protected:
    void ProfilerPushObjects(std::stringstream& stream, size_t size);
    void ProfilerPopObjects(std::stringstream& stream, size_t size);

    MemAllocator opAllocator_;
    MemAllocator imageAllocator_;
    MemAllocator bitmapAllocator_;
    std::optional<size_t> lastOpItemOffset_ = std::nullopt;
    std::recursive_mutex mutex_;
    std::map<size_t, std::shared_ptr<Image>> imageMap_;
    std::vector<std::pair<size_t, OpDataHandle>> imageHandleVec_;
    uint32_t opCnt_ = 0;

    std::vector<std::shared_ptr<RecordCmd>> recordCmdVec_;
    std::mutex recordCmdMutex_;
    std::vector<std::shared_ptr<ExtendImageObject>> imageObjectVec_;
    std::mutex imageObjectMutex_;
    std::vector<std::shared_ptr<ExtendImageBaseObj>> imageBaseObjVec_;
    std::mutex imageBaseObjMutex_;
    std::vector<std::shared_ptr<ExtendImageNineObject>> imageNineObjectVec_;
    std::mutex imageNineObjectMutex_;
    std::vector<std::shared_ptr<ExtendImageLatticeObject>> imageLatticeObjectVec_;
    std::mutex imageLatticeObjectMutex_;
    std::vector<std::shared_ptr<ExtendObject>> extendObjectVec_;
    std::mutex extendObjectMutex_;
#ifdef ROSEN_OHOS
    std::vector<std::shared_ptr<SurfaceBufferEntry>> surfaceBufferEntryVec_;
    std::mutex surfaceBufferEntryMutex_;
#endif
    std::vector<std::shared_ptr<ExtendDrawFuncObj>> drawFuncObjVec_;
    std::mutex drawFuncObjMutex_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif