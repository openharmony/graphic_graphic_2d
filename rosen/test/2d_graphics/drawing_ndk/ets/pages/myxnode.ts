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

import drawing from "@ohos.graphics.drawing";
import { NodeController, FrameNode, RenderNode, DrawContext, Size } from "@ohos.arkui.node"
import nativeXNode from "libmyxnode.so";
import {PrintCallback} from "./printcallback";

const TAG = '[DrawingTest]';
let printCallback: PrintCallback;

export class MyRenderNode extends RenderNode {
  testType: string = 'function';
  drawingType: string = 'cpu';
  caseNameStr: string  = 'drawrect';
  testCount: number = 0;
  clear:boolean = false;

  async draw(context: DrawContext) {
    if (this.clear) {
      printCallback('Clear XNode');
      return;
    }

    console.log(TAG, 'MyRenderNode draw');
    if (this.testType == 'function') {
      console.log(TAG, 'MyRenderNode draw function');
      this.TestFunctional(context);
    } else {
      console.log(TAG, 'MyRenderNode draw perf');
      this.TestPerformance(context);
    }
  }

  async TestFunctional(context: DrawContext) {
    console.info(TAG, 'MyRenderNode TestFunctional', this.caseNameStr);
    nativeXNode.TestFunctional(context, this.caseNameStr);
    printCallback('XNode TestFunctional ok');
  }

  async TestPerformance(context: DrawContext) {
    console.info(TAG, 'MyRenderNode TestPerformance', this.caseNameStr, this.testCount);
    let performanceTime = nativeXNode.TestPerformance(context, this.caseNameStr, this.testCount);
    printCallback('Count: ' + this.testCount.toString() + ', used: ' + performanceTime.toString() + 'ms');
  }
}

export class MyNodeController extends NodeController {
  private rootNode: FrameNode | null = null;
  myRenderNode = new MyRenderNode();

  makeNode(uiContext: UIContext): FrameNode {
    console.info(TAG, 'MyNodeController makeNode');
    this.rootNode = new FrameNode(uiContext);
    if (this.rootNode === null) {
      console.error(TAG, 'failed to new FrameNode')
      return this.rootNode
    }

    const renderNode = this.rootNode.getRenderNode();
    if (renderNode !== null) {
      this.myRenderNode.backgroundColor = 0xffffffff;
      this.myRenderNode.frame = { x: 0, y: 0, width: 480, height: 480 }; //2:3 = 480:720
      this.myRenderNode.pivot = { x: 0.2, y: 0.8 }
      this.myRenderNode.scale = { x: 1, y: 1 }
      renderNode.appendChild(this.myRenderNode);
      console.info(TAG, 'MyNodeController appendChild end');
    }

    console.info(TAG, 'MyNodeController makeNode end');
    return this.rootNode;
  }
  TestFunctional(caseName: string) {
    console.info(TAG, "TestFunctional");
    this.myRenderNode.clear = false;
    this.myRenderNode.testType = 'function';
    this.myRenderNode.drawingType = 'cpu';
    this.myRenderNode.caseNameStr = caseName;
    this.myRenderNode.invalidate();
  }

  TestPerformance(caseName: string, count: number) {
    console.info(TAG, "TestPerformance");
    this.myRenderNode.clear = false;
    this.myRenderNode.testType = 'performance';
    this.myRenderNode.drawingType = 'cpu';
    this.myRenderNode.caseNameStr = caseName;
    this.myRenderNode.testCount = count;
    this.myRenderNode.invalidate();
  }

  Clear() {
    console.info(TAG, "Clear");
    this.myRenderNode.clear = true;
    this.myRenderNode.invalidate();
  }
  SetPrintCallback(callback: PrintCallback){
    printCallback = callback;
  }
}
