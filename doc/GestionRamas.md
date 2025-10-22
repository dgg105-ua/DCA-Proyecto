## Estrategia de gestión de ramas: **GitHub Flow**

### 1.1 Resumen de la estrategia
GitHub Flow es un flujo **ligero** basado en una rama principal estable (**`main`**) y ramas cortas por cambio (**feature/hotfix**). Todo cambio se integra vía **Pull Request** (PR).

**Pasos básicos:**
1. Crear rama desde `main`: `feature/` o `fix/`...
2. Commits pequeños y descriptivos (incluyendo la **clave de Jira**).
3. Abrir PR a `main` (revisión + CI verde).

### 1.2 Por qué se ajusta a este proyecto (justificación)
- **Equipo pequeño (3 personas)** y **alcance PC**: necesitamos **fricción mínima** y ciclos rápidos.
- **Pre-Alpha** con entregas frecuentes: GitHub Flow favorece **iteración corta** y **PRs** claros.
- **Hotfixes** sencillos: rama `fix/…` desde `main` y PR rápida.

### 1.3 Convenciones del equipo
- **Nombres de ramas**  
  - `feature/GAS-123-salto-cargado-limite`  
  - `fix/GAS-201-corregir-colision-picos`  
  - Otros prefijos: `release/`, `docs/`, `hotfix/`
- **Commits** 
  - Cortos 
- **Pull Requests**  
  - Título: `[GAS-123] Implementar límite de salto cargado`  
  - Cuerpo: _Descripción, capturas/gif, pasos de prueba, impacto, fallback_
  - **Checks obligatorios**: CI válido.
- **Protecciones de `main`**  
  - Prohibido push directo.  
  - Revisión obligatoria.  

### 1.4 Versionado y releases
- **VersionadoSemántico** aproximado en pre-alpha.  
- **Tags** en GitHub tras merges significativos: `v0.3.0`, `v1.0.0`.  
- **Notas de versión**: autogeneradas a partir de PRs (título + etiqueta `feature`, `fix`).

### 1.5 Riesgos y mitigación
- **Ramas que se alargan** → dividir en PRs pequeños, *feature flags* (menú oculto dev) para integrar temprano.  