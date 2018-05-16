//
// Created by ruoshui on 4/23/18.
//

#ifndef PROJECT_SKIPLIST_H
#define PROJECT_SKIPLIST_H


#include <atomic>
#include <cassert>
#include "random.h"
#include "arena.h"
#include "atomic_ptr.h"

namespace gamtools {

    class Arena;

    template <typename Key, class Comparator>
    class SkipList {
    private:
        struct Node;
    public:
        explicit SkipList(Comparator cmp, Arena *arena);
        SkipList(const SkipList&) = delete;
        void operator=(const SkipList&) = delete;
        void Insert(const Key &key);
        bool Contains(const Key &key);

        class Iterator {
        public:
            explicit Iterator(const SkipList *list);
            bool Valid() const;
            void Next();
            const Key& key() const;
            void SeekToFirst();
        private:
            const SkipList *list_;
            Node *node_;
        };
    private:
        enum {kMaxHeight = 12};
        Comparator const cmp_;
        Arena *const arena_;
        Node *const head_;
        AtomicPointer max_height_;
        Random rnd_;
        Node *NewNode(const Key &key, int height);
        inline int GetMaxHeight() const {
            return static_cast<int>(
                    reinterpret_cast<intptr_t>(max_height_.NoBarrier_Load()));
        }
        int RandomHeight();
        Node *FindGreater(const Key &key, Node **prev) const;
        bool KeyIsAfterNode(const Key& key, Node* n) const;
        bool Equal(const Key& a, const Key& b) const { return (cmp_(a, b) == 0); }


    };


    template <typename Key, class Comparator>
    inline SkipList<Key, Comparator>::Iterator::Iterator(const SkipList *list) {
        list_ = list;
        node_ = nullptr;
    }

    template <typename Key, class Comparator>
    inline bool SkipList<Key, Comparator>::Iterator::Valid() const {
        return node_ != nullptr;
    }

    template <typename Key, class Comparator>
    inline const Key& SkipList<Key, Comparator>::Iterator::key() const {
        return node_->key;
    }

    template <typename Key, class Comparator>
    inline void SkipList<Key, Comparator>::Iterator::Next() {
        node_ = node_->Next(0);
    }

    template <typename Key, class Comparator>
    inline void SkipList<Key, Comparator>::Iterator::SeekToFirst() {
        node_ = list_->head_->Next(0);
    }
    template <typename Key, class Comparator>
    struct SkipList<Key, Comparator>::Node {
        explicit Node(const Key& k): key(k) {}
        Key const key;
        Node *Next(int n) {
            assert(n >= 0);
            return reinterpret_cast<Node *>(next_[n].Acquire_Load());
        }
        void SetNext(int n, Node *x) {
            assert(n >= 0);
            next_[n].Release_Store(x);
        }
        void NoBarrier_SetNext(int n, Node *x){
            assert(n >= 0);
            next_[n].NoBarrier_Store(x);
        }
        Node *NoBarrier_Next(int n) {
            assert(n >= 0);
            return reinterpret_cast<Node *>(next_[n].NoBarrier_Load());
        }
    private:
        AtomicPointer next_[1];
    };


    template <typename Key, class Comparator>
    typename SkipList<Key, Comparator>::Node*
    SkipList<Key, Comparator>::NewNode(const Key &key, int height) {
        char *mem = arena_->AllocateAligned(sizeof(Node) + sizeof(AtomicPointer) * (height - 1));
        return new (mem) Node(key);
    }


    template <typename Key, class Comparator>
    void SkipList<Key, Comparator>::Insert(const Key &key) {
        Node* prev[kMaxHeight];
        Node* x = FindGreater(key, prev);
//        assert(x == nullptr || cmp_(key, x->key) < 0 );
        int height = RandomHeight();
        if (height > GetMaxHeight()) {
            for (int i = GetMaxHeight(); i < height; ++i) {
                prev[i] = head_;
            }
            max_height_.NoBarrier_Store(reinterpret_cast<void *>(height));
        }

        x = NewNode(key, height);
        for (int i = 0; i < height; i++) {
            // NoBarrier_SetNext() suffices since we will add a barrier when
            // we publish a pointer to "x" in prev[i].
            x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
            prev[i]->SetNext(i, x);
        }

    }

    template <typename Key, class Comparator>
    bool SkipList<Key, Comparator>::Contains(const Key &key) {
        Node* x = FindGreater(key, nullptr);
        if (x != nullptr && Equal(key, x->key)) {
            return true;
        } else {
            return false;
        }

    }

    template<typename Key, class Comparator>
    int SkipList<Key,Comparator>::RandomHeight() {
        // Increase height with probability 1 in kBranching
        static const unsigned int kBranching = 4;
        int height = 1;
        while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0)) {
            height++;
        }
        assert(height > 0);
        assert(height <= kMaxHeight);
        return height;
    }

    template <typename Key, class Comparator>
    typename  SkipList<Key, Comparator>::Node *
    SkipList<Key, Comparator>::FindGreater(const Key &key, Node **prev) const {
        Node *x = head_;
        int level = GetMaxHeight() - 1;
        while (true) {
            Node *next = x->Next(level);
            if (KeyIsAfterNode(key, next)) {
                x = next;
            } else {
                if (prev != nullptr) prev[level] = x;
                if (level == 0) {
                    return next;
                } else {
                    level--;
                }
            }
        }

    }

    template<typename Key, class Comparator>
    bool SkipList<Key,Comparator>::KeyIsAfterNode(const Key& key, Node* n) const {
        // null n is considered infinite
        return (n != nullptr) && (cmp_(n->key, key) < 0);
    }

    template<typename Key, class Comparator>
    SkipList<Key, Comparator>::SkipList(Comparator cmp, Arena *arena)
            :cmp_(cmp),
             arena_(arena),
             head_(NewNode(0, kMaxHeight)),
             max_height_(reinterpret_cast<void *>(1)),
             rnd_(0xdeadbeef) {
        for (int i = 0; i < kMaxHeight; ++i) {
            head_->SetNext(i, nullptr);
        }
    }
}


#endif //PROJECT_SKIPLIST_H
