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

#ifndef TEXT_BLOB_TEST_H
#define TEXT_BLOB_TEST_H
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"

class TextBlobBuilderCreate : public TestBase {
public:
    explicit TextBlobBuilderCreate(int type) : TestBase(type)
    {
        fileName_ = "TextBlobBuilderCreate";
    };
    ~TextBlobBuilderCreate() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 614 ms 1000次
};

class TextBlobCreateFromText : public TestBase {
public:
    explicit TextBlobCreateFromText(int type) : TestBase(type)
    {
        fileName_ = "TextBlobCreateFromText";
    };
    ~TextBlobCreateFromText() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 440 ms 100次
};

class TextBlobCreateFromPosText : public TestBase {
public:
    explicit TextBlobCreateFromPosText(int type) : TestBase(type)
    {
        fileName_ = "TextBlobCreateFromPosText";
    };
    ~TextBlobCreateFromPosText() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 230 ms 100次
};

class TextBlobCreateFromString : public TestBase {
public:
    explicit TextBlobCreateFromString(int type) : TestBase(type)
    {
        fileName_ = "TextBlobCreateFromString";
    };
    ~TextBlobCreateFromString() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 486 ms 100次
};

class TextBlobGetBounds : public TestBase {
public:
    explicit TextBlobGetBounds(int type) : TestBase(type)
    {
        fileName_ = "TextBlobGetBounds";
    };
    ~TextBlobGetBounds() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 365 ms 100次
};
#endif // TEXT_BLOB_TEST_H