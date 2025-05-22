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

import drawing from './@ohos.graphics.drawing';
import common2D from '@ohos.graphics.common2D';
import image from '@ohos.multimedia.image';

function testPen() {
    console.log(`testPen start`);
    let pen : drawing.Pen = new drawing.Pen();
    pen.setAlpha(157);
    console.log(`testPen pen.getAlpha = ${pen.getAlpha()}`);
    pen.reset();
    console.log(`testPen pen.getAlpha = ${pen.getAlpha()}`);

    pen.setBlendMode(drawing.BlendMode.CLEAR)
    pen.setAlpha(179);
    let pen2 : drawing.Pen = new drawing.Pen(pen);
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

    brush.setBlendMode(drawing.BlendMode.CLEAR)
    brush.setAlpha(179);
    let brush2 : drawing.Brush = new drawing.Brush(brush);
    console.log(`testBrush brush.getAlpha = ${brush2.getAlpha()}`);
    console.log(`testBrush end`);
}

function testColorFilter() {
    console.log(`testColorFilter start`);

    let pen : drawing.Pen = new drawing.Pen();
    let brush : drawing.Brush = new drawing.Brush();
    let colorFilter1 : drawing.ColorFilter =
        drawing.ColorFilter.createBlendModeColorFilter(0x85957, drawing.BlendMode.CLEAR)
    brush.setColorFilter(colorFilter1)
    pen.setColorFilter(colorFilter1)

    let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
    let colorFilter2 : drawing.ColorFilter =
        drawing.ColorFilter.createBlendModeColorFilter(color, drawing.BlendMode.CLEAR)
    brush.setColorFilter(colorFilter2)
    pen.setColorFilter(colorFilter2)

    console.log(`testColorFilter end`);
}

function testFont() {
    console.log(`testFont start`);
    let font : drawing.Font = new drawing.Font();
    font.setSize(9);
    console.log(`testFont font.getSize = ${font.getSize()}`);
    let fontMetrics = font.getMetrics();
    if (fontMetrics == null) {
        console.error(`testFont fontMetrics == null`);
        return;
    }
    console.log(`testFont fontMetrics = ${fontMetrics}`);
    console.log(`testFont fontMetrics.top = ${fontMetrics.top}`);
    console.log(`testFont fontMetrics.ascent = ${fontMetrics.ascent}`);

    let typeface : drawing.Typeface = drawing.Typeface.makeFromFile('/system/fonts/HarmonyOS_Sans.ttf');
    if (typeface == null) {
        console.error(`testFont typeface == null`);
        return;
    };
    font.setTypeface(typeface);
    let typeface1 = font.getTypeface();
    if (typeface1 == null) {
        console.error(`testFont typeface1 == null`);
        return;
    }
    console.log(`testFont typeface1.getFamilyName = ${typeface1.getFamilyName()}`);
    console.log(`testFont end`);
}

function testTypeface() {
    console.log(`testTypeface start`);
    let typeface : drawing.Typeface = drawing.Typeface.makeFromFile('/system/fonts/HarmonyOS_Sans.ttf');
    if (typeface == null) {
        console.error(`testTypeface typeface == null`);
        return;
    }
    console.log(`testTypeface typeface.getFamilyName = ${typeface.getFamilyName()}`);
    console.log(`testTypeface end`);
}

function testErrorCode() {
    console.log(`testErrorCode start`);
    let pen : drawing.Pen = new drawing.Pen();
    try {
        pen.setAlpha(157);
        console.log(`testErrorCode pen.setAlpha(157) success`);
    } catch (e: Error) {
        console.log(`testErrorCode pen.setAlpha(157) failed, err.msg = ${e.message}`);
    }
    console.log(`testErrorCode pen.getAlpha = ${pen.getAlpha()}`);
    try {
        pen.setAlpha(-10);
        console.log(`testErrorCode pen.setAlpha(-10) success`);
    } catch (e: Error) {
        console.log(`testErrorCode pen.setAlpha(-10) failed, err.msg = ${e.message}`);
    }
    console.log(`testErrorCode pen.getAlpha = ${pen.getAlpha()}`);
    try {
        pen.setAlpha(321);
        console.log(`testErrorCode pen.setAlpha(321) success`);
    } catch (e: Error) {
        console.log(`testErrorCode pen.setAlpha(321) failed, err.msg = ${e.message}`);
    }
    console.log(`testErrorCode pen.getAlpha = ${pen.getAlpha()}`);
    console.log(`testErrorCode end`);
}

function testSaveRestore(canvas: drawing.Canvas) {
  let count: number = canvas.save();
  let saveCount: number = canvas.getSaveCount();
  console.log(`testSaveRestore save: ${count} getSaveCount: ${saveCount}`);

  let sacveLayerCount: number = canvas.saveLayer();
  console.log(`testSaveRestore sacveLayerCount: ${sacveLayerCount}`);
  canvas.restore();

  let rect: common2D.Rect = { left: 10, top: 10, right: 100, bottom: 100 };
  saveCount = canvas.saveLayer(rect);
  console.log(`testSaveRestore sacveLayerCount: ${sacveLayerCount}`);
  canvas.restore();

  let brush : drawing.Brush = new drawing.Brush();
  saveCount = canvas.saveLayer(rect, brush);
  console.log(`testSaveRestore sacveLayerCount: ${sacveLayerCount}`);
  canvas.restore();

  saveCount = canvas.saveLayer(null, null);
  saveCount = canvas.saveLayer(null, brush);
  saveCount = canvas.saveLayer(rect, null);
  console.log(`testSaveRestore sacveLayerCount: ${sacveLayerCount}`);
  canvas.restore();
  canvas.restore();
  canvas.restore();

  canvas.restore();
  saveCount = canvas.getSaveCount();
  console.log(`testSaveRestore getSaveCount: ${saveCount}`);
}

