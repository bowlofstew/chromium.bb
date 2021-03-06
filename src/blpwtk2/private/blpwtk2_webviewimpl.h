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

#ifndef INCLUDED_BLPWTK2_WEBVIEWIMPL_H
#define INCLUDED_BLPWTK2_WEBVIEWIMPL_H

#include <blpwtk2_config.h>

#include <blpwtk2_findonpage.h>
#include <blpwtk2_nativeviewwidgetdelegate.h>
#include <blpwtk2_webview.h>
#include <blpwtk2_webviewproperties.h>

#include <content/public/browser/web_contents_delegate.h>
#include <content/public/browser/web_contents_observer.h>
#include <content/public/common/context_menu_params.h>
#include <content/public/common/file_chooser_params.h>
#include <ui/gfx/native_widget_types.h>
#include <third_party/WebKit/public/web/WebTextDirection.h>

struct WebPreferences;

namespace content {
    class WebContents;
}  // close namespace content

namespace views {
    class Widget;
}  // close namespace views

namespace blpwtk2 {

class BrowserContextImpl;
class ContextMenuParams;
class DevToolsFrontendHostDelegateImpl;
class NativeViewWidget;
class WebViewDelegate;
class WebViewImplClient;

// This is the implementation of the blpwtk2::WebView interface.  It creates a
// content::WebContents object, and implements the content::WebContentsDelegate
// interface.  It forwards the content::WebContentsDelegate override events to
// the blpwtk2::WebViewDelegate, which is provided by the application.
//
// This class can only be instantiated from the browser-main thread.
//
// When we are using 'ThreadMode::ORIGINAL', this object is returned to the
// application by 'Toolkit::createWebView'.  When we are using
// 'ThreadMode::RENDERER_MAIN', this object is created on the secondary
// browser-main thread, and the application instead gets a 'WebViewProxy',
// which forwards events back and forth between the secondary browser-main
// thread and the application thread.  See blpwtk2_toolkit.h for an explanation
// about threads.
class WebViewImpl : public WebView,
                    private NativeViewWidgetDelegate,
                    private content::WebContentsDelegate,
                    private content::WebContentsObserver {
  public:
    WebViewImpl(WebViewDelegate* delegate,
                blpwtk2::NativeView parent,
                BrowserContextImpl* browserContext,
                int hostAffinity,
                bool initiallyVisible,
                const WebViewProperties& properties);
    WebViewImpl(content::WebContents* contents,
                BrowserContextImpl* browserContext,
                const WebViewProperties& properties);
    virtual ~WebViewImpl();

    void setImplClient(WebViewImplClient* client);
    gfx::NativeView getNativeView() const;
    void showContextMenu(const ContextMenuParams& params);
    void saveCustomContextMenuContext(
        content::RenderFrameHost* rfh,
        const content::CustomContextMenuContext& context);
    void handleFindRequest(const FindOnPageRequest& request);
    void handleExternalProtocol(const GURL& url);
    void overrideWebkitPrefs(WebPreferences* prefs);

    /////////////// WebView overrides

    virtual void destroy() OVERRIDE;
    virtual WebFrame* mainFrame() OVERRIDE;
    virtual void loadUrl(const StringRef& url) OVERRIDE;
    virtual void loadInspector(WebView* inspectedView) OVERRIDE;
    virtual void inspectElementAt(const POINT& point) OVERRIDE;
    virtual void reload(bool ignoreCache) OVERRIDE;
    virtual void goBack() OVERRIDE;
    virtual void goForward() OVERRIDE;
    virtual void stop() OVERRIDE;
    virtual void takeKeyboardFocus() OVERRIDE;
    virtual void setLogicalFocus(bool focused) OVERRIDE;
    virtual void show() OVERRIDE;
    virtual void hide() OVERRIDE;
    virtual void setParent(NativeView parent) OVERRIDE;
    virtual void embedChild(NativeView child) OVERRIDE;
    virtual void move(int left, int top, int width, int height) OVERRIDE;
    virtual void cutSelection() OVERRIDE;
    virtual void copySelection() OVERRIDE;
    virtual void paste() OVERRIDE;
    virtual void deleteSelection() OVERRIDE;
    virtual void enableFocusBefore(bool enabled) OVERRIDE;
    virtual void enableFocusAfter(bool enabled) OVERRIDE;
    virtual void enableNCHitTest(bool enabled) OVERRIDE;
    virtual void onNCHitTestResult(int x, int y, int result) OVERRIDE;
    virtual void fileChooserCompleted(const StringRef* paths,
                                      size_t numPaths) OVERRIDE;
    virtual void performCustomContextMenuAction(int actionId) OVERRIDE;
    virtual void enableAltDragRubberbanding(bool enabled) OVERRIDE;
    virtual void enableCustomTooltip(bool enabled) OVERRIDE;
    virtual void setZoomPercent(int value) OVERRIDE;
    virtual void find(const StringRef& text, bool matchCase, bool forward) OVERRIDE;
    virtual void replaceMisspelledRange(const StringRef& text) OVERRIDE;
    virtual void rootWindowPositionChanged() OVERRIDE;
    virtual void rootWindowSettingsChanged() OVERRIDE;
    virtual void print() OVERRIDE;
    virtual void handleInputEvents(const InputEvent *events, size_t eventsCount) OVERRIDE;
    virtual void setDelegate(WebViewDelegate* delegate) OVERRIDE;

  private:
    void createWidget(blpwtk2::NativeView parent);

    /////// NativeViewWidgetDelegate overrides

    virtual void onDestroyed(NativeViewWidget* source) OVERRIDE;
    virtual bool OnNCHitTest(int* result) OVERRIDE;
    virtual bool OnNCDragBegin(int hitTestCode) OVERRIDE;
    virtual void OnNCDragMove() OVERRIDE;
    virtual void OnNCDragEnd() OVERRIDE;

    /////// WebContentsDelegate overrides

    // Notification that the target URL has changed.
    virtual void UpdateTargetURL(content::WebContents* source,
                                 int32 page_id,
                                 const GURL& url) OVERRIDE;

    // Notifies the delegate that this contents is starting or is done loading
    // some resource. The delegate should use this notification to represent
    // loading feedback. See WebContents::IsLoading()
    virtual void LoadingStateChanged(content::WebContents* source,
                                     bool to_different_document) OVERRIDE;

    // Invoked when a main frame navigation occurs.
    virtual void DidNavigateMainFramePostCommit(content::WebContents* source) OVERRIDE;

    // Called when a file selection is to be done.
    virtual void RunFileChooser(content::WebContents* source,
                                const content::FileChooserParams& params) OVERRIDE;

    // This is called when WebKit tells us that it is done tabbing through
    // controls on the page. Provides a way for WebContentsDelegates to handle
    // this. Returns true if the delegate successfully handled it.
    virtual bool TakeFocus(content::WebContents* source, bool reverse) OVERRIDE;

    // Notification that |contents| has gained focus.
    virtual void WebContentsFocused(content::WebContents* contents) OVERRIDE;

    // Notification that |contents| has lost focus.
    virtual void WebContentsBlurred(content::WebContents* contents) OVERRIDE;

    // Notifies the delegate about the creation of a new WebContents. This
    // typically happens when popups are created.
    virtual void WebContentsCreated(content::WebContents* source_contents,
                                    int opener_render_frame_id,
                                    const base::string16& frame_name,
                                    const GURL& target_url,
                                    const content::ContentCreatedParams& params,
                                    content::WebContents* new_contents) OVERRIDE;

    // Request the delegate to close this web contents, and do whatever cleanup
    // it needs to do.
    virtual void CloseContents(content::WebContents* source) OVERRIDE;

    // Request the delegate to move this WebContents to the specified position
    // in screen coordinates.
    virtual void MoveContents(content::WebContents* source, const gfx::Rect& pos) OVERRIDE;

    // Called to determine if the WebContents is contained in a popup window
    // or a panel window.
    virtual bool IsPopupOrPanel(const content::WebContents* source) const OVERRIDE;

    // Asks permission to use the camera and/or microphone. If permission is
    // granted, a call should be made to |callback| with the devices. If the
    // request is denied, a call should be made to |callback| with an empty list
    // of devices. |request| has the details of the request (e.g. which of audio
    // and/or video devices are requested, and lists of available devices).
    virtual void RequestMediaAccessPermission(
        content::WebContents* web_contents,
        const content::MediaStreamRequest& request,
        const content::MediaResponseCallback& callback) OVERRIDE;

    // Return true if the RWHV should take focus on mouse-down.
    virtual bool ShouldSetKeyboardFocusOnMouseDown() OVERRIDE;
    virtual bool ShouldSetLogicalFocusOnMouseDown() OVERRIDE;

    // Allows delegate to show a custom tooltip. If the delegate doesn't want a
    // custom tooltip, it should just return 'false'. Otherwise, it should show
    // the tooltip and return 'true'. By default, the delegate doesn't provide a
    // custom tooltip.
    virtual bool ShowTooltip(content::WebContents* source,
                             const base::string16& tooltip_text,
                             blink::WebTextDirection text_direction_hint) OVERRIDE;

    // Information about current find request
    virtual void FindReply(content::WebContents* source_contents,
                           int request_id,
                           int number_of_matches,
                           const gfx::Rect& selection_rect,
                           int active_match_ordinal,
                           bool final_update) OVERRIDE;

    /////// WebContentsObserver overrides

    // This method is invoked after the WebContents decided which RenderViewHost
    // to use for the next navigation, but before the navigation starts.
    virtual void AboutToNavigateRenderView(
        content::RenderViewHost* render_view_host) OVERRIDE;

    // This method is invoked when the navigation is done, i.e. the spinner of
    // the tab will stop spinning, and the onload event was dispatched.
    //
    // If the WebContents is displaying replacement content, e.g. network error
    // pages, DidFinishLoad is invoked for frames that were not sending
    // navigational events before. It is safe to ignore these events.
    virtual void DidFinishLoad(
        int64 frame_id,
        const GURL& validated_url,
        bool is_main_frame,
        content::RenderViewHost* render_view_host) OVERRIDE;

    // This method is like DidFinishLoad, but when the load failed or was
    // cancelled, e.g. window.stop() is invoked.
    virtual void DidFailLoad(
        int64 frame_id,
        const GURL& validated_url,
        bool is_main_frame,
        int error_code,
        const base::string16& error_description,
        content::RenderViewHost* render_view_host) OVERRIDE;

  private:
    scoped_ptr<DevToolsFrontendHostDelegateImpl> d_devToolsFrontEndHost;
    scoped_ptr<content::WebContents> d_webContents;
    scoped_ptr<FindOnPage> d_find;
    WebViewDelegate* d_delegate;
    WebViewImplClient* d_implClient;
    BrowserContextImpl* d_browserContext;
    NativeViewWidget* d_widget;  // owned by the views system
    WebViewProperties d_properties;  // TODO(SHEZ): move more properties into this struct
    bool d_focusBeforeEnabled;
    bool d_focusAfterEnabled;
    bool d_isReadyForDelete;  // when the underlying WebContents can be deleted
    bool d_wasDestroyed;      // if destroy() has been called
    bool d_isDeletingSoon;    // when DeleteSoon has been called
    bool d_isPopup;           // if this view is a popup view
    bool d_altDragRubberbandingEnabled;
    bool d_customTooltipEnabled;
    bool d_ncHitTestEnabled;
    bool d_ncHitTestPendingAck;
    int d_lastNCHitTestResult;
    content::FileChooserParams::Mode d_lastFileChooserMode;

    // For calling performCustomContextMenuAction().  TODO: clean this
    content::CustomContextMenuContext d_customContext;


    DISALLOW_COPY_AND_ASSIGN(WebViewImpl);
};

}  // close namespace blpwtk2

#endif  // INCLUDED_BLPWTK2_WEBVIEWIMPL_H

