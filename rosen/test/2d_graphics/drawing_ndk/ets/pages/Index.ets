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

import XComponentContext from "../interface/XComponentContext";
import image from '@ohos.multimedia.image';
import {MyNodeController} from './myxnode';
import {Global} from './global';
import componentSnapshot from '@ohos.arkui.componentSnapshot';
const TAG = '[DrawingTest]';

@Entry
@Component
struct Index {
  private xComponentContext: XComponentContext | undefined = undefined;
  @State testType: string = 'function';
  @State drawingType: string = 'cpu-xcomponent';
  @State caseNameStr: string  = 'aarectmodes';
  @State information: string = '';
  imagePackerApi: image.ImagePacker = image.createImagePacker();
  caseNameStrBefore: string  = '';
  scroller: Scroller = new Scroller();
  caseCountStr: string = '1000';
  testAll: boolean = false;
  isXnode: boolean = true;
  myNodeController = new MyNodeController();

  build() {
    Column() {
      if (this.drawingType == 'gpu-xnode') {
        Row() {
          NodeContainer(this.myNodeController)
            .width('100%')
            .height('100%')
            .backgroundColor(Color.White)
            .onAppear(()=>{
              console.warn(TAG, 'MyNodeController onAppear');
            })
            .id('myNodeController')
        }.height('65%')
      } else {
        Row() {
          XComponent({ id: 'xcomponentId', type: 'surface', libraryname: 'entry' })
            .onLoad((xComponentContext) => {
              this.xComponentContext = xComponentContext as XComponentContext;
            }).width('100%') // Multiples of 64
            .id('myXcomponent').backgroundColor(Color.White)
        }.height('65%')
      }

      Row() {
        Select([
          { value: 'function' },
          { value: 'performance' },
          { value: 'stability' },
        ])
          .id('testType')
          .selected(0)
          .value(this.testType)
          .padding({left:5})
          .border({width:1})
          .width('40%')
          .onSelect((index: number, value: string)=>{
            this.testType = value;
            console.log(TAG, "select: " + this.testType);
          })

        if (this.testType == 'stability'){
          Select([
            { value: 'cpu-xcomponent' },
          ])
            .id('drawingType')
            .selected(0)
            .value(this.drawingType)
            .padding({left:5})
            .border({width:1})
            .width('40%')
        } else {
          Select([
            { value: 'cpu-xcomponent' },
            { value: 'gpu-xcomponent' },
            { value: 'gpu-xnode' },
          ])
            .id('drawingType')
            .selected(0)
            .value(this.drawingType)
            .padding({left:5})
            .border({width:1})
            .width('40%')
            .onSelect((index: number, value: string)=>{
              this.drawingType = value;
              console.log(TAG, "select: " + this.drawingType);
            })
        }
      }
      .height('7%')
      Row() {
        Text('TestCase')
          .fontSize(14)
          .textAlign(TextAlign.End)
          .height(38)
          .margin({right:5})

        Column() {
          TextInput({ placeholder: 'ex:drawrect', text: this.caseNameStr})
            .id('caseName')
            .enterKeyType(EnterKeyType.Send)
            .placeholderFont({size: 15})
            .height(38)
            .width('60%')
            .border({width: 1, radius:10, color: '#88000000'})
            .onChange((value) => {
              if (value != "") {
                this.caseNameStr = value
              }
            })

        }
        Select([

        ])
          .id('caseSelect')
          .selected(0)
          .padding({left:5})
          .border({width:1})
          .width('20%')
          .onSelect((index: number, value: string)=>{
            this.caseNameStr = value
            console.log(TAG, "select: " + this.caseNameStr);
          })
      }
      .height('7%')
      .width('80%')
      .margin({left:5})

      if(this.testType == 'performance' || this.testType == 'stability') {
        Row() {
          Text('TestCount')
            .fontSize(14)
            .textAlign(TextAlign.End)
            .height(38)
            .margin({right:5})
          Column() {
            TextInput({ placeholder: '10', text:this.caseCountStr})
              .id('testCount')
              .enterKeyType(EnterKeyType.Send)
              .placeholderFont({size: 15})
              .height(38)
              .width('50%')
              .border({width: 1, radius:10, color: '#88000000'})
              .onChange((value) => {
                if (value != "") {
                  this.caseCountStr = value
                }
              })
          }
          Button("Clear")
            .fontSize('16fp')
            .id('Clear')
            .onClick(() => {
              console.log(TAG, "Clear click");
              this.myNodeController.Clear();
              this.caseNameStr = ""
            })
            .width('30%')
            .shadow(ShadowStyle.OUTER_DEFAULT_LG)
        }
        .width('80%')
        .height('7%')
      } else if (this.testType == 'function'){
        if (this.drawingType == 'gpu-xnode') {
          Row(){
            Button("drawNext")
              .id('drawNext')
              .fontSize('16fp')
              .fontWeight(500)
              .onClick(() => {
                this.TestNextFunctionalGpuXnode();
              })
              .width('80%')
              .shadow(ShadowStyle.OUTER_DEFAULT_LG)
          }
          .height('7%')
        } else {
          Row(){
            Button("TestAll")
              .id('testAll')
              .fontSize('16fp')
              .fontWeight(500)
              .onClick(() => {
                if (this.drawingType == 'cpu-xcomponent') {
                  this.TestFunctionalCpuAll();
                } else if (this.drawingType == 'gpu-xcomponent'){
                  this.TestFunctionalGpuAll();
                }
              })
              .width('80%')
              .shadow(ShadowStyle.OUTER_DEFAULT_LG)
          }
          .height('7%')
        }
      }
      Row() {
        Button("draw")
          .id('draw')
          .fontSize('16fp')
          .fontWeight(500)
          .onClick(() => {
            console.log(TAG, "Draw click");
            this.Draw();
          })
          .width('80%')
          .shadow(ShadowStyle.OUTER_DEFAULT_LG)
      }
      .height('7%')
      Row() {
        Text(this.information)
          .fontSize(20)
          .textAlign(TextAlign.Center)
          .id('infoText')
      }
      .height('7%')
    }
  }

