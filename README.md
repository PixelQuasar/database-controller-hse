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
Для запуска тестов на отдельную компоненту можно использовать команду 
`ctest -R <component_name>`
Пример:
`ctest -R Calculator` - запуск тестов на компоненту Calculator