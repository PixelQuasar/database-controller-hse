# database-controller-hse

## Установка репозитория:
```bash
git clone https://github.com/PixelQuasar/database-controller-hse.git
cd database-controller-hse
git submodule update --init --recursive # Загрузить googletest
```

## Сборка:
```bash
mkdir build
cd build
cmake ..
make
```

## Запуск тестов:
```bash
ctest
```