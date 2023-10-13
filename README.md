# Лабораторная работа №2

### Запуск
1) Сбилдить проект:
```
$ make generate
```
*При повторном запуске использовать*
```
$ make build-project
```
2) Запустить программу в обычном режиме (2 способа)
   1) ```
      $ ./build/simplifier
      ```
   2) ```
      $ make run
      ```
**ВАЖНО** При запуске программы поддерживаются следующие флаги:

1) `-f` `--file` для указания пути к файлу с входными регулярными выражениями
2) `-r` `--regex` для указания регулярного выражения непосредственно при запуске программы

* Если запускать через `make run` то необходимо прописать эти флаги в `Makefile`
* Для вывода в файл необходимо написать так: 
```
$ make run >> file.txt 
```

3) Запустить программу в режиме *консоли**
```
$ make console
```
**интерактивный режим, в котором можно отслеживать структуру дерева разбора
и представления автомата в виде графа в режиме онлайн (в текущей директории создается директория tmp с визулизациями)*

4) Для запуска Fuzz-тестирования
```
$ make run-fuzz
```

*P.s Из-за особенности встроенной в C++ библиотеки для работы с регуляркам
некоторые сгенерированные строки обрабатываются неприлично долго, поэтому если тесты не отработали в течение минуты
то можно смело перезапускать, потому что `std::regex_match` завис :)*
