# Лабораторная работа #2

## Задачи:

1. Написать библиотеку на языке C или C++, которая:
   - Содержит функцию для запуска программ в фоновом режиме.
   - Позволяет ожидать завершения работы дочерней программы.
   - Возвращает код завершения дочерней программы.
   
2. Разработать тестовую утилиту для проверки работы библиотеки, которая:
   - Запускает различные команды (например, `sleep`, `echo`).
   - Ожидает их завершения и выводит код завершения.
   
3. Убедиться, что и библиотека, и утилита:
   - Кроссплатформенные.
   - Собираться и работать как на ОС семейства Windows, так и на UNIX (POSIX).

---

## Структура проекта:

- **`BackgroundProcess.h`** — заголовочный файл библиотеки.
- **`BackgroundProcess.cpp`** — реализация библиотеки.
- **`TestUtility.cpp`** — тестовая утилита для проверки работы библиотеки.
- **`build.sh`** — скрипт для сборки проекта на Linux.
- **`build.cmd`** — скрипт для сборки проекта на Windows.

---

## Инструкция:

### Для Windows:

1. Запустить сборку проекта:
   ```cmd
   build.cmd
   ```
2. Выполнить программу:
   ```cmd
   build\TestUtility.exe
   ```

### Для Linux:

1. Запустить сборку проекта:
   ```bash
   build.sh
   ```
2. Выполнить программу:
   ```bash
   build\TestUtility
   ```

---

## Пример работы утилиты:

### Пример 1: Запуск команды `echo`

```bash
Enter the program to execute: echo
Enter the arguments (or leave empty): Hello World
Process started. Waiting for it to finish...
Process finished with exit code: 0
```

#### В данном примере команда echo успешно выполняется и выводит "Hello World". Код завершения равен 0.

### Пример 2: Запуск команды `sleep` с таймаутом

```bash
Enter the program to execute: sleep
Enter the arguments (or leave empty): 5
Process started. Waiting for it to finish...
Process finished with exit code: 0
```

#### В этом примере команда sleep 5 выполняется и завершает свою работу через 5 секунд(Таймаут стоит 10 секунд).

### Пример 3: Запуск команды `sleep` с таймаутом

```bash
Enter the program to execute: sleep
Enter the arguments (or leave empty): 15
Timeout exceeded while waiting for process.
Process did not finish in time or failed.
```

#### В этом примере команда sleep 15 выполняется и выдает ошибку, так как не успело выполнится за 10 секунд.

### Пример 4: Ошибка выполнения несуществующей команды

```bash
Enter the program to execute: dfkdfk
Enter the arguments (or leave empty):  
Process started. Waiting for it to finish...
Unknown command: dfkdfk
Process finished with exit code: 127
```

#### В этом примере попытка запустить несуществующую команду приводит к ошибке, и выводится сообщение о том, что процесс не был запущен.
