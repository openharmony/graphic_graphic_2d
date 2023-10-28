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
import { SphereGraph } from "../utils/SphereGraph";
import { Matrix4 } from "../utils/Matrix";
import { Images } from '../utils/Images';

const VSCODE = `#version 300 es
        in vec4 a_Position;
        in vec2 a_TexCoord;
        uniform mat4 u_MvpMatrix;
        uniform mat4 u_ModelMatrix;
        in vec4 a_Color;
        out vec4 v_Color;
        out vec2 v_TexCoord;
        void main() {
            gl_Position = u_MvpMatrix * u_ModelMatrix * a_Position;
            v_Color = vec4(1.0,0.0,0.0,1.0);
            gl_PointSize = 10.0;
            v_TexCoord = a_TexCoord;
        }
    `;
const FSCODE = `#version 300 es
        precision mediump float;
        uniform sampler2D u_Sampler;
        in vec4 v_Color;
        in vec2 v_TexCoord;
        out vec4 fragColor;
        void main() {
            fragColor = texture(u_Sampler,v_TexCoord);;
        }
    `;
let indices;
let modelMatrix = new Matrix4();
let angle = 0;
let lastTime = Date.now();

export async function panorama2_demo(gl) {
    let program = createProgram(gl, VSCODE, FSCODE, ["a_Position", "a_TexCoord"], ["u_MvpMatrix", "u_ModelMatrix","u_Sampler"]);
    let sphere = new SphereGraph(1, 16, 12);
    let arr = new Float32Array(sphere.vertices);
    indices = new Uint8Array(sphere.indices);
    let FSIZE = arr.BYTES_PER_ELEMENT;
    let buffer = gl.createBuffer(); //创建缓冲区
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer); //绑定缓冲区
    gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STATIC_DRAW); //将数据写入缓冲区对象
    gl.vertexAttribPointer(program.a_Position, 3, gl.FLOAT, false, FSIZE * 3, 0); //将缓冲区对象分配给一个attribute变量
    gl.enableVertexAttribArray(program.a_Position); //开启attribute变量(连接a_Position变量与分配给它的缓冲区对象
    let texBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, texBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sphere.textureCoord), gl.STATIC_DRAW);
    gl.vertexAttribPointer(program.a_TexCoord, 2, gl.FLOAT, false, FSIZE * 2, 0); //将缓冲区对象分配给一个attribute变量
    gl.enableVertexAttribArray(program.a_TexCoord);
    let indexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);
    gl.clearColor(0.92, 0.92, 0.92, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.POLYGON_OFFSET_FILL);
    gl.polygonOffset(1.0, 1.0);
    let image = await loadImage(Images.VR_IMG);
    let texture = gl.createTexture();
    if (texture === null) {
        return;
    }
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
    gl.generateMipmap(gl.TEXTURE_2D);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.uniform1i(program.u_Sampler, 0);
    let mvpMatrix = new Matrix4();
    mvpMatrix.setPerspective(80, gl.width / gl.height, 0.1, 1000);
    mvpMatrix.lookAt(0.01, 0, 0, 0, 0, 0, 0, 1, 0);
    gl.uniformMatrix4fv(program.u_MvpMatrix, false, mvpMatrix.getElements());
    draw(program, gl);
}

function anim(angle) {
    let now = Date.now();
    let offset = now - lastTime;
    lastTime = now;
    let newAngle = angle + (1 * offset) / 1000.0;
    return newAngle % 360;
}

function draw(program, gl) {
    angle = anim(angle);
    modelMatrix.setRotate(angle, 0, 1, 0);
    gl.uniformMatrix4fv(program.u_ModelMatrix, false, modelMatrix.getElements());
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
    gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_BYTE, 0);
    gl.flush();
    if (!gl.closed) {
        requestAnimationFrame(() => draw(program, gl));
    }
};
