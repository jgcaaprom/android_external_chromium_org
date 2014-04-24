// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_CUSTOMIZATION_WALLPAPER_DOWNLOADER_H_
#define CHROME_BROWSER_CHROMEOS_CUSTOMIZATION_WALLPAPER_DOWNLOADER_H_

#include <string>

#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "url/gurl.h"

namespace net {
class URLRequestContextGetter;
}  // namespace net

namespace chromeos {

// Download customized wallpaper.
// Owner of this class must provide callback, which will be called on
// finished (either successful or failed) wallpaper download.
class CustomizationWallpaperDownloader : public net::URLFetcherDelegate {
 public:
  // - |url_context_getter| - Context to initialize net::URLFetcher.
  // - |wallpaper_url| - wallpaper URL to download.
  // - |wallpaper_dir| - directory, where wallpaper will be downloaded
  // (it will be created).
  // - |wallpaper_downloaded_file| - full path to local file to store downloaded
  // wallpaper file. File is downloaded to temporary location
  // |wallpaper_downloaded_file| + ".tmp", so directory must be writable.
  // After download is completed, temporary file will be renamed to
  // |wallpaper_downloaded_file|.
  CustomizationWallpaperDownloader(
      net::URLRequestContextGetter* url_context_getter,
      const GURL& wallpaper_url,
      const base::FilePath& wallpaper_dir,
      const base::FilePath& wallpaper_downloaded_file,
      base::Callback<void(bool success, const GURL&)>
          on_wallpaper_fetch_completed);

  virtual ~CustomizationWallpaperDownloader();

  // Start download.
  void Start();

  // net::URLFetcherDelegate
  virtual void OnURLFetchComplete(const net::URLFetcher* source) OVERRIDE;

 private:
  // Start new request.
  void StartRequest();

  // Schedules retry.
  void Retry();

  // Called on UI thread.
  void OnWallpaperDirectoryCreated(scoped_ptr<bool> success);

  // Called on UI thread.
  void OnTemporaryFileRenamed(scoped_ptr<bool> success);

  // This is used to initialize net::URLFetcher object.
  scoped_refptr<net::URLRequestContextGetter> url_context_getter_;

  // This fetcher is used to download wallpaper file.
  scoped_ptr<net::URLFetcher> url_fetcher_;

  // The wallpaper URL to fetch.
  const GURL wallpaper_url_;

  // Wallpaper directory (to be created).
  const base::FilePath wallpaper_dir_;

  // Full path to local file to save downloaded wallpaper.
  const base::FilePath wallpaper_downloaded_file_;

  // Full path to temporary file to fetch downloaded wallpper.
  const base::FilePath wallpaper_temporary_file_;

  // Pending retry.
  base::OneShotTimer<CustomizationWallpaperDownloader> request_scheduled_;

  // Number of download retries (first attempt is not counted as retry).
  size_t retries_;

  // Callback supplied by caller.
  base::Callback<void(bool success, const GURL&)> on_wallpaper_fetch_completed_;

  base::WeakPtrFactory<CustomizationWallpaperDownloader> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(CustomizationWallpaperDownloader);
};

}  //   namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_CUSTOMIZATION_WALLPAPER_DOWNLOADER_H_
