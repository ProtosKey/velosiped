# Contributing to Velosiped System

Спасибо, что хочешь добавить что-то в **vls**. Этот документ — короткий гайд, чтобы твой PR без лишних кругов попал в `main`.

## Как поднять проект локально

Зависимости:
- C-компилятор с поддержкой **C23** (gcc 14+ или clang 18+)
- **CMake** ≥ 3.21
- **OpenSSL** (используется для MD5)
- **cJSON**

На Arch:

```sh
sudo pacman -S cmake gcc openssl cjson
```

Сборка и запуск:

```sh
cmake -S . -B build
cmake --build build
./build/vls help
```

## Тесты

В проекте есть юнит-тесты на Unity и интеграционный smoke-скрипт. Перед каждым PR прогоняй:

```sh
ctest --test-dir build --output-on-failure
```

Если добавляешь новую функцию в `utils/` — добавь юнит-тест в `tests/unit/`.
Если меняешь поведение какой-то команды (`init`, `add`, `commit`, `log`, ...) — расширь `tests/integration/smoke.sh`.

## Стиль кода

- **C23 + POSIX**, без GNU-расширений. CMake-флаги уже это обеспечивают (`_POSIX_C_SOURCE=200809L`, `_XOPEN_SOURCE=700`). Если функция не видна под этими макросами — обсуди, прежде чем тащить `_GNU_SOURCE`.
- **Все системные вызовы проверяются.** Любой `open`, `read`, `write`, `mmap` без проверки результата — нет.
- **Вывод только через `vls_safety_write` / `vls_raw` / `vls_say`** — никаких `printf`/`fprintf` в основном коде.
- **Пути собираются через `vls_join_path`**, без ручных `snprintf("%s/%s", ...)`.
- **Заголовки** инклудятся относительно `include/` (например, `#include "utils/fs.h"`), не относительно текущего файла.
- Форматирование — clang-format с дефолтными настройками (если будет добавлен `.clang-format` — применять до коммита).

## Коммиты

Используем **Conventional Commits**. Тип в начале, краткое описание в настоящем времени:

```
feat: implement vls log
fix: correct alias-safe vls_join_path
refactor: split commit pipeline
test: add commit metadata smoke checks
build: enable XSI feature-test macros
style: rename shadowed variables
```

Один коммит = одно логически законченное изменение. Не смешивай рефакторинг с фиксом бага в одном коммите — потом сложно ревьюить и откатывать.

## Pull request

- Малые PR проходят быстрее. Если меняешь больше 300 строк — подумай, нельзя ли разбить.
- В описании PR опиши **зачем**, а не **что** — что делает код, видно из диффа.
- CI должен быть зелёным: и сборка, и `ctest`. PR с падающими тестами не мерджим.
- Один из двух мейнтейнеров должен заapprove'ить перед мерджем.

## Сообщить баг или предложить фичу

Открой issue с воспроизводимым примером (init → add → commit → ...) и ожидаемым vs. фактическим поведением. Если нашёл утечку или undefined behavior — приложи вывод valgrind/sanitizer'а.
