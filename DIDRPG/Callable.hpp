#ifndef CALLABLE_HPP
#define CALLABLE_HPP

template<class Handler, size_t offset, class Ret, class... Args> class Callable {
public:
	using PtrType = Ret(*)(Handler&, Args...);

	PtrType ptr;

	inline Handler& self() {
		return *reinterpret_cast<Handler*>(reinterpret_cast<unsigned char*>(this) - offset);
	}
	inline const Handler& self() const {
		return *reinterpret_cast<const Handler*>(reinterpret_cast<const unsigned char*>(this) - offset);
	}

public:
	Callable(nullptr_t np) : ptr(nullptr) {}
	Callable(PtrType pointer) : ptr(pointer) {}

	inline Ret operator()(Args&&... args) const {
		if (ptr != nullptr)
			return (*ptr)(self(), std::forward<Args>(args)...);
		else throw std::bad_function_call{};
	}
	inline Ret operator()(Args&&... args) {
		if (ptr != nullptr)
			return (*ptr)(self(), std::forward<Args>(args)...);
		else throw std::bad_function_call{};
	}
	inline bool operator==(nullptr_t np) const {
		return ptr == np;
	}
	inline bool operator!=(nullptr_t np) const {
		return ptr != np;
	}
};

#endif // !CALLABLE_HPP
