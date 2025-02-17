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

#ifndef STABILITY_TEXT_BLOB_TEST_H
#define STABILITY_TEXT_BLOB_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    // setting function
    SETTING_FUNCTION_TEXT_BLOB_BUILDER_CREATE = 0,
    SETTING_FUNCTION_TEXT_BLOB_CREATE_FROM_POS_TEXT,
    SETTING_FUNCTION_TEXT_BLOB_CREATE_FROM_STRING,
    SETTING_FUNCTION_TEXT_BLOB_GET_BOUNDS,
    SETTING_FUNCTION_TEXT_BLOB_UNIQUE_ID,
    SETTING_FUNCTION_TEXT_BLOB_BUILDER_ALLOC_RUN_POS,
    SETTING_FUNCTION_TEXT_BLOB_BUILDER_MAKE,
} SetttingFunctionTextBlob;

// TextBlobBuilderCreate-TextBlobBuilderMake-TextBlobDestroy-TextBlobBuilderDestroy循环
class StabilityTextBlobBuilderCreate : public TestBase {
public:
    StabilityTextBlobBuilderCreate()
    {
        fileName_ = "StabilityTextBlobBuilderCreate";
    }
    ~StabilityTextBlobBuilderCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// TextBlobCreateFromText-TextBlobDestroy循环
class StabilityTextBlobCreateFromText : public TestBase {
public:
    StabilityTextBlobCreateFromText()
    {
        fileName_ = "StabilityTextBlobCreateFromText";
    }
    ~StabilityTextBlobCreateFromText() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// TextBlobCreateFromString-TextBlobDestroy循环
class StabilityTextBlobCreateFromString : public TestBase {
public:
    StabilityTextBlobCreateFromString()
    {
        fileName_ = "StabilityTextBlobCreateFromString";
    }
    ~StabilityTextBlobCreateFromString() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// TextBlobCreateFromPosText-TextBlobDestroy循环
class StabilityTextBlobCreateFromPosText : public TestBase {
public:
    StabilityTextBlobCreateFromPosText()
    {
        fileName_ = "StabilityTextBlobCreateFromPosText";
    }
    ~StabilityTextBlobCreateFromPosText() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// TextBlobBuilderAllocRunPos-TextBlobBuilderDestroy循环
class StabilityTextBlobBuilderAllocRunPos : public TestBase {
public:
    StabilityTextBlobBuilderAllocRunPos()
    {
        fileName_ = "StabilityTextBlobBuilderAllocRunPos";
    }
    ~StabilityTextBlobBuilderAllocRunPos() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// TextBlobCreate-相关配置接口全调用-TextBlobDestroy循环
// OH_Drawing_TextBlobGetBounds,
// OH_Drawing_TextBlobUniqueID,
class StabilityTextBlobRandomFunc : public TestBase {
public:
    StabilityTextBlobRandomFunc()
    {
        fileName_ = "StabilityTextBlobRandomFunc";
    }
    ~StabilityTextBlobRandomFunc() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

#endif // STABILITY_TEXT_BLOB_TEST_H
