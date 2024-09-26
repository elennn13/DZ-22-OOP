#include <iostream>

// Реализация пользовательского unique_ptr
// UniquePtr является владельцем динамически выделенного 
// объекта и удаляет его в деструкторе.Копирование UniquePtr 
// запрещено, но разрешено перемещение.
template <typename T>
class UniquePtr { // 
public:
    explicit UniquePtr(T* ptr = nullptr) : m_ptr(ptr) {}

    ~UniquePtr() {
        delete m_ptr;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete m_ptr;
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    T* get() const noexcept {
        return m_ptr;
    }

    T* operator->() const {
        return m_ptr;
    }

    T& operator*() const {
        return *m_ptr;
    }

    explicit operator bool() const noexcept {
        return m_ptr != nullptr;
    }

private:
    T* m_ptr;
};

// Реализация пользовательского shared_ptr
// SharedPtr использует счетчик ссылок для 
// отслеживания количества указателей, ссылающихся 
// на один и тот же объект. Объект удаляется, когда 
// счетчик ссылок доходит до 0. Копирование и перемещение 
// SharedPtr разрешены
template <typename T>
class SharedPtr {
public:
    SharedPtr(T* ptr = nullptr) : m_ptr(ptr), m_count(ptr ? new long(1) : nullptr) {}

    ~SharedPtr() {
        decrement_and_destroy();
    }

    SharedPtr(const SharedPtr& other) noexcept : m_ptr(other.m_ptr), m_count(other.m_count) {
        if (m_count) {
            ++(*m_count);
        }
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this != &other) {
            decrement_and_destroy();
            m_ptr = other.m_ptr;
            m_count = other.m_count;
            if (m_count) {
                ++(*m_count);
            }
        }
        return *this;
    }

    SharedPtr(SharedPtr&& other) noexcept : m_ptr(other.m_ptr), m_count(other.m_count) {
        other.m_ptr = nullptr;
        other.m_count = nullptr;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            decrement_and_destroy();
            m_ptr = other.m_ptr;
            m_count = other.m_count;
            other.m_ptr = nullptr;
            other.m_count = nullptr;
        }
        return *this;
    }

    T* get() const noexcept {
        return m_ptr;
    }

    T* operator->() const {
        return m_ptr;
    }

    T& operator*() const {
        return *m_ptr;
    }

    long use_count() const noexcept {
        return m_count ? *m_count : 0;
    }

    explicit operator bool() const noexcept {
        return m_ptr != nullptr;
    }

private:
    void decrement_and_destroy() {
        if (m_count && --(*m_count) == 0) {
            delete m_ptr;
            delete m_count;
        }
    }

    T* m_ptr;
    long* m_count;
};

int main() {
    // Тестирование UniquePtr
    {
        UniquePtr<int> ptr(new int(42));
        std::cout << "*ptr = " << *ptr << std::endl;
    }
    // Деструктор UniquePtr удалил динамически выделенный объект

    // Тестирование SharedPtr
    {
        SharedPtr<int> ptr1(new int(42));
        SharedPtr<int> ptr2 = ptr1;
        std::cout << "*ptr1 = " << *ptr1 << ", use_count = " << ptr1.use_count() << std::endl;
        std::cout << "*ptr2 = " << *ptr2 << ", use_count = " << ptr2.use_count() << std::endl;
    }
    // Деструкторы SharedPtr уменьшили счетчик ссылок до 0 и удалили динамически выделенный объект

    return 0;
}