function testDrawImage(canvas: drawing.Canvas) {
  const opts: image.InitializationOptions = {
    size: { width: 100, height: 100 },
    editable: true,
    pixelFormat: image.PixelMapFormat.BGRA_8888,
  };
  let pixelMap: image.PixelMap = image.createPixelMapSync(opts);
  let rect: common2D.Rect = { left: 50, top: 50, right: 150, bottom: 150 };
  canvas.drawImageRect(pixelMap, rect)
  let samplingOptions = new drawing.SamplingOptions(drawing.FilterMode.FILTER_MODE_LINEAR);
  canvas.drawImageRect(pixelMap, rect, samplingOptions)

  let verts: number[] = [100, 100, 200, 100, 150, 200, 200, 200];
  let colors: number[] = [0x00ff0088, 0x00ff0088, 0x00ff0088, 0x00ff0088];
  canvas.drawPixelMapMesh(pixelMap, 1, 1, verts, 0, colors, 0);
}

function testCanvas() {
  console.log(`testCanvas start`);
  const opts: image.InitializationOptions = {
    size: { width: 480, height: 360 },
    editable: true,
    pixelFormat: image.PixelMapFormat.BGRA_8888,
  };
  let pixelMap: image.PixelMap = image.createPixelMapSync(opts);
  if (pixelMap == undefined) {
    console.log('Create image.createPixelMapSync failed');
    return;
  }
  console.log('Create image.createPixelMapSync success');

  let canvas: drawing.Canvas = new drawing.Canvas(pixelMap)
  testSaveRestore(canvas)

  let pen: drawing.Pen = new drawing.Pen()
  let brush : drawing.Brush = new drawing.Brush();
  canvas.attachPen(pen)
  canvas.attachBrush(brush)

  let rect: common2D.Rect = { left: 10, top: 10, right: 100, bottom: 100 };
  canvas.drawRect(rect)
  canvas.drawRect(100, 100, 120, 150)

  canvas.rotate(60, 10, 10)

  testDrawImage(canvas)

  canvas.detachPen()
  canvas.detachBrush()
  console.log(`testCanvas end`);
}

function testLattice() {
    console.log(`testLattice start`);
    let xDivs : number[] = [1, 2, 4];
    let yDivs : number[] = [1, 2, 4];
    let fBounds: common2D.Rect = { left: 10, top: 10, right: 100, bottom: 100 };
    let fRectTypes : drawing.RectType[] = [
        drawing.RectType.DEFAULT, drawing.RectType.DEFAULT,
        drawing.RectType.DEFAULT, drawing.RectType.DEFAULT,
        drawing.RectType.TRANSPARENT, drawing.RectType.TRANSPARENT,
        drawing.RectType.TRANSPARENT, drawing.RectType.TRANSPARENT,
        drawing.RectType.FIXEDCOLOR, drawing.RectType.FIXEDCOLOR,
        drawing.RectType.FIXEDCOLOR, drawing.RectType.FIXEDCOLOR,
        drawing.RectType.TRANSPARENT, drawing.RectType.TRANSPARENT,
        drawing.RectType.TRANSPARENT, drawing.RectType.TRANSPARENT ]
    let colorArray : number[] = [0xffffff,0x444444,0x999999,0xffffff,0x444444,0x999999,0xffffff,0x444444,
        0x999999,0x444444,0x999999,0xffffff,0x444444,0x999999,0xffffff,0x444444];
    let color : common2D.Color = { alpha: 0xFF, red: 0x55, green: 0xFF, blue: 0x44 };
    let colorArray1 :common2D.Color[] = [
        color,color,color,color,color,color,color,color,color,color,color,color,color,color,color,color]
    let lattice1 : drawing.Lattice = drawing.Lattice.createImageLattice(
        xDivs, yDivs, 3, 3, fBounds, fRectTypes, colorArray);
    let lattice2 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3);
    let lattice3 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, fBounds);
    let lattice4 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, fBounds, fRectTypes);
    let lattice5 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, null)
    let lattice6 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, null, null);
    let lattice7 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, null, null, null);
    let lattice8 : drawing.Lattice = drawing.Lattice.createImageLattice(
        xDivs, yDivs, 3, 3, null, fRectTypes, colorArray);
    let lattice9 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, fBounds, null, colorArray);
    let lattice10 : drawing.Lattice = drawing.Lattice.createImageLattice(
        xDivs, yDivs, 3, 3, fBounds, fRectTypes, null);
    let lattice11 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, null, null, colorArray);
    let lattice12 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, fBounds, null, null);
    let lattice13 : drawing.Lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3, null, fRectTypes, null);
    let lattice14 : drawing.Lattice = drawing.Lattice.createImageLattice(
        xDivs, yDivs, 3, 3, fBounds, fRectTypes, colorArray1);
    console.log(`testLattice end`);
}

function main() {
    testPen();
    testBrush();
    testColorFilter();
    testFont();
    testTypeface();
    testErrorCode();
    testCanvas();
    testLattice();
}