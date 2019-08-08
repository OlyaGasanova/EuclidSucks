#ifndef H_ARRAY
#define H_ARRAY

template <typename T>
struct Array {
    int capacity;
    int length;
    T   *items;

    Array(int capacity = 32) : capacity(capacity), length(0), items(NULL) {}

    ~Array() { 
        clear();
    }

    void reserve(int capacity) {
        this->capacity = capacity;
        if (items)
            items = (T*)realloc(items, capacity * sizeof(T));
        else
            items = (T*)malloc(capacity * sizeof(T));
    }

    int push(const T &item) {
        if (!items) {
            items = (T*)malloc(capacity * sizeof(T));
        }

        if (length == capacity) {
            reserve(capacity + capacity / 2);
        }

        items[length] = item;
        return length++;
    }

    int pop() {
        ASSERT(length > 0);
        return --length;
    }

    void removeFast(int index) {
        (*this)[index] = (*this)[length - 1];
        length--;
    }

    void remove(int index) {
        length--;
        ASSERT(length >= 0);
        for (int i = index; i < length; i++) {
            items[i] = items[i + 1];
        }
    }

    void resize(int length) {
        if (capacity < length) {
            reserve(length);
        }
        this->length = length;
    }

    void clear() {
        length = 0;
        free(items);
        items = NULL;
    }

    void sort() {
        ::sort(items, length);
    }

    T& operator[] (int index) {
        ASSERT(index >= 0 && index < length);
        return items[index]; 
    };

    operator T*() const { return items; };
};

#endif
