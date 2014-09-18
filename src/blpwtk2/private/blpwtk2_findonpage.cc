/*
 * Copyright (C) 2013 Bloomberg Finance L.P.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <blpwtk2_findonpage.h>

namespace blpwtk2 {

FindOnPageRequest FindOnPage::makeRequest(const StringRef& text,
                                          bool matchCase,
                                          bool forward)
{
    bool findNext = StringRef(d_text).equals(text);
    if (!findNext) {
        d_text.assign(text.data(), text.length());

        // clear values for a new find
        d_numberOfMatches = 0;
        d_activeMatchOrdinal = 0;

        if (!text.isEmpty() && ++d_reqId <= 0) {
            d_reqId = 1; // handle overflow
        }
    }
    int reqId = d_text.empty() ? 0 : d_reqId; // 0 reqId to reset search

    FindOnPageRequest request;
    request.reqId = reqId;
    request.text = d_text;
    request.matchCase = matchCase;
    request.findNext = findNext;
    request.forward = forward;
    return request;
}

bool FindOnPage::applyUpdate(int reqId,
                             int numberOfMatches,
                             int activeMatchOrdinal)
{
    if (reqId != d_reqId) return false;

    if (numberOfMatches != -1) {
        d_numberOfMatches = numberOfMatches;
    }
    if (activeMatchOrdinal != -1) {
        d_activeMatchOrdinal = activeMatchOrdinal;
    }
    return true;
}

}  // close namespace blpwtk2
