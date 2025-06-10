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

#ifndef DRAW_CMD_LIST_H
#define DRAW_CMD_LIST_H

#include "draw/canvas.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawOpItem;
class DRAWING_API DrawCmdList : public CmdList {
public:
    /**
     * @brief   there are five enable type for Hybrid Render
     * @param   NONE       default type, not enable Hybrid Render
     * @param   TEXT       text type
     * @param   SVG        svg type
     * @param   HMSYMBOL   HMSymbol type
     * @param   CANVAS     canvasDrawingNode type
     * @detail  enable type for Hybrid Render
     */
    enum class HybridRenderType : uint32_t {
        NONE,
        TEXT,
        SVG,
        HMSYMBOL,
        CANVAS
    };

    /**
     * @brief   there are two mode for DrawCmdList to add new op
     * @param   IMMEDIATE   add op to continouns buffer immediately, overload will benefit from this
     * @param   DEFERRED    add op to vector and then add to contiguous buffer if needed
     * @detail  playback can get all op from continouns buffer in IMMEDIATE mode or vector int DEFERRED mode
     */
    enum class UnmarshalMode {
        IMMEDIATE,
        DEFERRED
    };

    /**
     * @brief   Creates a DrawCmdList with contiguous buffers.
     * @param   data    A contiguous buffers.
     * @param   isCopy  Whether to copy data or not.
     * @detail  Called only by Unmarshalling-Thread, the default mode is DEFERRED since all DrawOp store in vector
     */
    static std::shared_ptr<DrawCmdList> CreateFromData(const CmdListData& data, bool isCopy = false);

    /**
     * @brief   Creates a DrawCmdList
     */
    DrawCmdList(UnmarshalMode mode = UnmarshalMode::IMMEDIATE);
    DrawCmdList(int32_t width, int32_t height, UnmarshalMode mode = UnmarshalMode::IMMEDIATE);

    /**
     * @brief   Destroy a DrawCmdList
     */
    ~DrawCmdList() override;

    /**
     * @brief   Gets cmd list type.
     * @return  Returns DRAW_CMD_LIST
     */
    uint32_t GetType() const override
    {
        return Type::DRAW_CMD_LIST;
    }

    /**
     * @brief   Add DrawOpItem to DrawCmdList, only can be used in IMMEDIATE mode
     * @param   T   The name of DrawOpItem class
     * @param   Args    Constructs arguments to the DrawOpItem
     * @return  true if add success, false if not in IMMEDIATE mode or create op in contiguous buffer failed
     */
    template<typename T, typename... Args>
    bool AddDrawOp(Args&&... args)
    {
        if (mode_ != UnmarshalMode::IMMEDIATE) {
            return false;
        }
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        T* op = opAllocator_.Allocate<T>(std::forward<Args>(args)...);
        if (op == nullptr) {
            return false;
        }

        uint32_t offset = opAllocator_.AddrToOffset(op);
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
        return true;
    }

    /**
     * @brief   Add DrawOpItem to DrawCmdList, only can be used in DEFERRED mode
     * @param   drawOpItem  A real DrawOpItem instance
     * @return  true if add success, false if not in DEFERRED mode
     */
    bool AddDrawOp(std::shared_ptr<DrawOpItem>&& drawOpItem);

    /**
     * @brief   Clear DrawOpItem in contiguous buffer, draw op vector, and other resource associated with draw op
     */
    void ClearOp();

    /**
     * @brief   Get op vector size
     */
    size_t GetOpItemSize() const;

    /**
     * @brief   for each op in vector, combine there desc together
     */
    std::string GetOpsWithDesc() const;

    /**
     * @brief   Marshalling Draw Ops Param from vector to contiguous buffers.
     */
    void MarshallingDrawOps();

    /**
     * @brief   Marshalling Draw Ops Param from vector to contiguous buffers. For Profiler Only.
     */
     void ProfilerMarshallingDrawOps(Drawing::DrawCmdList *cmdlist);

    /**
     * @brief   Unmarshalling Draw Ops from contiguous buffers to vector
     *          it is only called by Unmarshalling-Thread, the mode should be set to DEFERRED when create.
     */
    void UnmarshallingDrawOps(uint32_t* opItemCount = nullptr);

    /**
     * @brief   Change typeface ids adding 1 << (30 + 32) - used for profiler replay
     */
    void PatchTypefaceIds(std::shared_ptr<Drawing::DrawCmdList> refDrawCmdList = nullptr);

    /**
     * @brief   Draw cmd is empty or not.
     */
    bool IsEmpty() const;

