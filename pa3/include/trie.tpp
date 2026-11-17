#ifndef TRIE_TPP
#define TRIE_TPP

#ifndef TRIE_H
#include "trie.h"
#endif

#include <algorithm>
#include <cassert>
#include <utility>

template<typename T, typename Key, std::size_t N>
Trie<T, Key, N>::Trie(Trie::KeyFn key_fn, Trie::BucketFn bucket_fn) {
  // TODO
}

template<typename T, typename Key, std::size_t N>
Trie<T, Key, N>::~Trie() {
  // TODO
}

template<typename T, typename Key, std::size_t N>
Trie<T, Key, N>::Trie(const Trie& other) {
  // TODO
}

template<typename T, typename Key, std::size_t N>
Trie<T, Key, N>& Trie<T, Key, N>::operator=(const Trie& other) {
  // TODO
  return *this;
}

template<typename T, typename Key, std::size_t N>
void Trie<T, Key, N>::clear() {
  // TODO
}

template<typename T, typename Key, std::size_t N>
typename Trie<T, Key, N>::size_type Trie<T, Key, N>::size() const {
  // TODO
  return 0;
}

// template<typename T, typename Key, std::size_t N>
// void Trie<T, Key, N>::copy_from(const Trie& other) {
//   // TODO(Optional)
// }

template<typename T, typename Key, std::size_t N>
bool Trie<T, Key, N>::erase_from(const Key& key) {
  // TODO
  return false;
}

template<typename T, typename Key, std::size_t N>
bool Trie<T, Key, N>::insert(const Key& key, const T& val) {
  // TODO
  return false;
}

// template<typename T, typename Key, std::size_t N>
// void Trie<T, Key, N>::push_down() {
//   // TODO(Optional)
// }

// template<typename T, typename Key, std::size_t N>
// Trie<T, Key, N>*& Trie<T, Key, N>::child(const Key& key) {
//   // TODO(Optional)
// }

// template<typename T, typename Key, std::size_t N>
// bool Trie<T, Key, N>::is_value() const {
//   // TODO(Optional)
//   return false;
// }

template<typename T, typename Key, std::size_t N>
bool Trie<T, Key, N>::is_empty() const {
  // TODO
  return false;
}

template<typename T, typename Key, std::size_t N>
TrieIterator<T, Key, N>::TrieIterator(TrieIterator::NodePtr root) {
  // TODO
}

template<typename T, typename Key, std::size_t N>
TrieIterator<T, Key, N>::TrieIterator(NodePtr root, const Key& key) {
  // TODO
}

template<typename T, typename Key, std::size_t N>
bool TrieIterator<T, Key, N>::operator==(const TrieIterator& other) const {
  // TODO
  return false;
}

template<typename T, typename Key, std::size_t N>
TrieIterator<T, Key, N>& TrieIterator<T, Key, N>::operator++() {
  // TODO
  return *this;
}

template<typename T, typename Key, std::size_t N>
typename TrieIterator<T, Key, N>::reference TrieIterator<T, Key, N>::operator*() const {
  // TODO
  pointer ptr{nullptr};
  return *ptr;
}

// template<typename T, typename Key, std::size_t N>
// bool TrieIterator<T, Key, N>::scan_siblings(std::add_const_t<NodePtr> node) {
//   // TODO(Optional)
//   return false;
// }

// template<typename T, typename Key, std::size_t N>
// void TrieIterator<T, Key, N>::descend() {
//   // TODO(Optional)
// }

#endif  // TRIE_TPP
