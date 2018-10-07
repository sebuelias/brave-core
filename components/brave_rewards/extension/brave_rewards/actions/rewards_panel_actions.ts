/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { action } from 'typesafe-actions'

// Constants
import { types } from '../constants/rewards_panel_types'

export const createWallet = () => action(types.CREATE_WALLET, {})

export const onWalletCreated = () => action(types.ON_WALLET_CREATED, {})

export const onWalletCreateFailed = () => action(types.ON_WALLET_CREATE_FAILED, {})

export const onTabId = (tabId: number | undefined) => action(types.ON_TAB_ID, {
  tabId
})

export const onTabRetrieved = (tab: chrome.tabs.Tab) => action(types.ON_TAB_RETRIEVED, {
  tab
})

export const onPublisherData = (windowId: number, publisher: RewardsExtension.Publisher) => action(types.ON_PUBLISHER_DATA, {
  windowId,
  publisher
})

export const getWalletProperties = () => action(types.GET_WALLET_PROPERTIES, {})

export const onWalletProperties = (properties: RewardsExtension.WalletProperties) => action(types.ON_WALLET_PROPERTIES, {
  properties
})

export const getCurrentReport = () => action(types.GET_CURRENT_REPORT, {})

export const onCurrentReport = (properties: RewardsExtension.Report) => action(types.ON_CURRENT_REPORT, {
  properties
})

export const onNotificationAdded = (id: number, type: number, timestamp: number, args: string[]) => action(types.ON_NOTIFICATION_ADDED, {
  id,
  type,
  timestamp,
  args
})

export const onNotificationDeleted = (id: number, type: number, timestamp: number) => action(types.ON_NOTIFICATION_DELETED, {
  id,
  timestamp,
  type
})

export const deleteNotification = (id: number) => action(types.DELETE_NOTIFICATION, {
  id
})

export const includeInAutoContribution = (publisherKey: string, excluded: boolean) => action(types.INCLUDE_IN_AUTO_CONTRIBUTION, {
  publisherKey,
  excluded
})
