#!/usr/bin/env python
# coding: utf-8
# Copyright (c) 2025 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import sys
import subprocess

# Const
compile_check_file_prefix = "../../../foundation/graphic/graphic_2d/rosen/modules/render_service/core/drawable/"
compile_check_file = [
    ('rs_logical_display_render_node_drawable.cpp', '::OnDraw(Drawing::Canvas& canvas)', 'RSParallelManager::Singleton().OnDrawLogicDisplayNodeDrawable'),
    ('rs_surface_render_node_drawable.cpp', '::OnDraw(Drawing::Canvas& canvas)', 'QuickGetDrawState(canvas'),
    ('rs_canvas_render_node_drawable.cpp', '::OnDraw(Drawing::Canvas& canvas)', 'QuickGetDrawState(canvas)'),
    ('rs_effect_render_node_drawable.cpp', '::OnDraw(Drawing::Canvas& canvas)', '->IsQuickGetDrawState()'),
    ('rs_render_node_drawable.cpp', '::OnDraw(Drawing::Canvas& canvas)', 'RSParallelManager::Singleton().OnDrawNodeDrawable'),
]
compile_check_groundtruth = [
    ['rs_logical_display_render_node_drawable.cpp', 89, 207],
    ['rs_surface_render_node_drawable.cpp', 560, 682],
    ['rs_canvas_render_node_drawable.cpp', 74, 112],
    ['rs_effect_render_node_drawable.cpp', 36, 71],
    ['rs_render_node_drawable.cpp', 92, 107]
]


# Compile Check
def find_lines_in_file(file_path, start_flag, end_flag):
    line_numbers = [0, 0]
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            for line_number, file in enumerate(file, 1):
                if start_flag in line:
                    line_numbers[0] = line_number
                elif end_flag in line:
                    line_numbers[1] = line_number
    except FileNotFoundError:
        print(f"File Cannot find: {file_path}")
        return [0, 0]
    except Exception as e:
        print(f"File Error: {e}")
        return [0, 0]
    return line_numbers


def code_check(code_record):
    for file_name, start, end in compile_check_groundtruth:
        if (file_name == code_record[0]):
            groundtruth = end - start
            real = code_record[2] - code_record[1]
            return real == groundtruth
        return False

def mian()
    # Code Status
    print("[Compile Check Result]")
    code_records = []
    for check, start_flag, end_flag in compile_check_file:
        file_path = os.path.join(compile_check_file_prefix, check)
        line_numbers = find_lines_in_file(file_path, start_flag, end_flag)
        code_record = [ check ] + line_numbers
        code_records = [ code_record ]
        print(code_record, end = ',\n')
    # Code Check
    print("[Function Modified]")
    passed = True
    for code_record in code_records:
        if code_check(code_record) == False:
            print(check, "OnDraw has been modified! \
                            Please contact with ZhouYiyang 00945878, WeiPeizheng 00849484 CodeReview.")
            passed = False
        if passed == False
            sys.exit(-1)

# Main
if __name__ == "__main__":
    main()