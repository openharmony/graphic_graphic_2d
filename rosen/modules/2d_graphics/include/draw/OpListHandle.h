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

#ifndef OP_LIST_HANDLE_H
#define OP_LIST_HANDLE_H

#include <vector>
#include <utils/rect.h>
#include "drawing/engine_adapter/impl_interface/OpListHandleImpl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum CanDrawOpErrType : int {
    CANDRAW_SUCCESS = 0,
    CANDRAW_MULTIPLE = 1,
    CANDRAW_NOTREUSED = 2,
    CANDRAW_CHECKTRANS = 3,
    CANDRAW_CHECKTRANS_TYPE = 4,
    CANDRAW_NOCLIPRECT = 5,
    CANDRAW_AFTERCLIP = 6,
    CANDRAW_EMPTRPTR = 7,
    CANDRAW_OPLIST_EMPTY = 8,
};

class DRAWING_API OpListHandle {
public:
struct OpInfo {
    bool canReUseCache = false;
    int num = 0;
    int percent = 0;
    Rect unionRect;
    std::vector<Rect> drawAreaRects;
};

    OpListHandle();
    OpListHandle(OpInfo opinfo);
    ~OpListHandle() {}

    const OpInfo &GetOpInfo() const
    {
        return opInfo_;
    }

template<typename T>
    T* GetImpl() const
    {
        return opImpl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<OpListHandleImpl> opImpl_; // for opinc diff
    OpInfo opInfo_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
