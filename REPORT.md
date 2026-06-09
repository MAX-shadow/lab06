# Отчет по лабораторной работе №6

**Студент:** MAX-shadow
**Тема:** Изучение средств пакетирования на примере CPack

---

## 1. Цель работы

Научиться собирать установочные пакеты для проекта с помощью CPack и
настроить автоматическую сборку пакетов при создании тега, с публикацией
их в GitHub Release.

В работе используется проект из лабораторной №5 — статическая библиотека
`banking` (классы `Account` и `Transaction`).

> Примечание: вместо Travis CI (стал платным) используется GitHub Actions, как и в предыдущих работах.

---

## 2. Ход выполнения работы

### 2.1. Подготовка проекта

Проект скопирован из лабораторной работы №5:

```sh
git clone https://github.com/MAX-shadow/lab05 lab06
cd lab06
git remote set-url origin https://github.com/MAX-shadow/lab06
```

### 2.2. Добавление версионирования в CMakeLists.txt

В корневой `CMakeLists.txt` добавлены переменные версии:

```cmake
set(BANKING_VERSION_MAJOR 0)
set(BANKING_VERSION_MINOR 1)
set(BANKING_VERSION_PATCH 0)
set(BANKING_VERSION_TWEAK 0)
set(BANKING_VERSION "${BANKING_VERSION_MAJOR}.${BANKING_VERSION_MINOR}.${BANKING_VERSION_PATCH}.${BANKING_VERSION_TWEAK}")
set(BANKING_VERSION_STRING "v${BANKING_VERSION}")
```

### 2.3. Файлы DESCRIPTION и ChangeLog.md

Создан файл `DESCRIPTION` с кратким описанием библиотеки и `ChangeLog.md`
в формате, который понимает RPM:

```
* Tue Jun 09 2026 MAX-shadow <maxsharov07@gmail.com> 0.1.0.0
- Initial RPM release
```

### 2.4. Конфигурация CPack

Создан файл `CPackConfig.cmake`. В нём указаны общие метаданные пакета
(контакт, версия, описание, лицензия), а также отдельные настройки для
RPM и DEB:

```cmake
set(CPACK_PACKAGE_CONTACT "maxsharov07@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Static C++ banking library")

set(CPACK_RPM_PACKAGE_NAME "banking-devel")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_CHANGELOG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.md)

set(CPACK_DEBIAN_PACKAGE_NAME "libbanking-dev")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "cmake (>= 3.0)")

include(CPack)
```

В корневом `CMakeLists.txt` в конце подключается этот файл:

```cmake
include(CPackConfig.cmake)
```

Чтобы заголовки и библиотека gtest не попадали в пакет, для подпроекта
отключена установка:

```cmake
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
```

### 2.5. Сборка пакетов локально

```sh
cmake -H. -B_build
cmake --build _build
cd _build
cpack -G DEB
cpack -G RPM
cpack -G TGZ
cpack -G ZIP
```

Получились пакеты:

```
lab06-0.1.0.0-Linux.deb
lab06-0.1.0.0-Linux.rpm
lab06-0.1.0.0-Linux.tar.gz
lab06-0.1.0.0-Linux.zip
```

Проверка содержимого deb-пакета:

```
$ dpkg -c lab06-0.1.0.0-Linux.deb
./usr/include/Account.h
./usr/include/Transaction.h
./usr/lib/libbanking.a

$ dpkg -I lab06-0.1.0.0-Linux.deb
 Package: libbanking-dev
 Version: 0.1.0.0-1
 Depends: cmake (>= 3.0)
```

---

## 3. Домашнее задание

Задание: настроить автоматическую сборку пакетов для коммитов, помеченных
тегом, и заливку их в GitHub Release.

Реализовано через GitHub Actions, файл `.github/workflows/ci.yml`. Логика:

- на каждый push в `master` запускается сборка и тесты (матрица gcc/clang);
- при создании тега вида `v*` отдельный job собирает пакеты `cpack -G DEB/RPM/TGZ/ZIP`;
- собранные пакеты загружаются в GitHub Release через `softprops/action-gh-release`.

Ключевой кусок:

```yaml
  release:
    needs: build
    if: startsWith(github.ref, 'refs/tags/')
    runs-on: ubuntu-22.04
    permissions:
      contents: write
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive
      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y cmake build-essential rpm
      - name: Configure
        run: cmake -H. -B_build
      - name: Build
        run: cmake --build _build
      - name: Create packages
        run: |
          cd _build
          cpack -G DEB
          cpack -G RPM
          cpack -G TGZ
          cpack -G ZIP
      - name: Upload packages to Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            _build/*.deb
            _build/*.rpm
            _build/*.tar.gz
            _build/*.zip
```

После пуша тега `v0.1.0.0` workflow собрал пакеты и автоматически
создал релиз с прикреплёнными файлами `.deb`, `.rpm`, `.tar.gz`, `.zip`.

---

## 4. Результаты

- Настроена сборка пакетов DEB/RPM/TGZ/ZIP через CPack.
- Тесты проходят (11 тестов, gcc и clang).
- При создании тега пакеты автоматически собираются и публикуются в GitHub Release.
