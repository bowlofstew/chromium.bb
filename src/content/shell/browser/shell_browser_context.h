// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_SHELL_BROWSER_CONTEXT_H_
#define CONTENT_SHELL_BROWSER_SHELL_BROWSER_CONTEXT_H_

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "net/url_request/url_request_job_factory.h"

class PrefRegistrySimple;
class PrefService;

namespace net {
class NetLog;
}

namespace content {

class DownloadManagerDelegate;
class ResourceContext;
class ShellDownloadManagerDelegate;
class ShellURLRequestContextGetter;

class ShellBrowserContext : public BrowserContext {
 public:
  ShellBrowserContext(bool off_the_record, net::NetLog* net_log);
  virtual ~ShellBrowserContext();

  void set_guest_manager_delegate_for_testing(
      BrowserPluginGuestManagerDelegate* guest_manager_delegate) {
    guest_manager_delegate_ = guest_manager_delegate;
  }

  // BrowserContext implementation.
  virtual base::FilePath GetPath() const OVERRIDE;
  virtual bool IsOffTheRecord() const OVERRIDE;
  virtual DownloadManagerDelegate* GetDownloadManagerDelegate() OVERRIDE;
  virtual net::URLRequestContextGetter* GetRequestContext() OVERRIDE;
  virtual net::URLRequestContextGetter* GetRequestContextForRenderProcess(
      int renderer_child_id) OVERRIDE;
  virtual net::URLRequestContextGetter* GetMediaRequestContext() OVERRIDE;
  virtual net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(
      int renderer_child_id) OVERRIDE;
  virtual net::URLRequestContextGetter*
      GetMediaRequestContextForStoragePartition(
          const base::FilePath& partition_path,
          bool in_memory) OVERRIDE;
  virtual void RequestMidiSysExPermission(
      int render_process_id,
      int render_view_id,
      int bridge_id,
      const GURL& requesting_frame,
      bool user_gesture,
      const MidiSysExPermissionCallback& callback) OVERRIDE;
  virtual void CancelMidiSysExPermissionRequest(
      int render_process_id,
      int render_view_id,
      int bridge_id,
      const GURL& requesting_frame) OVERRIDE;
  virtual void RequestProtectedMediaIdentifierPermission(
      int render_process_id,
      int render_view_id,
      int bridge_id,
      int group_id,
      const GURL& requesting_frame,
      const ProtectedMediaIdentifierPermissionCallback& callback) OVERRIDE;
  virtual void CancelProtectedMediaIdentifierPermissionRequests(
      int group_id) OVERRIDE;
  virtual ResourceContext* GetResourceContext() OVERRIDE;
  virtual GeolocationPermissionContext*
      GetGeolocationPermissionContext() OVERRIDE;
  virtual content::BrowserPluginGuestManagerDelegate*
      GetGuestManagerDelegate() OVERRIDE;
  virtual quota::SpecialStoragePolicy* GetSpecialStoragePolicy() OVERRIDE;
  virtual bool AllowDictionaryDownloads() OVERRIDE;

  net::URLRequestContextGetter* CreateRequestContext(
      ProtocolHandlerMap* protocol_handlers,
      ProtocolHandlerScopedVector protocol_interceptors);
  net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      ProtocolHandlerMap* protocol_handlers,
      ProtocolHandlerScopedVector protocol_interceptors);

 private:
  class ShellResourceContext;

  // Performs initialization of the ShellBrowserContext while IO is still
  // allowed on the current thread.
  void InitWhileIOAllowed();

  bool off_the_record_;
  net::NetLog* net_log_;
  bool ignore_certificate_errors_;
  base::FilePath path_;
  BrowserPluginGuestManagerDelegate* guest_manager_delegate_;
  scoped_ptr<ShellResourceContext> resource_context_;
  scoped_ptr<ShellDownloadManagerDelegate> download_manager_delegate_;
  scoped_refptr<ShellURLRequestContextGetter> url_request_getter_;
  scoped_refptr<PrefRegistrySimple> pref_registry_;
  scoped_ptr<PrefService> pref_service_;

  DISALLOW_COPY_AND_ASSIGN(ShellBrowserContext);
};

}  // namespace content

#endif  // CONTENT_SHELL_BROWSER_SHELL_BROWSER_CONTEXT_H_
