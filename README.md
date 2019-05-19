# DiningSavages
Dining savages problem written  in C

```
 gcc savages.c -o savages -pthread
```

```
./savages 5 10 0 1
```

Exemplo acima:
5 selvagens, que precisam de 10 refeições, o pote inicia com 0, cozinheiro cozinha 1 refeição por vez

```
./savages 10000 10 0 4
```

Exemplo acima:
10000 selvagens, que precisam de 10 refeições, o pote inicia com 0, cozinheiro cozinha 4 refeição por vez

(vai demorar um pouco com um sleep(1), você pode comentar a linha na função eat para execução instantânea)

(Certifique-se de que seu linux suporta 10001 threads...)


```
./savages 10000 10 0 4
```

Exemplo acima:
10000 selvagens, que precisam de 10 refeições, o pote inicia com 0, cozinheiro cozinha 4 refeição por vez
(vai demorar um pouco com um sleep(1), você pode comentar a linha na função eat para execução instantânea)

```
./savages 100 1 0 2
```

Exemplo acima:
100 selvagens, que precisam de 1 refeições, o pote inicia com 0, cozinheiro cozinha 2 refeição por vez
