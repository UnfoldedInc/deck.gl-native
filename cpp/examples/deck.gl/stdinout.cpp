// Copyright (c) 2020 Unfolded Inc.
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

#include <iostream>

#include "deck.gl/core.h"
#include "deck.gl/json.h"
#include "deck.gl/layers.h"

using namespace std;
using namespace deckgl;

// Simple application that reads deck.gl configurations from standard in
// (one configuration per line) and applies them to the deck.gl state.
int main(int argc, char** argv) {
  auto jsonConverter = unique_ptr<JSONConverter>(new JSONConverter());
  registerJSONConvertersForDeckCore(jsonConverter.get());
  registerJSONConvertersForDeckLayers(jsonConverter.get());

  auto deckProps = make_shared<Deck::Props>();
  auto deck = make_unique<Deck>(deckProps);

  while (true) {
    cout << "> ";
    string jsonStr;
    getline(cin, jsonStr);

    if (cin.eof()) {
      return 0;
    }

    auto newProps = jsonConverter->convertJson(jsonStr, "Deck");
    auto deckProps = dynamic_pointer_cast<Deck::Props>(newProps);

    deck->setProps(deckProps);

    cout << deck->viewManager->getNeedsRedraw(true).value_or("no redraw needed") << endl;
  }
  return 0;
}
