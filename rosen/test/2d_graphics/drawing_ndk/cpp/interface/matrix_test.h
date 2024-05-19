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

#ifndef INTERFACE_MATRIX_TEST_H
#define INTERFACE_MATRIX_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class MatrixReset : public TestBase {
public:
    MatrixReset(int type) : TestBase(type)
    {
        fileName_ = "MatrixReset";
    }
    ~MatrixReset() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class MatrixConcat : public TestBase {
public:
    MatrixConcat(int type) : TestBase(type)
    {
        fileName_ = "MatrixConcat";
    }
    ~MatrixConcat() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class MatrixInvert : public TestBase {
public:
    MatrixInvert(int type) : TestBase(type)
    {
        fileName_ = "MatrixInvert";
    }
    ~MatrixInvert() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class MatrixSetPolyToPoly : public TestBase {
public:
    MatrixSetPolyToPoly(int type) : TestBase(type)
    {
        fileName_ = "MatrixSetPolyToPoly";
    }
    ~MatrixSetPolyToPoly() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class MatrixPreRotate : public TestBase {
public:
    MatrixPreRotate(int type) : TestBase(type)
    {
        fileName_ = "MatrixPreRotate";
    };
    ~MatrixPreRotate() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class MatrixPostScale : public TestBase {
public:
    MatrixPostScale(int type) : TestBase(type)
    {
        fileName_ = "MatrixPostScale";
    };
    ~MatrixPostScale() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class MatrixPostTranslate : public TestBase {
public:
    MatrixPostTranslate(int type) : TestBase(type)
    {
        fileName_ = "MatrixPostTranslate";
    };
    ~MatrixPostTranslate() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class MatrixIsEqual : public TestBase {
public:
    MatrixIsEqual(int type) : TestBase(type)
    {
        fileName_ = "MatrixIsEqual";
    };
    ~MatrixIsEqual() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // INTERFACE_MATRIX_TEST_H