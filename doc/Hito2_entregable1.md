# Hito 2
## Entregable 1

### Tiempos de compilación con diferentes valores de N (-jN)

| N (trabajos en paralelo) | Tiempo (s) |
|---------------------------|-------------:|
| 1 | 2.781 |
| 2 | 1.613 |
| 3 | 1.316 |
| 4 | 1.188 |
| 5 | 1.185 |
| 6 | 1.177 |
| 7 | 1.088 |
| 8 | 1.110 |
| 9 | 1.167 |
| 10 | 1.356 |

**Conclusión:** A partir de `N = 4–6`, la mejora en tiempo de compilación deja de ser sustancial.  
Incluso, a partir de `N > 8`, el tiempo aumenta ligeramente debido a la sobrecarga de procesos paralelos.

No se producen fallos de comilacion con ejecuciones en paralelo.

Aplicando **ccache**, la primera ejecucion del compilador tarda más tiempo que con los métodos anteriores (3.121s). Sin embargo, al volver a ejecutarlo y por ello hacer uso de la herramienta ccache, la ejecucion es muchísimo más rápida (0.245s).

