# GAME DESIGN DOCUMENT (GDD)

---

## 0. Metadatos
- **Título del juego:** Project Prime-8
- **Versión del documento:** v1.0.0
- **Estado del juego:** Pre-Alpha
- **Equipo:** Daniel García-Diego González, Máximo Laino Chiaro, Gabriel Manso Gutiérrez de Rozas
- **Alcance inicial:** PC (Windows/Linux)

---

## 1. Visión y objetivos

### 1.1 Gran concepto
“**Salta sin parar** escalando plataformas, **carga tu salto** y usa **objetos** que te ayudan a **huir del gas** que asciende.”

### 1.2 Elevator pitch (actualizado)
Arcade vertical de precisión en el que **tú decides cada salto**: te mueves lateralmente y **cuanto más tiempo mantengas el salto, más alto/largo será**. El reto está en **leer el patrón de plataformas**, **gestionar el timing** de la carga y **encadenar saltos óptimos** mientras el gas sube. **Power-ups** temporales amplían tus capacidades (salto más grande, mejor control aéreo, escudo, etc.).

### 1.3 Pilares (resumidos)
1) **Claridad en 5 s** (se entiende sin leer).  
2) **Control preciso** (mismo input = mismo salto).  
3) **Legibilidad ante todo** (tipos de plataforma claros).  
4) **Reintento instantáneo** (morir → jugar ya).  
5) **Profundidad emergente** (decisiones nuevas con lo ya aprendido).  
6) **Feedback jugoso** (visual + sonoro, llamativo).  
7) **Ritmo adaptativo** (tensión sin trampas).  
8) **Accesibilidad sin ventaja** (opciones que no rompen la dificultad).  
9) **Rendimiento constante** (60 FPS).

### 1.4 Objetivos de experiencia
- Tensión constante por el gas: “**una más y me voy**”.
- Sensación de **mejora personal** (dominar carga y colocación).
- **Estética y audio** pegadizos que refuerzan el flow.

---

## 2. Introducción

### 2.1 Resumen
Arcade vertical para PC donde **cargas cada salto**: mantener **Espacio** aumenta la fuerza **hasta un límite**. Encadena saltos precisos, lee patrones de plataformas y sobrevive al **gas**. **Power-ups** temporales (salto mayor, control aéreo, escudo).

---

## 3. Resumen del juego (Game Overview)

### 3.1 Concepto
**Objetivo:** alcanzar la **máxima altura**.  
**Qué hace el jugador:** moverse **izq/der** y **cargar/saltar** con precisión.  
**Qué debe sentir:** tensión creciente, satisfacción al clavar el **timing** del salto y fluidez por reintentos instantáneos.

### 3.2 Audiencia
- **Edad:** 10–45; sesiones de 1–3 minutos.
- **Gustos:** arcade, plataformas de precisión, score-chasing.
- **Juegos similares:** *Doodle Jump*, *Stack Jump*, *N++*.  
- **Motivo de interés:** curva de habilidad clara (saltos cargados), runs cortos.

### 3.3 Género
**Plataformas Arcade vertical (Score-Chaser).**

### 3.4 Ambientación
**Torre volcánica** estilizada en 2D cartoon; colores fríos/neutros para plataformas.

### 3.5 Estructura del mundo
**Ascenso infinito** por **plataformas** procedurales. La cámara **solo sube**.

### 3.6 Jugador
Un personaje ágil. **Singleplayer**. Progresión basada en **habilidad**, no en stats.

### 3.7 Bucle central
Aparecer → **leer** plataformas → **moverse** (A/D) → **mantener** Espacio para **cargar** → **soltar** para saltar → **recoger** → **evitar** peligros → repetir → **morir** → **reintentar** al instante.

### 3.8 Look & Feel
- **Look:** 2D claro, llamativo y legible.  
- **Feel:** control snappy; arcos de salto predecibles; música pegadiza.

---

## 4. Público y referencias
- **Target:** 10–45 años; sesiones cortas.
- **Motivaciones:** superar récords personales, retos de precisión.
- **Referencias:** *Doodle Jump*, *Stack Jump*, *Ninja UP!*.

---

## 5. Plataformas y negocio
- **Plataformas:** PC.
- **Modelo:** Precio simbólico o demo gratuita (sin P2W).
- **Publicidad:** No prevista en PC.
- **Clasificación estimada:** PEGI 3 / ESRB E.

---

## 6. Controles y accesibilidad

### 6.1 PC
- **Mover izquierda/derecha:** `A / D` o `← / →`
- **Salto con carga:** **Mantener** `ESPACIO` para **cargar** y **soltar** para saltar (cuanto más sostienes, **más salto**, con **límite**).
- **Pausa:** `ESC`.

### 6.2 Accesibilidad (opciones)
- **Curva de carga:** Lineal (base).
- **Micro-salto:** taps < **120 ms** → impulso corto y controlado.  
- **Tamaños UI altos** y alto contraste.  

---

## 7. Jugabilidad

### 7.1 Objetivos
- **Principal:** alcanzar la **mayor altura** posible (score).  
- **Secundarios:** recoger monedas, lograr **timings perfectos**, completar reto diario con semilla fija.

