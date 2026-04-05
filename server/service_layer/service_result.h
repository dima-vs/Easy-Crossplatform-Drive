#ifndef SERVICE_RESULT_H
#define SERVICE_RESULT_H

#include <variant>
#include <utility>

template<typename T, typename E>
struct ServiceResult
{
private:
    std::variant<T, E> m_result;

public:
    ServiceResult(T data) : m_result(std::move(data)) {}
    ServiceResult(E err) : m_result(err) {}

    bool isOk() const { return std::holds_alternative<T>(m_result); }

    const T& data() const { return std::get<T>(m_result); }
    const E& error() const { return std::get<E>(m_result); }

    static ServiceResult ok(T data) { return ServiceResult(std::move(data)); }
    static ServiceResult fail(E err) { return ServiceResult(std::move(err)); }
};

#endif // SERVICE_RESULT_H
