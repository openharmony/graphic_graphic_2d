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

import { createProgram, loadImage } from "../utils/extension";
import { Images } from '../utils/Images';

const VS_CODE = `
        attribute mat4 a_Matrix;
        attribute vec4 a_Position;
        attribute vec2 a_TexCoord;
        varying vec2 v_TexCoord;
        void main(){
            gl_Position =  a_Position;
            v_TexCoord = a_TexCoord;
        }
    `;
const FS_CODE = `
        precision mediump float;
        uniform sampler2D u_Sampler;
        varying vec2 v_TexCoord;
        void main(){
            gl_FragColor = texture2D(u_Sampler,v_TexCoord);
        }
    `;
let side = 1.0;
let maxS = side;
let maxT = side;
let arr = new Float32Array([
    -0.5, 0.5, 0.0, maxT,
    -0.5, -0.5, 0.0, 0.0,
    0.5, 0.5, maxS, maxT,
    0.5, -0.5, maxS, 0.0
]);

export async function texture_2d_demo() {
    let gl = globalThis.gl;
    let program = createProgram(globalThis.gl, VS_CODE, FS_CODE, ["a_Matrix", "a_Position", "a_TexCoord"], ["u_Sampler"]);
    let buffer = gl.createBuffer();
    if (buffer === null) {
        gl.deleteBuffer(buffer);
        return;
    }
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STATIC_DRAW);
    gl.vertexAttribPointer(program.a_Position, 2, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 4, 0);
    gl.enableVertexAttribArray(program.a_Position);
    gl.vertexAttribPointer(program.a_TexCoord, 2, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 4, Float32Array.BYTES_PER_ELEMENT * 2);
    gl.enableVertexAttribArray(program.a_TexCoord);
    let image = await loadImage(Images.MF_IMG);
    console.log("webgltest", image);
    let texture = gl.createTexture();
    if (texture === null) {
        gl.deleteTexture(texture);
        return;
    }
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
    gl.generateMipmap(gl.TEXTURE_2D);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.MIRRORED_REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.MIRRORED_REPEAT);
    gl.uniform1i(program.u_Sampler, 0);
    gl.clearColor(0.92, 0.92, 0.92, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
    gl.flush();
}
