// TODO: Insert description here. (generated by jdanis)

#ifndef KEYSTORE_INCLUDE_KEYSTORE_UTILS_H_
#define KEYSTORE_INCLUDE_KEYSTORE_UTILS_H_

#include <iterator>
#include <memory>
#include <optional>
#include <vector>

namespace android {
namespace security {

/*
 * This iterator abstracts from a collection of the form
 * std::shared_ptr<COLLECTION_TYPE<std::optional<T>>>
 * such that it is defined both for nulled outer pointer and
 * nulled entries. If shared_ptr(nullptr) is passed in, the iterator behaves
 * like the end iterator yielding an empty collection. Nulled
 * entries are skipped so that the iterator is always dereferencable unless
 * it is equal to end.
 * The default constructor always yields an iterator equal to end.
 * The same iterator invalidation rules apply as they do for the iterators
 * of the corresponding collection.
 */
template <typename T, template <typename...> class Coll = std::vector>
class SharedNullableIterator {
  public:
    typedef Coll<std::optional<typename std::remove_const<T>::type>> CollectionType;
    typedef std::shared_ptr<CollectionType> CollectionPtr;

    SharedNullableIterator() {}
    explicit SharedNullableIterator(const std::shared_ptr<CollectionType>& coll) : coll_(coll) {
        init();
    }
    explicit SharedNullableIterator(std::shared_ptr<CollectionType>&& coll) : coll_(coll) {
        init();
    }

    SharedNullableIterator(const SharedNullableIterator& other)
        : coll_(other.coll_), cur_(other.cur_) {}
    SharedNullableIterator(SharedNullableIterator&& other) noexcept
        : coll_(std::move(other.coll_)), cur_(std::move(other.cur_)) {}

    SharedNullableIterator& operator++() {
        inc();
        return *this;
    }
    SharedNullableIterator operator++(int) {
        SharedNullableIterator retval(*this);
        ++(*this);
        return retval;
    }
    T& operator*() const { return **cur_; }

    T* operator->() const { return &**cur_; }

    bool operator==(const SharedNullableIterator& other) const {
        return cur_ == other.cur_ || (is_end() && other.is_end());
    }
    bool operator!=(const SharedNullableIterator& other) const { return !(*this == other); }

    SharedNullableIterator& operator=(const SharedNullableIterator&) = default;
    SharedNullableIterator& operator=(SharedNullableIterator&&) noexcept = default;

  private:
    inline bool is_end() const { return !coll_ || cur_ == coll_->end(); }
    inline void inc() {
        if (!is_end()) {
            do {
                ++cur_;
                // move forward to the next non null member or stay at end
            } while (cur_ != coll_->end() && !(*cur_));
        }
    }
    void init() {
        if (coll_) {
            // move forward to the first non null member
            for (cur_ = coll_->begin(); cur_ != coll_->end() && !(*cur_); ++cur_) {
            }
        }
    }

    CollectionPtr coll_;
    typename CollectionType::iterator cur_;
};

}  // namespace security
}  // namespace android

namespace std {
template <typename T, template <typename...> class COLL>
struct iterator_traits<android::security::SharedNullableIterator<T, COLL>> {
    typedef T& reference;
    typedef T value_type;
    typedef T* pointer;
    typedef forward_iterator_tag iterator_category;
};
}

#endif  // KEYSTORE_INCLUDE_KEYSTORE_UTILS_H_
