cmake_minimum_required(VERSION 3.21)
project(CryptoQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    add_compile_options(/utf-8 /Zc:__cplusplus)   # исходники в UTF-8 (русские строки)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

# Crypto++: vcpkg (cryptopp::cryptopp) или системный пакет через pkg-config (Linux)
find_package(cryptopp CONFIG QUIET)
if(TARGET cryptopp::cryptopp)
    set(CRYPTO_LIB cryptopp::cryptopp)
elseif(TARGET cryptopp::cryptopp-static)
    set(CRYPTO_LIB cryptopp::cryptopp-static)
elseif(TARGET cryptopp::cryptopp-shared)
    set(CRYPTO_LIB cryptopp::cryptopp-shared)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(CRYPTOPP REQUIRED IMPORTED_TARGET libcryptopp libcrypto++)
    set(CRYPTO_LIB PkgConfig::CRYPTOPP)
endif()

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.h)

qt_add_executable(CryptoQt WIN32 MACOSX_BUNDLE src/main.cpp ${HEADERS})
target_include_directories(CryptoQt PRIVATE src)
target_link_libraries(CryptoQt PRIVATE Qt6::Widgets Qt6::Concurrent ${CRYPTO_LIB})
