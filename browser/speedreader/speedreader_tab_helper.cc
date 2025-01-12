/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/speedreader/speedreader_tab_helper.h"

#include "brave/browser/brave_browser_process.h"
#include "brave/browser/speedreader/speedreader_service_factory.h"
#include "brave/browser/ui/brave_browser_window.h"
#include "brave/browser/ui/speedreader/speedreader_bubble_view.h"
#include "brave/components/speedreader/speedreader_rewriter_service.h"
#include "brave/components/speedreader/speedreader_service.h"
#include "brave/components/speedreader/speedreader_test_whitelist.h"
#include "brave/components/speedreader/speedreader_util.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_finder.h"
#include "content/public/browser/navigation_handle.h"

namespace speedreader {

SpeedreaderTabHelper::~SpeedreaderTabHelper() {
  HideBubble();
}

SpeedreaderTabHelper::SpeedreaderTabHelper(content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents) {}

bool SpeedreaderTabHelper::IsSpeedreaderEnabled() const {
  Profile* profile =
      Profile::FromBrowserContext(web_contents()->GetBrowserContext());
  DCHECK(profile);
  return SpeedreaderServiceFactory::GetForProfile(profile)->IsEnabled();
}

bool SpeedreaderTabHelper::IsEnabledForSite() {
  if (!IsSpeedreaderEnabled())
    return false;

  Profile* profile =
      Profile::FromBrowserContext(web_contents()->GetBrowserContext());
  auto* content_rules = HostContentSettingsMapFactory::GetForProfile(profile);
  return speedreader::IsEnabledForSite(content_rules,
                                       web_contents()->GetLastCommittedURL());
}

void SpeedreaderTabHelper::MaybeToggleEnabledForSite(bool on) {
  if (!IsSpeedreaderEnabled())
    return;

  Profile* profile =
      Profile::FromBrowserContext(web_contents()->GetBrowserContext());
  auto* content_rules = HostContentSettingsMapFactory::GetForProfile(profile);
  bool enabled = speedreader::IsEnabledForSite(
      content_rules, web_contents()->GetLastCommittedURL());
  if (enabled != on) {
    speedreader::SetEnabledForSite(content_rules,
                                   web_contents()->GetLastCommittedURL(), on);
    web_contents()->GetController().Reload(content::ReloadType::NORMAL, false);
  }
}

void SpeedreaderTabHelper::SingleShotSpeedreader() {
  single_shot_next_request_ = true;
  auto* contents = web_contents();
  if (contents)
    contents->GetController().Reload(content::ReloadType::NORMAL, false);
}

void SpeedreaderTabHelper::UpdateActiveState(
    content::NavigationHandle* handle) {
  DCHECK(handle);
  DCHECK(handle->IsInMainFrame());

  if (single_shot_next_request_) {
    SetNextRequestState(DistillState::kReaderMode);
    return;
  }

  if (!IsEnabledForSite()) {
    SetNextRequestState(DistillState::kNone);
    return;
  }

  // Work only with casual main frame navigations.
  if (handle->GetURL().SchemeIsHTTPOrHTTPS()) {
    auto* rewriter_service =
        g_brave_browser_process->speedreader_rewriter_service();
    if (speedreader::IsWhitelistedForTest(handle->GetURL()) ||
        rewriter_service->IsWhitelisted(handle->GetURL())) {
      VLOG(2) << __func__ << " SpeedReader active for " << handle->GetURL();
      SetNextRequestState(DistillState::kSpeedreaderMode);
      return;
    }
  }
  SetNextRequestState(DistillState::kNone);
}

void SpeedreaderTabHelper::SetNextRequestState(DistillState state) {
  distill_state_ = state;
  single_shot_next_request_ = false;
}

bool SpeedreaderTabHelper::IsActiveForMainFrame() const {
  return distill_state_ != DistillState::kNone;
}

void SpeedreaderTabHelper::DidStartNavigation(
    content::NavigationHandle* navigation_handle) {
  if (navigation_handle->IsInMainFrame()) {
    UpdateActiveState(navigation_handle);
  }
}

void SpeedreaderTabHelper::DidRedirectNavigation(
    content::NavigationHandle* navigation_handle) {
  if (navigation_handle->IsInMainFrame()) {
    UpdateActiveState(navigation_handle);
  }
}

SpeedreaderBubbleView* SpeedreaderTabHelper::speedreader_bubble_view() const {
  return speedreader_bubble_;
}

void SpeedreaderTabHelper::OnBubbleClosed() {
  speedreader_bubble_ = nullptr;
  auto* contents = web_contents();
  Browser* browser = chrome::FindBrowserWithWebContents(contents);
  DCHECK(browser);
  browser->window()->UpdatePageActionIcon(PageActionIconType::kReaderMode);
}

void SpeedreaderTabHelper::ShowSpeedreaderBubble() {
  ShowBubble(true);
}

void SpeedreaderTabHelper::ShowReaderModeBubble() {
  ShowBubble(false);
}

void SpeedreaderTabHelper::ShowBubble(bool is_bubble_speedreader) {
  auto* contents = web_contents();
  Browser* browser = chrome::FindBrowserWithWebContents(contents);
  DCHECK(browser);
  speedreader_bubble_ =
      static_cast<BraveBrowserWindow*>(browser->window())
          ->ShowSpeedreaderBubble(this, is_bubble_speedreader);
  browser->window()->UpdatePageActionIcon(PageActionIconType::kReaderMode);
}

// Hides speedreader information
void SpeedreaderTabHelper::HideBubble() {
  if (speedreader_bubble_) {
    speedreader_bubble_->Hide();
    speedreader_bubble_ = nullptr;
  }
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(SpeedreaderTabHelper)

}  // namespace speedreader
