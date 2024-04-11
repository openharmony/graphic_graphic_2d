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

#ifndef DRAWING_EXTEND_OBJECT_H
#define DRAWING_EXTEND_OBJECT_H

#ifdef ROSEN_OHOS
#include <parcel.h>
#endif

#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
/*
 * This is an abstract class. The class used by drawing, but must depend Class in rs, should extend this class.
 */
class DRAWING_API ExtendObject {
public:
    ExtendObject() = default;
    virtual ~ExtendObject() = default;
#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel&) = 0;
    virtual bool Unmarshalling(Parcel&) = 0;
#endif
    virtual void* GenerateBaseObject() = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif  // DRAWING_EXTEND_OBJECT_H