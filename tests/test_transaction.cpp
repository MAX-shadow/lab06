#include <gtest/gtest.h>
#include <stdexcept>

#include "Account.h"
#include "Transaction.h"

TEST(TransactionTest, SameAccountThrows) {
  Account from(1, 1000);
  Account to(1, 1000);
  Transaction tx;
  EXPECT_THROW(tx.Make(from, to, 100), std::logic_error);
}

TEST(TransactionTest, NegativeSumThrows) {
  Account from(1, 1000);
  Account to(2, 1000);
  Transaction tx;
  EXPECT_THROW(tx.Make(from, to, -100), std::invalid_argument);
}

TEST(TransactionTest, TooSmallSumThrows) {
  Account from(1, 1000);
  Account to(2, 1000);
  Transaction tx;
  EXPECT_THROW(tx.Make(from, to, 50), std::logic_error);
}

TEST(TransactionTest, FeeTooBigReturnsFalse) {
  Account from(1, 1000);
  Account to(2, 1000);
  Transaction tx;
  tx.set_fee(60);
  EXPECT_FALSE(tx.Make(from, to, 100));
}

TEST(TransactionTest, SuccessfulTransfer) {
  Account from(1, 1000);
  Account to(2, 1000);
  Transaction tx;
  EXPECT_TRUE(tx.Make(from, to, 100));
}

TEST(TransactionTest, FeeAccessors) {
  Transaction tx;
  tx.set_fee(5);
  EXPECT_EQ(tx.fee(), 5);
}
