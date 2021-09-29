/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_SURFACE_EGL_DATA_H
#define INTERFACES_INNERKITS_SURFACE_EGL_DATA_H

#include <refbase.h>
#include <GLES2/gl2.h>

namespace OHOS {
class EglData : public RefBase {
public:
    virtual GLuint GetFrameBufferObj() const = 0;

protected:
    EglData(){}
    virtual ~EglData(){}
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_EGL_DATA_H
