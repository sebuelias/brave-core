/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global window */

import { createReducer } from 'redux-act'
import { WalletState } from '../../constants/types'
import * as WalletActions from '../actions/wallet_actions'
import { InitializedPayloadType } from '../constants/action_types'

const defaultState: WalletState = {
  hasInitialized: false,
  isWalletCreated: false,
  isWalletLocked: true,
  favoriteApps: [],
  isWalletBackedUp: false,
  hasIncorrectPassword: false,
  accounts: [],
  walletAccountNames: [],
  transactions: []
}

const reducer = createReducer<WalletState>({}, defaultState)

reducer.on(WalletActions.initialized, (state: any, payload: InitializedPayloadType) => {
  const accountNames = payload.walletAccountNames.split(',')
  const accounts = payload.accounts.map((address: string, idx: number) => {
    return {
      id: `${idx + 1}`,
      name: accountNames[idx],
      address,
      balance: 0,
      fiatBalance: '0',
      asset: 'eth',
      accountType: 'Primary'
    }
  })

  return {
    ...state,
    hasInitialized: true,
    isWalletCreated: payload.isWalletCreated,
    isWalletLocked: payload.isWalletLocked,
    favoriteApps: payload.favoriteApps,
    accounts,
    isWalletBackedUp: payload.isWalletBackedUp,
    walletAccountNames: accountNames
  }
})

reducer.on(WalletActions.hasIncorrectPassword, (state: any, payload: boolean) => {
  return {
    ...state,
    hasIncorrectPassword: payload
  }
})

export default reducer
