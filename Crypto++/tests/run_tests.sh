#!/usr/bin/env bash
# Запуск проверок ядра (без Qt): векторы из RFC 8891 / 7801 / 6986 и тесты файловых операций.
# Нужны g++ и Crypto++.   Использование:  ./tests/run_tests.sh
set -e
cd "$(dirname "$0")"
CORE=../src/core
for t in test_magma_rfc8891 test_kuznyechik_rfc7801 test_streebog_rfc6986 test_gost_files test_core_aes_rsa_ecdsa; do
    echo "=== $t ==="
    g++ -std=c++17 -O2 -I$CORE $t.cpp -o /tmp/$t -lcryptopp
    /tmp/$t | grep -E "^\[FAIL\]|PASSED|FAILURES"
done
