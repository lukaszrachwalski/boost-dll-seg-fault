// Compilation
// cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
// cmake --build build --parallel --target all

#include <cassert>
#include <cstring>
#include <boost/dll/shared_library.hpp>

void working_case()
{
    using FuncT = std::size_t ( const char* str );

    boost::dll::shared_library lib;
    boost::system::error_code err;

    lib.load("/lib/x86_64-linux-gnu/libc.so.6", err);
    assert(lib.is_loaded() && !err.failed());


    FuncT* func = lib.get<FuncT>("strlen");
    assert(func != nullptr);

    const char text[] = "Hello, World";
    const auto len = func(text);
    assert(len == std::strlen(text));
}

void problematic_case()
{
    using FuncPtrT = std::size_t (*)( const char* str );    // Change 1. Function pointer instead of function type

    boost::dll::shared_library lib;
    boost::system::error_code err;

    lib.load("/lib/x86_64-linux-gnu/libc.so.6", err);
    assert(lib.is_loaded() && !err.failed());


    FuncPtrT func = lib.get<FuncPtrT>("strlen");    // Change 2. No asterix
    assert(func != nullptr);

    const char text[] = "Hello, World";
    const auto len = func(text);                // Boom! Segmentation fault!
    assert(len == std::strlen(text));

    /*
     * I have performed little investigation:
     *
     * in shared_library.hpp line 370: this overload get function is matched:
     *
     * //! \overload T& get(const std::string& symbol_name) const
    template <typename T>
    inline typename boost::disable_if_c<boost::is_member_pointer<T>::value || boost::is_reference<T>::value, T&>::type get(const char* symbol_name) const {
        return *boost::dll::detail::aggressive_ptr_cast<T*>(
            get_void(symbol_name)
        );
    }
     *
     * instead of this one:
     *
     * //! \overload T& get(const std::string& symbol_name) const
    template <typename T>
    inline typename boost::enable_if_c<boost::is_member_pointer<T>::value || boost::is_reference<T>::value, T>::type get(const char* symbol_name) const {
        return boost::dll::detail::aggressive_ptr_cast<T>(
            get_void(symbol_name)
        );
    }
     *
     *
     * IMHO the condition in enable/disable if (SFINAE) is not correct. There is missing boost::is_pointer<T>::value.
     * That would fix my problem, but I suppose there could be other cases.
     *
    */
}

int main(int /*argc*/, char* /*argv*/[])
{
    working_case();
    problematic_case();
}
