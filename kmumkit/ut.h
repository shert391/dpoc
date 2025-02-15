#pragma once
#pragma region all
#define funcptr(func) [this] (auto... args) { this->func(args...); }
#pragma endregion