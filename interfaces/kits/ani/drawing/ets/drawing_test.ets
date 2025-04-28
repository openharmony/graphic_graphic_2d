/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

import drawing from '@ohos.graphics.drawing';

function testPen() {
    console.log(`testPen start`);
    let pen : drawing.Pen = new drawing.Pen();
    pen.setAlpha(157);
    console.log(`testPen pen.getAlpha = ${pen.getAlpha()}`);
    pen.reset();
    console.log(`testPen pen.getAlpha = ${pen.getAlpha()}`);
    console.log(`testPen end`);
}

function testBrush() {
    console.log(`testBrush start`);
    let brush : drawing.Brush = new drawing.Brush();
    brush.setAlpha(157);
    console.log(`testBrush brush.getAlpha = ${brush.getAlpha()}`);
    brush.reset();
    console.log(`testBrush brush.getAlpha = ${brush.getAlpha()}`);
    console.log(`testBrush end`);
}

function main() {
    testPen();
    testBrush();
}