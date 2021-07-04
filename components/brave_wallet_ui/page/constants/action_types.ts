// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
import { GetAssetPriceHistoryReturnObjectInfo, AssetOptionType, AssetPriceTimeframe } from '../../constants/types'

export type CreateWalletPayloadType = {
  password: string
}

export type AddAccountToWalletPayloadType = {
  accountNames: string
}

export type RestoreWalletPayloadType = {
  mnemonic: string,
  password: string
}

export type WalletCreatedPayloadType = {
  mnemonic: string
}

export type RecoveryWordsAvailablePayloadType = {
  mnemonic: string
}

export type UpdateSelectedAssetType = {
  asset: AssetOptionType,
  timeFrame: AssetPriceTimeframe
}

export type SelectAssetPayloadType = {
  priceHistory: GetAssetPriceHistoryReturnObjectInfo | undefined,
  price: string
  timeFrame: AssetPriceTimeframe
}
