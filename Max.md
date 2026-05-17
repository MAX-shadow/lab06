# Лабораторная работа 06

Студент: MAX-shadow
Цель работы: создание покетов для изменнени(должны сочетаться с тегами)
Для этого нужно добавить ветлвение в конфигурационные файлы для СL.

## 1. Настройка переменных
```
export GITHUB_USERNAME=MAX-shadow
export GITHUB_EMAIL=MAX-shadow@github.com
alias edit=nano
alias gsed=sed
```

Результат: настроили окружение для работы в терминале 

## 2. Добавляем версионирование в CMakeLists.txt
```
gsed -i '/project(lab06)/a\ set(PRINT_VERSION_MAJOR 0)' CMakeLists.txt
gsed -i '/project(lab06)/a\ set(PRINT_VERSION_MINOR 1)' CMakeLists.txt
gsed -i '/project(lab06)/a\ set(PRINT_VERSION_PATCH 0)' CMakeLists.txt
gsed -i '/project(lab06)/a\ set(PRINT_VERSION_TWEAK 0)' CMakeLists.txt
gsed -i '/project(lab06)/a\ set(PRINT_VERSION "${PRINT_VERSION_MAJOR}.${PRINT_VERSION_MINOR}.${PRINT_VERSION_PATCH}.${PRINT_VERSION_TWEAK}")' CMakeLists.txt
gsed -i '/project(lab06)/a\ set(PRINT_VERSION_STRING "v\\${PRINT_VERSION}")' CMakeLists.txt
```
## 3. Создаем DESCRIPTION и ChangeLog.md
```
touch DESCRIPTION && edit DESCRIPTION
touch ChangeLog.md
export DATE="LANG=en_US date +'%a %b %d %Y'"
cat > ChangeLog.md <<EOF
* ${DATE} ${GITHUB_USERNAME} <${GITHUB_EMAIL}> 0.1.0.0
- Initial RPM release
EOF

```
## 4. Создаем CPackConfig.cmake
```
cat > CPackConfig.cmake <<EOF
include(InstallRequiredSystemLibraries)
EOF

cat >> CPackConfig.cmake <<EOF
set(CPACK_PACKAGE_CONTACT ${GITHUB_EMAIL})
set(CPACK_PACKAGE_VERSION_MAJOR \${PRINT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR \${PRINT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH \${PRINT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK \${PRINT_VERSION_TWEAK})
set(CPACK_PACKAGE_VERSION \${PRINT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_FILE \${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Static C++ library for printing")
EOF

cat >> CPackConfig.cmake <<EOF
set(CPACK_RESOURCE_FILE_LICENSE \${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README \${CMAKE_CURRENT_SOURCE_DIR}/README.md)
EOF

cat >> CPackConfig.cmake <<EOF
set(CPACK_RPM_PACKAGE_NAME "print-devel")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "print")
set(CPACK_RPM_CHANGELOG_FILE \${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.md)
set(CPACK_RPM_PACKAGE_RELEASE 1)
EOF

cat >> CPackConfig.cmake <<EOF
set(CPACK_DEBIAN_PACKAGE_NAME "libprint-dev")
set(CPACK_DEBIAN_PACKAGE_PREDEPENDS "cmake >= 3.0")
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
EOF

cat >> CPackConfig.cmake <<EOF
include(CPack)
EOF
```
## 4. Подключаем CPack в CMakeLists.txt
```
cat >> CMakeLists.txt <<EOF

include(CPackConfig.cmake)
EOF
```

## 5. Коммитим и пушим
```
git add .
git commit -m "added cpack config"
git tag v0.1.0.0
git push origin master --tags
```
## 6. Настраиваем GitHub Actions (вместо Travis CI)
```
mkdir -p .github/workflows
cat > .github/workflows/linux.yml << 'EOF'
name: Linux CI (gcc & clang)

on:
  push:
    branches: [ main, master ]
    tags:
      - 'v*'
  pull_request:
    branches: [ main, master ]

jobs:
  build:
    runs-on: ubuntu-22.04
    strategy:
      matrix:
        compiler: [gcc, clang]
    env:
      CC: ${{ matrix.compiler }}
      CXX: ${{ matrix.compiler == 'gcc' && 'g++' || 'clang++' }}
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0
      
      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y cmake build-essential rpm
      
      - name: Configure
        run: cmake -H. -B_build -DBUILD_TESTS=ON
      
      - name: Build
        run: cmake --build _build
      
      - name: Run tests
        run: ctest --test-dir _build --output-on-failure
      
      - name: Create packages (if tag)
        if: startsWith(github.ref, 'refs/tags/')
        run: |
          cd _build
          cpack -G DEB
          cpack -G RPM
          cpack -G TGZ
          cpack -G ZIP
      
      - name: Upload packages to Release
        if: startsWith(github.ref, 'refs/tags/')
        uses: softprops/action-gh-release@v1
        with:
          files: |
            _build/*.deb
            _build/*.rpm
            _build/*.tar.gz
            _build/*.zip
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
EOF
```
При каждом push запускаются тесты
При создании тэга v* CPack создаёт DEB, RPM, TGZ, ZIP пакеты
Пакеты автоматически загружаются в Release на GitHub
## 7. Коммитим GitHub Actions
```
git add .github/workflows/linux.yml
git commit -m "Add GitHub Actions with CPack packaging on tags"
git push origin master
```
## 8. Создаем пакеты локально для проверки
```
cmake -H. -B_build
cmake --build _build
cd _build
cpack -G "TGZ"
cpack -G "DEB"
cpack -G "RPM"
cd ..
```
Конфигурирует проект через CMake
Собирает проект
Создаёт пакеты локально (проверка без CI
## 9. Создаем artifacts (скриншот)
```
mkdir artifacts
sleep 20s && gnome-screenshot --file artifacts/screenshot.png
```
