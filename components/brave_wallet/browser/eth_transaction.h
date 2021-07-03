/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_TRANSACTION_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_TRANSACTION_H_

#include <string>
#include <vector>

#include "base/gtest_prod_util.h"
#include "base/optional.h"
#include "brave/components/brave_wallet/browser/brave_wallet_types.h"
#include "brave/components/brave_wallet/browser/eth_address.h"

namespace base {
class Value;
}  // namespace base

namespace brave_wallet {
FORWARD_DECLARE_TEST(EthTransactionTest, GetSignedTransaction);
FORWARD_DECLARE_TEST(EthTransactionTest, TransactionAndValue);
FORWARD_DECLARE_TEST(Eip2930TransactionUnitTest, GetSignedTransaction);

class EthTransaction {
 public:
  struct TxData {
    TxData();
    TxData(const uint256_t& nonce_in,
           const uint256_t& gas_price_in,
           const uint256_t& gas_limit_in,
           const EthAddress& to_in,
           const uint256_t& value_in,
           const std::vector<uint8_t>& data_in);
    ~TxData();

    uint256_t nonce;
    uint256_t gas_price;
    uint256_t gas_limit;
    EthAddress to;
    uint256_t value;
    std::vector<uint8_t> data;
  };
  EthTransaction();
  explicit EthTransaction(const TxData&);
  EthTransaction(const EthTransaction&);
  virtual ~EthTransaction();
  bool operator==(const EthTransaction&) const;

  static base::Optional<EthTransaction> FromValue(const base::Value& value);

  uint8_t type() const { return type_; }

  uint256_t nonce() const { return nonce_; }
  uint256_t gas_price() const { return gas_price_; }
  uint256_t gas_limit() const { return gas_limit_; }
  EthAddress to() const { return to_; }
  uint256_t value() const { return value_; }
  std::vector<uint8_t> data() const { return data_; }
  uint8_t v() const { return v_; }
  std::vector<uint8_t> r() const { return r_; }
  std::vector<uint8_t> s() const { return s_; }

  void set_nonce(uint256_t nonce) { nonce_ = nonce; }
  void set_gas_price(uint256_t gas_price) { gas_price_ = gas_price; }
  void set_gas_limit(uint256_t gas_limit) { gas_limit_ = gas_limit; }

  // return
  // keccack(rlp([nonce, gasPrice, gasLimit, to, value, data, chainID, 0, 0])
  // Support EIP-155 chain id
  virtual std::vector<uint8_t> GetMessageToSign(uint64_t chain_id = 0) const;

  // return rlp([nonce, gasPrice, gasLimit, to, value, data, v, r, s])
  virtual std::string GetSignedTransaction() const;

  // signature and recid will be used to produce v, r, s
  // Support EIP-155 chain id
  virtual void ProcessSignature(const std::vector<uint8_t> signature,
                                int recid,
                                uint64_t chain_id = 0);

  virtual bool IsSigned() const;

  virtual base::Value ToValue() const;

 protected:
  // type 0 would be LegacyTransaction
  uint8_t type_ = 0;

  uint256_t nonce_;
  uint256_t gas_price_;
  uint256_t gas_limit_;
  EthAddress to_;
  uint256_t value_;
  std::vector<uint8_t> data_;

  uint8_t v_ = 0;
  std::vector<uint8_t> r_;
  std::vector<uint8_t> s_;

 private:
  FRIEND_TEST_ALL_PREFIXES(EthTransactionUnitTest, GetSignedTransaction);
  FRIEND_TEST_ALL_PREFIXES(EthTransactionUnitTest, TransactionAndValue);
  FRIEND_TEST_ALL_PREFIXES(Eip2930TransactionUnitTest, GetSignedTransaction);
};

}  // namespace brave_wallet
#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_TRANSACTION_H_
