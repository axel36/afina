#include "SimpleLRU.h"
#include <memory>

namespace Afina {
namespace Backend {

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Put(const std::string &key, const std::string &value)
{
    size_t pair_size = key.size() + value.size();

    if ( pair_size > _max_size) {
        return false;
    }

    while(pair_size+_current_size > _max_size){
        SimpleLRU::_delete_less_used();
    }

    _insert_new_value(key,value, pair_size);


    return false;
}

// See MacpBasedGlobalLockImpl.h
bool SimpleLRU::PutIfAbsent(const std::string &key, const std::string &value) { return false; }

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Set(const std::string &key, const std::string &value) { return false; }

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Delete(const std::string &key) { return false; }

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Get(const std::string &key, std::string &value) const { return false; }

void SimpleLRU::_insert_new_value(const std::string &key, const std::string &value, size_t p_size)
{
    auto new_node = std::make_unique<SimpleLRU::lru_node>(key,value);

    if( _lru_tail == nullptr){
        _lru_tail = new_node.get();
        _lru_head.swap(new_node);
    }

    _lru_index.insert(std::make_pair(key,std::reference_wrapper<lru_node>(*new_node)));

    for(auto &el : _lru_index){
        e
    }

}

void SimpleLRU::_delete_less_used()
{
    _lru_index.erase(_lru_tail->key);

    _lru_tail = _lru_tail->prev;
    _lru_tail->next.reset(nullptr);

    _current_size -= (_lru_tail->key).size() + (_lru_tail->value).size();
}

void SimpleLRU::_swap_to_head( lru_node &recently_used)
{
    lru_node *previous = recently_used.prev;
    lru_node *next = recently_used.next.get();

    if (previous == nullptr) {
        return;
    }

    if (next == nullptr)
    {
        previous->next.reset(nullptr);
        _lru_tail = previous;
    } else {

        previous->next.swap(recently_used.next);
        next->prev = recently_used.prev;
    }

    _lru_head->prev = &recently_used;
    recently_used.next.swap(_lru_head);

    recently_used.prev = nullptr;
    _lru_head.reset(&recently_used);

}

} // namespace Backend
} // namespace Afina
