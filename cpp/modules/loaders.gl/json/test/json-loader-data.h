// Copyright (c) 2020 Unfolded, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

static auto ndjsonDataSimple = R"JSON(
{"@@type":"Deck","description":"Test","layers":[{"@@type":"ScatterplotLayer","data":[{"position1":[-155.25, 122.3],"position2":[23.1,522.21]}],"getFillColor":[255,0,0,255],"getRadius":1000},{"@@type":"LineLayer","data":[{"position1":[-122.45, 37.8],"position2":[-122.45,37.8]}]}]}
)JSON";

static auto jsonDataTypes = R"JSON(
{"string":"sample1","int":-3351,"double":65630.5,"double_array":[0.2,0.8]}
{"string":"sample2","int":21,"double":5}
)JSON";
