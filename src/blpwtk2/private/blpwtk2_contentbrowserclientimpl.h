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

#ifndef INCLUDED_BLPWTK2_CONTENTBROWSERCLIENTIMPL_H
#define INCLUDED_BLPWTK2_CONTENTBROWSERCLIENTIMPL_H

#include <blpwtk2_config.h>

#include <base/memory/scoped_ptr.h>
#include <content/public/browser/content_browser_client.h>
#include <net/url_request/url_request_job_factory.h>

namespace net {
    class URLRequestContextGetter;
}  // close namespace net

namespace blpwtk2 {

class RendererInfoMap;

// This is our implementation of the content::ContentBrowserClient interface.
// This interface allows us to add hooks to the "browser" portion of the
// content module.  This is created as part of the startup process of
// BrowserMainRunner.
class ContentBrowserClientImpl : public content::ContentBrowserClient {
  public:
    explicit ContentBrowserClientImpl(RendererInfoMap* rendererInfoMap);
    virtual ~ContentBrowserClientImpl();

    // Notifies that a render process will be created. This is called before
    // the content layer adds its own BrowserMessageFilters, so that the
    // embedder's IPC filters have priority.
    virtual void RenderProcessWillLaunch(content::RenderProcessHost* host) OVERRIDE;

    // Called by WebContents to override the WebKit preferences that are used by
    // the renderer. The content layer will add its own settings, and then it's up
    // to the embedder to update it if it wants.
    virtual void OverrideWebkitPrefs(content::RenderViewHost* render_view_host,
                                     const GURL& url,
                                     WebPreferences* prefs) OVERRIDE;

    // Returns true whether the embedder supports in-process renderers or not.
    // When running "in process", the browser maintains a RenderProcessHost which
    // communicates to a RenderProcess which is instantiated in the same process
    // with the Browser. All IPC between the Browser and the Renderer is the
    // same, it's just not crossing a process boundary. This returns false by
    // default. If implementations return true, they must also implement
    // StartInProcessRendererThread and StopInProcessRendererThread.
    virtual bool SupportsInProcessRenderer() OVERRIDE;

    virtual void ResourceDispatcherHostCreated() OVERRIDE;

    // If content creates the WebContentsView implementation, it will ask the
    // embedder to return an (optional) delegate to customize it. The view will
    // own the delegate.
    virtual content::WebContentsViewDelegate* GetWebContentsViewDelegate(
        content::WebContents* webContents);

    // Creates the main net::URLRequestContextGetter.  Should only be called
    // once per ContentBrowserClient object.
    // TODO(ajwong): Remove once http://crbug.com/159193 is resolved.
    virtual net::URLRequestContextGetter* CreateRequestContext(
        content::BrowserContext* browserContext,
        content::ProtocolHandlerMap* protocolHandlers,
        content::ProtocolHandlerScopedVector protocol_interceptors) OVERRIDE;

    // Returns whether a specified URL is handled by the embedder's internal
    // protocol handlers.
    virtual bool IsHandledURL(const GURL& url) OVERRIDE;

  private:
    RendererInfoMap* d_rendererInfoMap;

    DISALLOW_COPY_AND_ASSIGN(ContentBrowserClientImpl);
};

}  // close namespace blpwtk2

#endif  // INCLUDED_BLPWTK2_CONTENTBROWSERCLIENTIMPL_H

