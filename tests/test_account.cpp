#include <gtest/gtest.h>
#include <stdexcept>

#include "Account.h"

TEST(AccountTest, ConstructorAndGetBalance) {
  Account acc(1, 100);
  EXPECT_EQ(acc.id(), 1);
  EXPECT_EQ(acc.GetBalance(), 100);
}

TEST(AccountTest, ChangeBalanceWithoutLockThrows) {
  Account acc(1, 100);
  EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
}

TEST(AccountTest, ChangeBalanceAfterLock) {
  Account acc(1, 100);
  acc.Lock();
  acc.ChangeBalance(50);
  EXPECT_EQ(acc.GetBalance(), 150);
}

TEST(AccountTest, DoubleLockThrows) {
  Account acc(1, 100);
  acc.Lock();
  EXPECT_THROW(acc.Lock(), std::runtime_error);
}

TEST(AccountTest, UnlockAllowsRelock) {
  Account acc(1, 100);
  acc.Lock();
  acc.Unlock();
  EXPECT_NO_THROW(acc.Lock());
}
