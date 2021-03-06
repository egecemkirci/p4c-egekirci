
/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include "gtest/gtest.h"
#include "lib/bitvec.h"

namespace Test {

TEST(Bitvec, Shift) {
    bitvec simple(0, 1);

    EXPECT_EQ((simple << 64).getbit(64), true);
    EXPECT_EQ((simple << 63).getbit(63), true);
    EXPECT_EQ((simple << 64).getbit(63), false);
}

#ifdef __SIZEOF_INT128__
TEST(Bitvec, bigval) {
    __int128_t val[2] = { 0, 1 };
    val[1] <<= 100;
    bitvec bv(val[1]);
    EXPECT_EQ(bv.getbit(100), true);
    val[1] <<= 10;
    bv.setraw(val[1]);
    EXPECT_EQ(bv.getbit(110), true);
// older clang (<= 3.8) does not understand 'std::enable_if' in template function.
// anyone trying to use setraw() on a large int will run into compilation error with
// older clang, not a problem for gcc though.
#if (defined(__GNUC__) && !defined(__clang__)) || \
    (defined(__clang__) && (__clang_major__ >= 3) && (__clang_minor__ > 8))
    bv.setraw(val, 2);
    EXPECT_EQ(bv.getbit(238), true);
#endif   // (defined(__GNUC__) && !defined(__clang__)) ||
         // (defined(__clang__) && (__clang_major__ >= 3) && (__clang_minor__ > 8))
}
#else
TEST(Bitvec, bigval) {
    int64_t val[2] = { 0, 1 };
    val[1] <<= 60;
    bitvec bv(val[1]);
    EXPECT_EQ(bv.getbit(60), true);
    val[1] <<= 3;
    bv.setraw(val[1]);
    EXPECT_EQ(bv.getbit(63), true);
    bv.setraw(val, 2);
    EXPECT_EQ(bv.getbit(127), true);
}
#endif

TEST(Bitvec, ranges) {
    bitvec bv(0, 100);
    EXPECT_EQ(bv.popcount(), 100);
    bv.putrange(55, 20, 10);
    EXPECT_EQ(bv.popcount(), 82);
    EXPECT_EQ(bv.getrange(54, 22), (uintmax_t)0x200015);
    EXPECT_EQ(bv.ffz(0), 55U);
    EXPECT_EQ(bv.ffs(55), 56);
    EXPECT_EQ(bv.ffs(56), 56);
    EXPECT_EQ(bv.ffs(59), 75);
    EXPECT_EQ(bv.ffz(75), 100U);
    EXPECT_EQ(bv.ffs(100), -1);
}

TEST(Bitvec, getslice) {
    bitvec bv;
    for (int i = 0; i < 256; i += 32) {
        if (((i / 64) % 2) == 0) {
            bv.setrange(i, 16);
        } else {
            bv.setrange(i + 16, 16);
        }
    }
    auto slice = bv.getslice(16, 112);
    EXPECT_EQ(slice.ffs(0), 16);
    EXPECT_EQ(slice.ffz(16), 32u);
    EXPECT_EQ(slice.ffs(32), 64);
    EXPECT_EQ(slice.ffz(64), 80u);
    EXPECT_EQ(slice.ffs(80), 96);
}

}  // namespace Test