### 7.2 Progresión
A mayor altura: **gaps** más largos, más plataformas **frágiles/móviles**, aparición de **gas** más rápida.

### 7.3 Flujo de juego
Menú → run rápida → **Game Over** → **Resumen** → **Reintentar**.

### 7.4 Dificultad
Una base afinada para skill.

---

## 8. Mecánicas

### 8.1 Reglas generales
- Tocar gas = **muerte** (el **escudo** lo niega 1 vez).  
- 1 **power-up** activo; **Muelle** se guarda para el **siguiente** salto.  
- La cámara **no baja**.

### 8.3 Física
- Gravedad constante (arcade).  
- **Salto con carga** con **límite** y curva **lineal** (base).  

### 8.6 Interacción del jugador
- **Plataformas:**  
  - Estándar 
  - Frágil 
  - Móvil
  - Trampolín
  - Resbaladiza 
- **Power-ups (duración):**  
  - **Cohete (2.0 s):** ascenso continuo; **Espacio** modula empuje.  
  - **Muelle (1 uso):** gran salto hacia arriba.  
  - **Escudo (3 s):** niega 1 error.   

#### 8.6.1 Menús del juego
Menú principal (Título, Jugar, Salir) → In-Game → Pausa → Game Over → Resumen (puntuación) → Reintentar/Menú.

#### 8.6.3 Opciones
Volúmenes, remapeo de teclas, cambio de idioma.

### 8.7 Assets principales
Sprites (jugador, plataformas, power-ups, gas), VFX (poof, grietas, llama cohete, glow), Audio (música ), tipografías UI.

---

## 9. Gráficos y audio

### 9.1 Sistema visual
2D cartoon minimal; **atlas** de sprites

#### 9.1.1 Cámara de jugador
`Camera2D` centrada en el jugador; desplazamiento **vertical ascendente**.

#### 9.1.2 Paisaje
Eje vertical claro **laboratorio → exterior torre → atmósfera → órbita**.  
- **Laboratorio:** pozos de pruebas, conductos de mantenimiento, plataformas metálicas y ascensores de jaula.  
- **Atmósfera:** globos sonda, drones de vigilancia, placas térmicas flotantes.  
- **Órbita:** antenas, módulos de satélite, placas de blindaje y escombros espaciales.  
**Métricas constantes:** plataformas estándar ; frágiles ; **gaps**.

### 9.2 Interfaz
**HUD:** altura (m), mejor marca, indicador de “peligro” cuando el gas se acerca, botón de pausa.  
**Menús:** navegación por teclado/mando, foco claro; tarjetas para Jugar, Reto diario, Personalización, Clasificaciones, Opciones y Créditos; resumen de récords en portada.

---

## 10. Historia y narrativa

### 10.1 Antecedentes
Una **empresa científica** desarrolla un **compuesto experimental** que otorga habilidades especiales. Se realizan pruebas con animales; al protagonista (un **mono**) le entrenan el **supersalto**. Cada día, desde su celda, mira un **póster del sistema solar** y sueña con verlo de cerca.

### 10.2 Trama principal
Durante un ejercicio, el mono **domina** el supersalto lo suficiente para **escapar**. Debe trepar por pozos y plataformas del laboratorio, sortear sistemas de seguridad, seguir subiendo por la **atmósfera** y llegar a la **órbita** para cumplir su sueño.

#### 10.2.1 Progresión de trama
La narrativa avanza con **cambios de bioma** y señales ambientales..

---

## 11. Personajes

### 11.1 Protagonista
**El Mono (sujeto de pruebas)**  
- **Trasfondo:** identificado por un código de laboratorio; su motivación nace del póster del sistema solar.  
- **Personalidad:** curioso, decidido; anima con gestos cuando mejora su marca.  
- **Apariencia:** silueta cartoon con arnés de sensores; **skins** cosméticas (bandana, traje térmico, “astro-mono”).  
- **Habilidades:** **salto con carga** (press & hold con límite), control aéreo, uso de power-ups.  
- **Objetivo:** escapar y **alcanzar el espacio**.

### 11.2 Secundarios
- **Sistema de Seguridad** (voz/altavoces): emite avisos (“intruso detectado”), actúa como “narrador” diegético.  

### 11.3 Posibles Enemigos
- **Drones de vigilancia:** patrulla lateral; empujan si colisionan.  
- **Torretas-stun:** disparo telegráfico (el escudo lo anula).  
- **Plataformas frágiles** y **picos industriales**: peligros ambientales.  
- **Ventiladores/ráfagas** (atmósfera): empuje lateral temporal.

---

## 12. Mundo del juego

### 12.1 Look & Feel
Paleta y materiales evolucionan **laboratorio → celeste → espacial**.

### 12.2 Localizaciones
- **Laboratorio de pruebas:** celdas, cámaras de salto, pasarelas, tubos de contención.  
- **Atmósfera alta:** nubes, globos sonda, drones aerostáticos.  
- **Órbita baja:** satélites, paneles y módulos de servicio.

#### 12.2.1 Conexión con la trama
Cada zona representa un **paso de la fuga**: del encierro y la experimentación a la **libertad** del cielo y la **realización del sueño** en órbita.

### 12.3 Niveles
**Procedurales**