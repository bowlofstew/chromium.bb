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

#include <blpwtk2_browsercontextimpl.h>

#include <blpwtk2_prefstore.h>
#include <blpwtk2_resourcecontextimpl.h>
#include <blpwtk2_spellcheckconfig.h>
#include <blpwtk2_stringref.h>
#include <blpwtk2_urlrequestcontextgetterimpl.h>

#include <base/files/file_path.h>
#include <base/file_util.h>
#include <base/logging.h>  // for CHECK
#include <base/prefs/pref_service_builder.h>
#include <base/prefs/pref_service.h>
#include <base/threading/thread_restrictions.h>
#include <chrome/browser/spellchecker/spellcheck_factory.h>
#include <chrome/common/pref_names.h>
#include <content/public/browser/browser_thread.h>
#include <content/public/browser/storage_partition.h>
#include <components/browser_context_keyed_service/browser_context_dependency_manager.h>
#include <components/user_prefs/pref_registry_syncable.h>
#include <components/user_prefs/user_prefs.h>

namespace blpwtk2 {

BrowserContextImpl::BrowserContextImpl(const std::string& dataDir,
                                       bool diskCacheEnabled)
: d_isIncognito(dataDir.empty())
{
    // If disk cache is enabled, then it must not be incognito.
    DCHECK(!diskCacheEnabled || !d_isIncognito);

    base::FilePath path;

    if (!d_isIncognito) {
        // allow IO during creation of data directory
        base::ThreadRestrictions::ScopedAllowIO allowIO;

        path = base::FilePath::FromUTF8Unsafe(dataDir);
        if (!file_util::PathExists(path))
            file_util::CreateDirectory(path);
    }
    else {
        // It seems that even incognito browser contexts need to return a valid
        // data path.  Not providing one causes a DCHECK failure in
        // fileapi::DeviceMediaAsyncFileUtil::Create.
        // For now, just create a temporary directory for this.
        // TODO: see if we can remove this requirement.

        // allow IO during creation of temporary directory
        base::ThreadRestrictions::ScopedAllowIO allowIO;
        file_util::CreateNewTempDirectory(L"blpwtk2_", &path);
    }

    d_requestContextGetter = new URLRequestContextGetterImpl(path, diskCacheEnabled);

    {
        // Initialize prefs for this context.
        d_prefRegistry = new user_prefs::PrefRegistrySyncable();
        d_userPrefs = new PrefStore();

        d_prefRegistry->RegisterListPref(prefs::kSpellCheckCustomWords,
                                         new base::ListValue(),
                                         user_prefs::PrefRegistrySyncable::UNSYNCABLE_PREF);

        PrefServiceBuilder builder;
        builder.WithUserPrefs(d_userPrefs);
        d_prefService.reset(builder.Create(d_prefRegistry.get()));
        user_prefs::UserPrefs::Set(this, d_prefService.get());
    }

    SpellcheckServiceFactory::GetInstance();  // This needs to be initialized before
                                              // calling CreateBrowserContextServices.

    BrowserContextDependencyManager::GetInstance()->CreateBrowserContextServices(this, false);
}

BrowserContextImpl::~BrowserContextImpl()
{
    if (d_isIncognito) {
        // Delete the temporary directory that we created in the constructor.

        // allow IO during deletion of temporary directory
        base::ThreadRestrictions::ScopedAllowIO allowIO;
        DCHECK(file_util::PathExists(d_requestContextGetter->path()));
        file_util::Delete(d_requestContextGetter->path(), true);
    }

    BrowserContextDependencyManager::GetInstance()->DestroyBrowserContextServices(this);

    if (d_resourceContext.get()) {
        content::BrowserThread::DeleteSoon(content::BrowserThread::IO,
                                           FROM_HERE,
                                           d_resourceContext.release());
    }
}

URLRequestContextGetterImpl* BrowserContextImpl::requestContextGetter() const
{
    return d_requestContextGetter.get();
}

void BrowserContextImpl::setProxyConfig(const net::ProxyConfig& config)
{
    DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
    d_requestContextGetter->setProxyConfig(config);
}

void BrowserContextImpl::useSystemProxyConfig()
{
    DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
    d_requestContextGetter->useSystemProxyConfig();
}

void BrowserContextImpl::setSpellCheckConfig(const SpellCheckConfig& config)
{
    DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

    PrefService* prefs = user_prefs::UserPrefs::Get(this);

    bool wasEnabled = prefs->GetBoolean(prefs::kEnableContinuousSpellcheck);

    std::string languages;
    base::ListValue customWords;
    for (size_t i = 0; i < config.numLanguages(); ++i) {
        StringRef str = config.languageAt(i);
        if (!languages.empty()) {
            languages.append(",");
        }
        languages.append(str.data(), str.length());
    }
    for (size_t i = 0; i < config.numCustomWords(); ++i) {
        StringRef str = config.customWordAt(i);
        customWords.AppendString(std::string(str.data(), str.length()));
    }
    prefs->SetBoolean(prefs::kEnableContinuousSpellcheck,
                      config.isSpellCheckEnabled());
    prefs->SetBoolean(prefs::kEnableAutoSpellCorrect,
                      config.isAutoCorrectEnabled());
    prefs->SetString(prefs::kSpellCheckDictionary, languages);
    prefs->Set(prefs::kSpellCheckCustomWords, customWords);

    if (!wasEnabled && config.isSpellCheckEnabled()) {
        // Ensure the spellcheck service is created for this context if we just
        // turned it on.
        SpellcheckServiceFactory::GetForContext(this);
    }
}

// ======== content::BrowserContext implementation =============

base::FilePath BrowserContextImpl::GetPath()
{
    return d_requestContextGetter->path();
}

bool BrowserContextImpl::IsOffTheRecord() const
{
    return d_isIncognito;
}

net::URLRequestContextGetter* BrowserContextImpl::GetRequestContext()
{
    return GetDefaultStoragePartition(this)->GetURLRequestContext();
}

net::URLRequestContextGetter*
BrowserContextImpl::GetRequestContextForRenderProcess(int rendererChildId)
{
    return GetRequestContext();
}

net::URLRequestContextGetter* BrowserContextImpl::GetMediaRequestContext()
{
    return GetRequestContext();
}

net::URLRequestContextGetter*
BrowserContextImpl::GetMediaRequestContextForRenderProcess(int rendererChildId)
{
    return GetRequestContext();
}

net::URLRequestContextGetter*
BrowserContextImpl::GetMediaRequestContextForStoragePartition(
    const base::FilePath& partitionPath,
    bool inMemory)
{
    return GetRequestContext();
}

content::ResourceContext* BrowserContextImpl::GetResourceContext()
{
    DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

    if (!d_resourceContext.get()) {
        d_resourceContext.reset(
            new ResourceContextImpl(d_requestContextGetter.get()));
    }
    return d_resourceContext.get();
}

content::DownloadManagerDelegate*
BrowserContextImpl::GetDownloadManagerDelegate()
{
    return 0;
}

content::GeolocationPermissionContext*
BrowserContextImpl::GetGeolocationPermissionContext()
{
    return 0;
}

content::SpeechRecognitionPreferences*
BrowserContextImpl::GetSpeechRecognitionPreferences()
{
    return 0;
}

quota::SpecialStoragePolicy* BrowserContextImpl::GetSpecialStoragePolicy()
{
    return 0;
}

bool BrowserContextImpl::AllowDictionaryDownloads()
{
    return false;
}

}  // close namespace blpwtk2
