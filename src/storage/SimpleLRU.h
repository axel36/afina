#include <utility>

#ifndef AFINA_STORAGE_SIMPLE_LRU_H
#define AFINA_STORAGE_SIMPLE_LRU_H

#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>

#include <afina/Storage.h>

namespace Afina {
namespace Backend {

/**
 * # Map based implementation
 * That is NOT thread safe implementaiton!!
 */
class SimpleLRU : public Afina::Storage {
public:
    SimpleLRU(size_t max_size = 1024) :
    _max_size(max_size),
    _current_size(0),
    _lru_head(nullptr),
    _lru_tail(nullptr)
    {}

    ~SimpleLRU() {

        if (_lru_head == nullptr) {
            return;
        }
        _lru_index.clear();

        lru_node *node_to_delete;

        while(_lru_head->next != nullptr )
        {
            node_to_delete = _lru_head.get();
            _lru_head.swap(_lru_head->next);
            delete node_to_delete;
        }
        node_to_delete = _lru_head.release();
        delete node_to_delete;
    }

    // Implements Afina::Storage interface
    bool Put(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool PutIfAbsent(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool Set(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool Delete(const std::string &key) override;

    // Implements Afina::Storage interface
    bool Get(const std::string &key, std::string &value) const override;

private:
    // LRU cache node
    using lru_node = struct lru_node {
        std::string key;
        std::string value;
        std::unique_ptr<lru_node> next;
        lru_node *prev;
        lru_node (const std::string &k,const std::string &v)
        :key(k),
        value(v),
        prev(nullptr),
        next(nullptr)
        {

        }
    };

    // Maximum number of bytes could be stored in this cache.
    // i.e all (keys+values) must be less the _max_size
    std::size_t _max_size;

    std::size_t _current_size;

    // Main storage of lru_nodes, elements in this list ordered descending by "freshness": in the head
    // element that wasn't used for longest time.
    //
    // List owns all nodes
    std::unique_ptr<lru_node> _lru_head;
    lru_node * _lru_tail;

    // Index of nodes from list above, allows fast random access to elements by lru_node#key
    std::unordered_map<std::string, std::reference_wrapper<lru_node>> _lru_index;

    void _swap_to_head(lru_node &recently_used);
    void _delete_less_used();
    void _insert_new_value(const std::string &, const std::string &, size_t );
};

} // namespace Backend
} // namespace Afina

#endif // AFINA_STORAGE_SIMPLE_LRU_H
