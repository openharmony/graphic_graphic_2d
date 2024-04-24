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
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
using CmdListData = std::pair<const void*, size_t>;
using NodeId = uint64_t;

class DRAWING_API ExtendImageObject {
public:
    virtual ~ExtendImageObject() = default;
    virtual void Playback(Canvas& canvas, const Rect& rect,
        const SamplingOptions& sampling, bool isBackground = false) = 0;
    virtual void SetNodeId(NodeId id) {};
};

class DRAWING_API ExtendImageBaseObj {
public:
    virtual ~ExtendImageBaseObj() = default;
    virtual void Playback(Canvas& canvas, const Rect& rect,
        const SamplingOptions& sampling) = 0;
    virtual void SetNodeId(NodeId id) {};
};

class DRAWING_API ExtendDrawFuncObj {
public:
    virtual ~ExtendDrawFuncObj () = default;
    virtual void Playback(Canvas* canvas, const Rect* rect) = 0;
};

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

        uint32_t offset = opAllocator_.AddrToOffset(op);
        if (lastOpItemOffset_.has_value()) {
            auto* lastOpItem = static_cast<OpItem*>(opAllocator_.OffsetToAddr(lastOpItemOffset_.value()));
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
    uint32_t AddCmdListData(const CmdListData& data);

    const void* GetCmdListData(uint32_t offset) const;

    /**
     * @brief   Gets the contiguous buffers of CmdList.
     */
    CmdListData GetData() const;

    // using for recording, should to remove after using shared memory
    bool SetUpImageData(const void* data, size_t size);
    uint32_t AddImageData(const void* data, size_t size);
    const void* GetImageData(uint32_t offset) const;
    CmdListData GetAllImageData() const;

    OpDataHandle AddImage(const Image& image);
    std::shared_ptr<Image> GetImage(const OpDataHandle& imageHandle);

    uint32_t AddBitmapData(const void* data, size_t size);
    const void* GetBitmapData(uint32_t offset) const;
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
     * @brief  return surfaceBuffer index, negative is error.
     */
    uint32_t AddSurfaceBuffer(const sptr<SurfaceBuffer>& surfaceBuffer);

    /*
     * @brief  get surfaceBuffer by index.
     */
    sptr<SurfaceBuffer> GetSurfaceBuffer(uint32_t id);

    /*
     * @brief  return surfaceBuffer size, 0 is no surfaceBuffer.
     */
    uint32_t GetAllSurfaceBuffer(std::vector<sptr<SurfaceBuffer>>& objectList);

    /*
     * @brief  return real setup surfaceBuffer size.
     */
    uint32_t SetupSurfaceBuffer(const std::vector<sptr<SurfaceBuffer>>& objectList);
#endif

protected:
    MemAllocator opAllocator_;
    MemAllocator imageAllocator_;
    MemAllocator bitmapAllocator_;
    std::optional<uint32_t> lastOpItemOffset_ = std::nullopt;
    std::recursive_mutex mutex_;
    std::map<uint32_t, std::shared_ptr<Image>> imageMap_;
    std::vector<std::pair<uint32_t, OpDataHandle>> imageHandleVec_;
    uint32_t opCnt_ = 0;

    std::vector<std::shared_ptr<ExtendImageObject>> imageObjectVec_;
    std::mutex imageObjectMutex_;
    std::vector<std::shared_ptr<ExtendImageBaseObj>> imageBaseObjVec_;
    std::mutex imageBaseObjMutex_;
    std::vector<std::shared_ptr<ExtendObject>> extendObjectVec_;
    std::mutex extendObjectMutex_;
#ifdef ROSEN_OHOS
    std::vector<sptr<SurfaceBuffer>> surfaceBufferVec_;
    std::mutex surfaceBufferMutex_;
#endif
    std::vector<std::shared_ptr<ExtendDrawFuncObj>> drawFuncObjVec_;
    std::mutex drawFuncObjMutex_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif