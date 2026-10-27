# Crypto++ Utility (Qt 6)

Крипто-утилита: AES, RSA (гибридное шифрование), SHA-256, ECDSA и российские стандарты ГОСТ:
Магма и Кузнечик (ГОСТ Р 34.12-2015), Стрибог (ГОСТ Р 34.11-2012).
Криптография — Crypto++, интерфейс — Qt 6 Widgets (тёмная/светлая тема, боковая навигация,
карточки, перетаскивание файлов в поля, фоновые операции с индикатором).

## Структура

```
src/
  main.cpp                 — только точка входа
  core/                    — криптография и утилиты, КАЖДАЯ ФУНКЦИЯ В СВОЁМ .h (без Qt и WinAPI)
    magma/ kuznyechik/ streebog/ — ГОСТ-алгоритмы, написаны с нуля (в Crypto++ их нет)
  gui/                     — интерфейс, каждая функция/структура/класс в своём .h
```

## ГОСТ-алгоритмы

Crypto++ не содержит Магму, Кузнечик и Стрибог, поэтому они реализованы в проекте по спецификациям
**RFC 8891** (Магма), **RFC 7801** (Кузнечик) и **RFC 6986** (Стрибог). Таблицы замен, матрица A и константы
перенесены из RFC, реализация проверена по их тестовым векторам (`tests/run_tests.sh`):
преобразования t и g, ключевое расписание, шифрование/расшифровка блока, таблица π и обратная π⁻¹,
L и L⁻¹, раундовые константы, хеши обоих тестовых сообщений для 256 и 512 бит.

- Магма: блок 64 бита, ключ 256 бит; Кузнечик: блок 128 бит, ключ 256 бит. Режим CBC, дополнение PKCS#7,
  случайный IV (Crypto++ `AutoSeededRandomPool`) в начале файла. Файлы: `.magma`, `.kuz`.
- Стрибог: 256 и 512 бит, потоковое хеширование файла любого размера.
- Таблица π у Кузнечика и Стрибога одна и та же (`core/GostPi.h`).
- Все данные ГОСТ-страниц и SHA-256 (ключ, IV, начало шифртекста, хеш) выводятся в **двоичном виде**
  (`core/toBinaryString.h`).
- Байты хеша Стрибога выводятся в порядке памяти (как у OpenSSL/gostsum). В RFC 6986 те же значения
  записаны как большое число, то есть в обратном порядке байт.
- Магма, Кузнечик и Стрибог не входят в сертифицированные СКЗИ; реализация учебная.

## Сборка

Нужны: Qt 6 (Widgets, Concurrent), Crypto++, CMake ≥ 3.21, компилятор C++17.

**Windows (Visual Studio / Qt Creator + vcpkg):**
```
vcpkg install cryptopp:x64-windows
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=<путь к Qt>/msvc2022_64
cmake --build build --config Release
```
Можно просто открыть папку в Visual Studio (File → Open → Folder) или `CMakeLists.txt` в Qt Creator.
Для запуска рядом с .exe нужны DLL Qt — используйте `windeployqt build/Release/CryptoQt.exe`.

**Linux:** `sudo apt install qt6-base-dev libcrypto++-dev cmake g++ pkg-config`, затем
`cmake -S . -B build && cmake --build build`.

Исходники в UTF-8 (в CMake для MSVC включён `/utf-8`).

## Где теперь функции из CryptoGUI.cpp

| Было | Стало |
|---|---|
| `formatFileSize`, `getCurrentTime`, `fileExists`, `class Timer` | `core/formatFileSize.h`, `getCurrentTime.h`, `fileExists.h`, `Timer.h` |
| `Log` | `core/Log.h` (+ `logSinkStorage.h`, `setLogSink.h`); GUI подключает журнал через `setLogSink` |
| `saveRSAKey` ×2 / `loadRSAKey` ×2 | `saveRSAPrivateKey.h`, `saveRSAPublicKey.h`, `loadRSAPrivateKey.h`, `loadRSAPublicKey.h` |
| `saveECDSAKey` ×2 / `loadECDSAKey` ×2 | `saveECDSAPrivateKey.h`, `saveECDSAPublicKey.h`, `loadECDSAPrivateKey.h`, `loadECDSAPublicKey.h` |
| `AESGenerateKey`, `AESEncrypt`, `AESDecrypt` | одноимённые `core/*.h` |
| `RSAGenerateKeys`, `RSAEncrypt`, `RSADecrypt` | одноимённые `core/*.h` |
| `HashFile` | `core/HashFile.h` (теперь *возвращает* текст результата) |
| `ECDSAGenerateKeys`, `ECDSASign`, `ECDSAVerify` | одноимённые `core/*.h` (`ECDSAVerify` возвращает `bool`) |
| `SetHashResult` | не нужна: результат `HashFile` показывает страница SHA-256 |
| `BrowseForFile`, `BrowseForFolder` | `gui/browseForFile.h`, `browseForSaveFile.h`, `browseForFolder.h` |
| `CreateMainWindow` | `gui/createMainWindow.h` |
| `CreateMainButtons` | `gui/createSidebar.h` |
| `CreateLogWindow` | `gui/createLogPanel.h` |
| `CreateAESControls` / `CreateRSAControls` / `CreateHashControls` / `CreateECDSAControls` | `gui/createAesPage.h` / `createRsaPage.h` / `createHashPage.h` / `createEcdsaPage.h` |
| `ShowControls` | `QStackedWidget` (переключение страниц) |
| `WndProc` | сигналы/слоты Qt внутри страниц |
| `WinMain` | `main.cpp` |

ГОСТ-функции: `core/magma/` (`magmaT`, `magmaG`, `magmaExpandKey`, `magmaEncryptBlock`, `magmaDecryptBlock`,
`MagmaGenerateKey`, `MagmaEncrypt`, `MagmaDecrypt`), `core/kuznyechik/` (`kuznGfMul`, `kuznLinear`, `kuznR`, `kuznRInv`,
`kuznL`, `kuznLInv`, `kuznLSX`, `kuznExpandKey`, `kuznEncryptBlock`, `kuznDecryptBlock`, `Kuznyechik*`),
`core/streebog/` (`streebogL`, `streebogLPS`, `streebogE`, `streebogG`, `streebogInit/Update/Final`,
`computeFileStreebog`, `HashFileStreebog`). Общие для режима CBC: `cbcEncryptGeneric`, `cbcDecryptGeneric`.

Новые вспомогательные функции, которых не было в оригинале: `computeFileSHA256`, `readAESKeyFile`,
`cbcEncryptStream`, `cbcDecryptStream`, `removeFileQuietly`, `pathToUtf8`, `formatMs`, `ecdsaPublicKeyPathFor`
(убирают копипасту между AES/RSA и Sign/Verify), а в `gui/` — `runTask` (фоновый поток), `applyTheme` и др.

## Что изменено в логике (важно)

1. **Исправлена ошибка шифрования файлов > 64 КБ.** В оригинале каждый 64-КБ блок шифровался отдельным
   `StreamTransformationFilter`, и в середину шифртекста попадали лишние блоки паддинга; при расшифровке
   они оставались в файле. Теперь шифрование потоковое, паддинг добавляется один раз в конце.
   **Файлы, зашифрованные старой версией, новой не расшифровываются** (формат `.aes`/`.rsa` тот же по структуре,
   но содержимое блоков отличается).
2. Тяжёлые операции (особенно генерация RSA 8192/16384) идут в фоновом потоке — окно не «замирает».
3. Пути обрабатываются через `std::filesystem` (кириллица в путях, нет жёстких `\\`).
4. Добавлены проверки: размер/формат зашифрованного файла, неверный ключ, недописанный результат удаляется.
5. ECDSA: поле «Открытый ключ» выбирается явно (раньше проверка работала только сразу после генерации ключей).
6. Формат ECDSA-подписи, формат RSA-контейнера и хеш SHA-256 — как в оригинале.

Заметка для отчёта: AES-CBC не обеспечивает целостность — при неверном ключе в ~1/256 случаев паддинг
«случайно» окажется корректным и получится мусор вместо ошибки. Для защиты от этого используют AES-GCM.
