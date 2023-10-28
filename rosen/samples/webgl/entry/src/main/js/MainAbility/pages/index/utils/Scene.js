/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

import { Camera } from "./Camera.js";
export class Scene {
    gl;
    _camera = new Camera();
    constructor(gl) {
        this.gl = gl;
    }
    viewport(x, y, width, height) {
        this.gl.viewport(x, y, width, height);
    }
    objs = [];
    addShape(shape) {
        this.objs.push(shape);
    }
    camera() {
        return this._camera;
    }
    render() {
        this.clear();
        this.objs.forEach(obj => {
            this.gl.useProgram(obj.program);
            obj.init(this.gl, this.camera());
            obj.render(this.gl.TRIANGLES, this.gl);
        });
        this.gl.flush();
    }
    renderPoint() {
        this.objs.forEach(obj => obj.renderPoint(this.gl));
    }
    clear(r = 0, g = 0, b = 0, a = 1) {
        let gl = this.gl;
        gl.clearColor(r, g, b, a);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
        gl.enable(gl.POLYGON_OFFSET_FILL);
        gl.polygonOffset(1.0, 1.0);
    }
    setRender(render) {
        let fun = () => {
            render();
            this.render();
            if(!this.gl.closed){
                requestAnimationFrame(fun);
            }
        };
        fun();
    }
}
