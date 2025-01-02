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

//
// Created on 2024/8/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef COMMON_LOG_DRAW_H
#define COMMON_LOG_DRAW_H

#include <cstdio>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_text_blob.h>

const size_t bufferSize = 1000;
const size_t fontSize = 16;
#define DRAWING_DRAW_TEXT_BLOB(canvas, x, y, ...)                                                           \
    do {                                                                                                    \
        char* buf = new char[bufferSize];                                                                   \
        int ret = snprintf(buf, bufferSize, __VA_ARGS__);                                                   \
        if (ret == 0) {                                                                                     \
            delete[] buf;                                                                                   \
            break;                                                                                          \
        }                                                                                                   \
        OH_Drawing_Font* font = OH_Drawing_FontCreate();                                                    \
        OH_Drawing_FontSetTextSize(font, fontSize);                                                         \
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(buf, font, TEXT_ENCODING_UTF8); \
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, x, y);                                              \
        delete[] buf;                                                                                       \
        OH_Drawing_TextBlobDestroy(textBlob);                                                               \
        OH_Drawing_FontDestroy(font);                                                                       \
    } while (0)

#endif // COMMON_LOG_DRAW_H
