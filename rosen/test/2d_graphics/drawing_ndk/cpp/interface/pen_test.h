//
// Created on 2024/5/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#ifndef INTERFACE_PEN_TEST_H
#define INTERFACE_PEN_TEST_H
#include <bits/alltypes.h>
#include <multimedia/player_framework/native_avscreen_capture_base.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_sampling_options.h>

#include "napi/native_api.h"
#include "test_base.h"
#include "test_common.h"

class PenReset : public TestBase {
public:
    PenReset(int type) : TestBase(type)
    {
        fileName_ = "PenReset";
    };
    ~PenReset() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // INTERFACE_PEN_TEST_H