    /**
     * @brief         Calls the corresponding operations of all opitems in DrawCmdList to the canvas.
     * @param canvas  Implements the playback action of the DrawCmdList in the Canvas.
     * @param rect    Rect used to playback, may be nullptr.
     */
    void Playback(Canvas& canvas, const Rect* rect = nullptr);

    /**
     * @brief  Gets the width of the DrawCmdList.
     */
    int32_t GetWidth() const;

    /**
     * @brief  Gets the height of the DrawCmdList.
     */
    int32_t GetHeight() const;

    /**
     * @brief  Sets the width of the DrawCmdList.
     */
    void SetWidth(int32_t width);

    /**
     * @brief  Sets the height of the DrawCmdList.
     */
    void SetHeight(int32_t height);

    /**
     * @brief  Gets whether DrawCmdList needs to be UICaptured.
     */
    bool GetNoNeedUICaptured() const;

    /**
     * @brief  Sets whether DrawCmdList needs to be UICaptured.
     */
    void SetNoNeedUICaptured(bool noNeedUICaptured);

    /**
     * @brief   Convert Textblob Op to Image Op, it is different for difference mode
     *          IMMEDIATE: the Image Op will add to the end of buffer, and the mapped offset will be recorded in
     *          replacedOpListForBuffer.
     *          DEFERRED: the image Op will replace the Textblob op in vector, and the index-op_ptr will be recorded
     *          in replacedOpListForVector.
     */
    void GenerateCache(Canvas* canvas = nullptr, const Rect* rect = nullptr);

    bool GetIsCache() const;

    void SetIsCache(bool isCached);

    bool GetCachedHighContrast() const;

    void SetCachedHighContrast(bool cachedHighContrast);

    std::vector<std::pair<size_t, size_t>> GetReplacedOpList();

    void SetReplacedOpList(std::vector<std::pair<size_t, size_t>> replacedOpList);

    DrawCmdList::HybridRenderType GetHybridRenderType() const;

    void SetHybridRenderType(DrawCmdList::HybridRenderType hybridRenderType);

    void UpdateNodeIdToPicture(NodeId nodeId);

    void Dump(std::string& out);

    void Purge();

    void SetIsNeedUnmarshalOnDestruct(bool isNeedUnmarshalOnDestruct);

    size_t GetSize();

    void SetCanvasDrawingOpLimitEnable(bool isEnable);

    /**
     * @brief Gets the pixelmap rect for hybrid render.
     */
    void GetBounds(Rect& rect);

    /**
     * @brief Check whether enable hybrid render.
     */
    bool IsHybridRenderEnabled(uint32_t maxPixelMapWidth, uint32_t maxPixelMapHeight);

    const std::vector<std::shared_ptr<DrawOpItem>> GetDrawOpItems() const;

private:
    void ClearCache();
    void GenerateCacheByVector(Canvas* canvas, const Rect* rect);
    void GenerateCacheByBuffer(Canvas* canvas, const Rect* rect);

    void PlaybackToDrawCmdList(std::shared_ptr<DrawCmdList> drawCmdList);
    void PlaybackByVector(Canvas& canvas, const Rect* rect = nullptr);
    bool UnmarshallingDrawOpsSimple(std::vector<std::shared_ptr<DrawOpItem>>& drawOpItems, size_t& lastOpGenSize);
    void PlaybackByBuffer(Canvas& canvas, const Rect* rect = nullptr);
    void CaculatePerformanceOpType();

    void ProfilerTextBlob(void* handle, uint32_t count, std::shared_ptr<Drawing::DrawCmdList> refDrawCmdList = nullptr);

    int32_t width_;
    int32_t height_;
    const UnmarshalMode mode_;
    const size_t offset_ = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    std::vector<std::shared_ptr<DrawOpItem>> drawOpItems_;

    size_t lastOpGenSize_ = 0;
    std::vector<std::pair<size_t, size_t>> replacedOpListForBuffer_;
    std::vector<std::pair<int, std::shared_ptr<DrawOpItem>>> replacedOpListForVector_;
    bool isCached_ = false;
    bool cachedHighContrast_ = false;
    uint32_t performanceCaculateOpType_ = 0;
    bool isNeedUnmarshalOnDestruct_ = false;
    bool noNeedUICaptured_ = false;
    bool isCanvasDrawingOpLimitEnabled_ = false;

    DrawCmdList::HybridRenderType hybridRenderType_ = DrawCmdList::HybridRenderType::NONE;
};

using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // DRAW_CMD_LIST_H
