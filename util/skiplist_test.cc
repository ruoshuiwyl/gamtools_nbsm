//
// Created by ruoshui on 4/24/18.
//


#include <gtest/gtest.h>
#include "util/arena.h"
#include "util/skiplist.h"

namespace gamtools{
    class SkipTest {
    };

    typedef uint64_t Key;

    struct Comparator {
        int operator()(const Key &a, const Key &b) const {
            if (a < b) {
                return -1;
            } else if (a > b) {
                return +1;
            } else {
                return 0;
            }
        }
    };

    TEST(SkipTest, Empty) {
        Arena arena;
        Comparator cmp;
        SkipList<Key, Comparator> list(cmp, &arena);
        SkipList<Key, Comparator>::Iterator iter(&list);
        ASSERT_TRUE(!iter.Valid());
        iter.SeekToFirst();
        ASSERT_TRUE(!iter.Valid());
    }

    TEST(SkipList, InsertAndLookup) {
        const int N = 2000;
        const int R = 5000;
        Random rnd(1000);
        std::set<Key> keys;
        Arena arena;
        Comparator cmp;
        SkipList<Key, Comparator> list(cmp, &arena);
        for (int i = 0; i < N; i++) {
            Key key = rnd.Next() % R;
            if (keys.insert(key).second) {
                list.Insert(key);
            }
        }
        for (int i = 0; i < R; i++) {
            if (list.Contains(i)) {
                ASSERT_EQ(keys.count(i), 1);
            } else {
                ASSERT_EQ(keys.count(i), 0);
            }
        }
    }

    TEST(SkipList, MultiValue) {
        Arena arena;
        Comparator cmp;
        SkipList<int, Comparator> list(cmp, &arena);
        std::vector<int> values = {3, 3, 4, 4, 5, 4, 5, 7};
        for (auto value : values) {
            list.Insert(value);
        }
        SkipList<int, Comparator>::Iterator iter(&list);
        for (iter.SeekToFirst(); iter.Valid(); iter.Next()) {
            assert(iter.Valid());
            std::cout << iter.key() << std::endl;
        }
    }


}




int main(int argc, char *argv[] ) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}