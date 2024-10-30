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

#ifndef RS_POINTER_DRAWING_MANAGER_H
#define RS_POINTER_DRAWING_MANAGER_H

#include "common/rs_thread_handler.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPointerDrawingManager {
public:
    static RSPointerDrawingManager& Instance();
    std::mutex mtx_;

    struct BoundParam {
        float x;
        float y;
        float z;
        float w;
    };

    bool GetIsPointerEnableHwc() const
    {
        return isPointerEnableHwc_.load();
    }

    void SetIsPointerEnableHwc(bool flag)
    {
        isPointerEnableHwc_.store(flag);
    }

    bool GetIsPointerCanSkipFrame() const
    {
        return isPointerCanSkipFrame_.load();
    }

    void SetIsPointerCanSkipFrame(bool flag)
    {
        isPointerCanSkipFrame_.store(flag);
    }

    bool IsPointerCanSkipFrameCompareChange(bool flag, bool changeFlag)
    {
        bool expectChanged = flag;
        return isPointerCanSkipFrame_.compare_exchange_weak(expectChanged, changeFlag);
    }

    int64_t GetRsNodeId() const
    {
        return rsNodeId_;
    }

    void SetRsNodeId(int64_t id)
    {
        rsNodeId_ = id;
    }

    bool GetBoundHasUpdate() const
    {
        return boundHasUpdate_.load();
    }

    void SetBoundHasUpdate(bool flag)
    {
        boundHasUpdate_.store(flag);
    }

    bool BoundHasUpdateCompareChange(bool flag, bool changeFlag)
    {
        bool expectChanged = flag;
        return boundHasUpdate_.compare_exchange_weak(expectChanged, changeFlag);
    }

    BoundParam GetBound() const
    {
        return bound_;
    }

    void SetBound(BoundParam bound)
    {
        bound_.x = bound.x;
        bound_.y = bound.y;
        bound_.z = bound.z;
        bound_.w = bound.w;
    }

    void UpdatePointerInfo();

private:
    std::atomic<bool> isPointerEnableHwc_ = true;
    std::atomic<bool> isPointerCanSkipFrame_ = false;

    BoundParam bound_;
    int64_t rsNodeId_ = -1;
    std::atomic<bool> boundHasUpdate_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_POINTER_DRAWING_MANAGER_H
