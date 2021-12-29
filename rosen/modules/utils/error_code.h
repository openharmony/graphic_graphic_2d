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

#ifndef ERROR_CODE_H
#define ERROR_CODE_H
namespace Rosen {
enum GraphicsErrorCode {
    NO_ERR = 0,
    // 1-100 for Render Service

    // 101-200 for animation

    // 201-300 for 2d graphics
    2D_GRAPHICS_OBJ_CREATE_FAILED = 201,
    2D_GRAPHICS_DECODE_FILE_INVALID,

    // 301-400 for effect

    // 401-500 for backstore

    // 501-600 for composor

    // 601-700 for wms

};
}
#endif