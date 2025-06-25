#include <iostream>


class mempool {

private:
    struct Block {
        Block* next;
    };
    size_t block_size;
    size_t block_count;
    Block* freelist = nullptr;
    char* memoery = nullptr;

public:
    mempool(size_t size, size_t count);
    ~mempool();
    void* allocate();
    void deallocate(void* ptr);

};
mempool::mempool(size_t size, size_t count) :
    block_size(std::max(size, sizeof(Block))), block_count(count) {

    memoery = new char[block_size*block_count];
    freelist = reinterpret_cast<Block*>(memoery);
    
    Block* current = freelist;
    for (int i = 0; i < count - 1; i++ ) {
        Block* next = reinterpret_cast<Block*>(memoery + block_size * (i + 1));
        current->next = next;
        current = current -> next;
    }
    current->next = nullptr;
}
mempool::~mempool() {
    delete[] memoery;
}

void* mempool::allocate() {
    if (freelist == nullptr) {
        std::cout << "memory space not avail" << std::endl;
        return  nullptr;
    }
    Block* block;
    block = freelist;
    freelist = freelist->next;
    return block;
}

void mempool::deallocate(void* ptr) {
    if (!ptr) {
        return;
    }
    Block* block = reinterpret_cast<Block*>(ptr);
    block->next = freelist;
    freelist = block;
}

int main() {
    mempool pool(sizeof(int), 100);
    
    int* p1 = static_cast<int*>(pool.allocate());
    *p1 = 42;
    std::cout << *p1 << std::endl;
    
    int* p2 = static_cast<int*>(pool.allocate());
    *p2 = 84;
    std::cout << *p2 << std::endl;
    
    pool.deallocate(p1);
    pool.deallocate(p2);
    return 0;

}