  LogMsg(msg: string): void {
    this.information = msg;
    console.log(TAG, msg);
  }

  async Draw():Promise<void> {
    console.log(TAG, 'Drawing test start!');
    if (this.testType == 'function') {
      if (this.drawingType == 'cpu-xcomponent') {
        return this.TestFunctionalCpu(this.caseNameStr);
      } else if (this.drawingType == 'gpu-xcomponent') {
        return this.TestFunctionalGpu(this.caseNameStr);
      } else if (this.drawingType == 'gpu-xnode') {
        return this.TestFunctionalXnode(this.caseNameStr);
      }
    }
    if (this.testType == 'performance') {
      if (this.drawingType == 'cpu-xcomponent') {
        return this.TestPerformanceCpu();
      } else if (this.drawingType == 'gpu-xnode') {
        let testCount = Number.parseInt(this.caseCountStr, 10);
        this.myNodeController.SetPrintCallback((str:string)=>{
          this.LogMsg(str);
        })
        return this.myNodeController.TestPerformance(this.caseNameStr, testCount);
      } else {
        return this.TestPerformanceGpu();
      }
    }
    if (this.testType == 'stability') {
      return this.TestStabilityCpu(this.caseNameStr);
    }
  }

  sleep(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

  async TestFunctionalCpuAll() {
    console.log(TAG, 'TestFunctionalCpuAll start');
    if (this.xComponentContext) {
      let str = this.xComponentContext.getTestNames();
      let names = str.split(",");
      for (let name of names) {
        this.LogMsg('name:' + name);
        this.TestFunctionalCpu(name);
      }
    }
    this.LogMsg('TestFunctionalCpuAll end');
  }

  async TestFunctionalGpuAll() {
    console.log(TAG, 'TestFunctionalGpuAll start');
    if (this.xComponentContext) {
      let str = this.xComponentContext.getTestNames();
      let names = str.split(",");
      for (let name of names) {
        this.TestFunctionalGpu(name);
      }
    }
    this.LogMsg('TestFunctionalGpuAll end');
  }

  async TestNextFunctionalGpuXnode() {
    console.log(TAG, 'TestNextFunctionalGpuXnode start');
    if (this.xComponentContext) {
      let str = this.myNodeController.GetAllFunctionalNames();
      let names = str.split(",");
      let index = names.indexOf(this.caseNameStr);
      if (index == names.length - 1) {
        this.TestFunctionalXnode(names[0]);
      } else {
        this.TestFunctionalXnode(names[index + 1]);
      }
    }
    this.LogMsg('TestNextFunctionalGpuXnode end');
  }

  async TestFunctionalXnode(str: string) {
    console.log(TAG, 'TestFunctionalXnode start');
    this.myNodeController.Clear();
    await this.sleep(100);
    this.myNodeController.TestFunctional(str);
    this.caseNameStr = str;
    await this.sleep(1500);
    const context: Context = getContext(this);
    componentSnapshot.get("myNodeController",
      (error:Error, pixmap: image.PixelMap) => {
        if (error) {
          console.log("error:", JSON.stringify(error))
          return;
        }
        Global.savePixmap(context.cacheDir, str, pixmap);
      })
    this.LogMsg('TestFunctionalXnode end');
  }

  async TestFunctionalCpu(str: string) {
    console.log(TAG, 'TestFunctionalCpu start');
    if (this.xComponentContext) {
      this.xComponentContext.testFunctionCpu(str);
    }
    this.LogMsg('TestFunctionalCpu end');
  }

  async TestStabilityCpu(str: string) {
    console.log(TAG, 'TestStabilityCpu start');
    if (this.xComponentContext) {
      let testCount = Number.parseInt(this.caseCountStr, 10);
      this.xComponentContext.setTestCount(testCount);
      this.xComponentContext.testStabilityCpu(str);
    }
  }

  async TestFunctionalGpu(str: string) {
    console.log(TAG, 'TestFunctionalGpu start');
    if (this.xComponentContext) {
      this.xComponentContext.testFunctionGpu(str);
    }
    this.LogMsg('TestFunctionalGpu end');
  }

  async TestPerformanceCpu() {
    console.log(TAG, 'TestPerformanceCpu start');

    if (this.xComponentContext) {
      let testCount = Number.parseInt(this.caseCountStr, 10);
      this.xComponentContext.setTestCount(testCount);
      this.xComponentContext.testPerformanceCpu(this.caseNameStr);
      let time = this.xComponentContext.getTime();
      this.LogMsg('Count: ' + testCount + ', used: ' + time.toString() + 'ms');
    }
    console.log(TAG, 'TestPerformanceCpu end');
  }

  async TestPerformanceGpu() {
    console.log(TAG, 'TestPerformanceGpu start');

    let testCount = Number.parseInt(this.caseCountStr, 10);
    if (this.xComponentContext) {
      this.xComponentContext.setTestCount(testCount);
      this.xComponentContext.testPerformanceGpu(this.caseNameStr);
      let time = this.xComponentContext.getTime();
      this.LogMsg('Count: ' + testCount + ', used: ' + time.toString() + 'ms');
    }
    console.log(TAG, 'TestPerformanceGpu end');
  }
}
