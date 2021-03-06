/*
 * Copyright (C) 2014 Bloomberg Finance L.P.
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

#ifndef INCLUDED_BLPWTK2_IPCPARAMTRAITS_H
#define INCLUDED_BLPWTK2_IPCPARAMTRAITS_H

#include <blpwtk2_config.h>

#include <ipc/ipc_message.h>
#include <ipc/ipc_param_traits.h>

namespace blpwtk2 {
class ContextMenuParams;
class FileChooserParams;
struct FindOnPageRequest;
class NewViewParams;
class ProxyConfig;
class SpellCheckConfig;
struct WebViewProperties;
}  // close namespace blpwtk2

namespace IPC {

template <>
struct ParamTraits<blpwtk2::WebViewProperties> {
    typedef blpwtk2::WebViewProperties param_type;
    static void Write(Message* m, const param_type& p);
    static bool Read(const Message* m, PickleIterator* iter, param_type* r);
    static void Log(const param_type& p, std::string* l);
};

template <>
struct ParamTraits<blpwtk2::ContextMenuParams> {
    typedef blpwtk2::ContextMenuParams param_type;
    static void Write(Message* m, const param_type& p);
    static bool Read(const Message* m, PickleIterator* iter, param_type* r);
    static void Log(const param_type& p, std::string* l);
};

template <>
struct ParamTraits<blpwtk2::FileChooserParams> {
    typedef blpwtk2::FileChooserParams param_type;
    static void Write(Message* m, const param_type& p);
    static bool Read(const Message* m, PickleIterator* iter, param_type* r);
    static void Log(const param_type& p, std::string* l);
};

template <>
struct ParamTraits<blpwtk2::FindOnPageRequest> {
    typedef blpwtk2::FindOnPageRequest param_type;
    static void Write(Message* m, const param_type& p);
    static bool Read(const Message* m, PickleIterator* iter, param_type* r);
    static void Log(const param_type& p, std::string* l);
};

template <>
struct ParamTraits<blpwtk2::NewViewParams> {
    typedef blpwtk2::NewViewParams param_type;
    static void Write(Message* m, const param_type& p);
    static bool Read(const Message* m, PickleIterator* iter, param_type* r);
    static void Log(const param_type& p, std::string* l);
};

template <>
struct ParamTraits<blpwtk2::ProxyConfig> {
    typedef blpwtk2::ProxyConfig param_type;
    static void Write(Message* m, const param_type& p);
    static bool Read(const Message* m, PickleIterator* iter, param_type* r);
    static void Log(const param_type& p, std::string* l);
};

template <>
struct ParamTraits<blpwtk2::SpellCheckConfig> {
    typedef blpwtk2::SpellCheckConfig param_type;
    static void Write(Message* m, const param_type& p);
    static bool Read(const Message* m, PickleIterator* iter, param_type* r);
    static void Log(const param_type& p, std::string* l);
};

}  // close namespace IPC

#endif  // INCLUDED_BLPWTK2_IPCPARAMTRAITS_H

