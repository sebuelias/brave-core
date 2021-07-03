/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eip2930_transaction.h"

#include <utility>

#include "base/values.h"
#include "brave/components/brave_wallet/browser/brave_wallet_utils.h"
#include "brave/components/brave_wallet/browser/eth_address.h"
#include "brave/components/brave_wallet/browser/rlp_encode.h"

namespace brave_wallet {

Eip2930Transaction::AccessListItem::AccessListItem() = default;
Eip2930Transaction::AccessListItem::~AccessListItem() = default;
Eip2930Transaction::AccessListItem::AccessListItem(const AccessListItem&) =
    default;

bool Eip2930Transaction::AccessListItem::operator==(
    const AccessListItem& item) const {
  if (!std::equal(address.begin(), address.end(), item.address.begin()))
    return false;
  if (storage_keys.size() != item.storage_keys.size())
    return false;
  for (size_t i = 0; i < storage_keys.size(); ++i) {
    if (!std::equal(storage_keys[i].begin(), storage_keys[i].end(),
                    item.storage_keys[i].begin()))
      return false;
  }
  return true;
}

bool Eip2930Transaction::AccessListItem::operator!=(
    const AccessListItem& item) const {
  return !operator==(item);
}

Eip2930Transaction::Eip2930Transaction() = default;
Eip2930Transaction::Eip2930Transaction(const TxData& tx_data, uint64_t chain_id)
    : EthTransaction(tx_data), chain_id_(chain_id) {
  type_ = 1;
}
Eip2930Transaction::Eip2930Transaction(const Eip2930Transaction&) = default;
Eip2930Transaction::~Eip2930Transaction() = default;

bool Eip2930Transaction::operator==(const Eip2930Transaction& tx) const {
  return EthTransaction::operator==(tx) && chain_id_ == tx.chain_id_ &&
         std::equal(access_list_.begin(), access_list_.end(),
                    tx.access_list_.begin());
}

// static
base::Optional<Eip2930Transaction> Eip2930Transaction::FromValue(
    const base::Value& value) {
  base::Optional<EthTransaction> legacy_tx = EthTransaction::FromValue(value);
  if (!legacy_tx)
    return base::nullopt;
  TxData tx_data(legacy_tx->nonce(), legacy_tx->gas_price(),
                 legacy_tx->gas_limit(), legacy_tx->to(), legacy_tx->value(),
                 legacy_tx->data());

  const std::string* tx_chain_id = value.FindStringKey("chain_id");
  if (!tx_chain_id)
    return base::nullopt;
  uint256_t chain_id;
  if (!HexValueToUint256(*tx_chain_id, &chain_id))
    return base::nullopt;

  Eip2930Transaction tx(tx_data, static_cast<uint64_t>(chain_id));
  tx.v_ = legacy_tx->v();
  tx.r_ = legacy_tx->r();
  tx.s_ = legacy_tx->s();

  const base::Value* access_list = value.FindKey("access_list");
  if (!access_list)
    return base::nullopt;
  base::Optional<AccessList> access_list_from_value =
      ValueToAccessList(*access_list);
  if (!access_list_from_value)
    return base::nullopt;
  tx.access_list_ = *access_list_from_value;

  return tx;
}

// static
std::vector<base::Value> Eip2930Transaction::AccessListToValue(
    const AccessList& list) {
  std::vector<base::Value> access_list;
  for (const AccessListItem& item : list) {
    std::vector<base::Value> access_list_item;
    access_list_item.push_back(base::Value(item.address));
    std::vector<base::Value> storage_keys;
    for (const AccessedStorageKey& key : item.storage_keys) {
      storage_keys.push_back(base::Value(key));
    }
    access_list_item.push_back(base::Value(storage_keys));

    access_list.push_back(base::Value(access_list_item));
  }
  return access_list;
}

// static
base::Optional<Eip2930Transaction::AccessList>
Eip2930Transaction::ValueToAccessList(const base::Value& value) {
  AccessList access_list;
  for (const auto& item_value : value.GetList()) {
    AccessListItem item;
    std::vector<uint8_t> address = item_value.GetList()[0].GetBlob();
    std::move(address.begin(), address.end(), item.address.begin());
    for (const auto& storage_key_value : item_value.GetList()[1].GetList()) {
      std::vector<uint8_t> storage_key_vec = storage_key_value.GetBlob();
      AccessedStorageKey storage_key;
      std::move(storage_key_vec.begin(), storage_key_vec.end(),
                storage_key.begin());
      item.storage_keys.push_back(storage_key);
    }
    access_list.push_back(item);
  }
  return access_list;
}

std::vector<uint8_t> Eip2930Transaction::GetMessageToSign(
    uint64_t chain_id) const {
  std::vector<uint8_t> result;
  result.push_back(type_);

  // TODO(darkdh): Migrate to std::vector<base::Value>, base::ListValue is
  // deprecated
  base::ListValue list;
  list.Append(RLPUint256ToBlobValue(chain_id_));
  list.Append(RLPUint256ToBlobValue(nonce_));
  list.Append(RLPUint256ToBlobValue(gas_price_));
  list.Append(RLPUint256ToBlobValue(gas_limit_));
  list.Append(base::Value(to_.bytes()));
  list.Append(RLPUint256ToBlobValue(value_));
  list.Append(base::Value(data_));
  list.Append(base::Value(AccessListToValue(access_list_)));

  const std::string rlp_msg = RLPEncode(std::move(list));
  result.insert(result.end(), rlp_msg.begin(), rlp_msg.end());
  return KeccakHash(result);
}

std::string Eip2930Transaction::GetSignedTransaction() const {
  DCHECK(IsSigned());

  // TODO(darkdh): Migrate to std::vector<base::Value>, base::ListValue is
  // deprecated
  base::ListValue list;
  list.Append(RLPUint256ToBlobValue(chain_id_));
  list.Append(RLPUint256ToBlobValue(nonce_));
  list.Append(RLPUint256ToBlobValue(gas_price_));
  list.Append(RLPUint256ToBlobValue(gas_limit_));
  list.Append(base::Value(to_.bytes()));
  list.Append(RLPUint256ToBlobValue(value_));
  list.Append(base::Value(data_));
  list.Append(base::Value(AccessListToValue(access_list_)));
  list.Append(base::Value(v_));
  list.Append(base::Value(r_));
  list.Append(base::Value(s_));

  std::vector<uint8_t> result;
  result.push_back(type_);

  const std::string rlp_msg = RLPEncode(std::move(list));
  result.insert(result.end(), rlp_msg.begin(), rlp_msg.end());

  return ToHex(result);
}

void Eip2930Transaction::ProcessSignature(const std::vector<uint8_t> signature,
                                          int recid,
                                          uint64_t chain_id) {
  EthTransaction::ProcessSignature(signature, recid, chain_id_);
  v_ = recid;
}

bool Eip2930Transaction::IsSigned() const {
  return r_.size() != 0 && s_.size() != 0;
}

base::Value Eip2930Transaction::ToValue() const {
  base::Value tx = EthTransaction::ToValue();
  tx.SetStringKey("chain_id", Uint256ValueToHex(chain_id_));
  tx.SetKey("access_list", base::Value(AccessListToValue(access_list_)));

  return tx;
}

}  // namespace brave_wallet
