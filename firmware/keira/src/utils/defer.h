#pragma once

#include <functional>

class Defer {
public:
    explicit Defer(std::function<void()> func);
    ~Defer();

private:
    std::function<void()> _func;
};
