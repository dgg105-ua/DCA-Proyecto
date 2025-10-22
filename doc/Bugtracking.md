## Sistema de bugtracking: **Jira**

### Enlace al Formulario
https://dcaproyectogrupo.atlassian.net/jira/software/c/form/527e922d-2147-4d0a-91c6-c98137901fc7

### ¿Por qué Jira?
- **Simplicidad** Hemos elegido este sistema de bugtracking ya que es de los más comunes, con mucha documentacón y foros

### Configuración mínima
- **Proyecto**: `DCA-Bugtracking`
- **Tipos**: `Bug`.
- **Workflow**: `Backlog → To Do → En Progreso → En Revisión → Terminado`
- **Campos útiles**: Severidad, Prioridad, Estado, Persona Asignada, Versiones Afectadas, Estimación Original.

### Severidad vs Prioridad
- **Severidad**: Grado de severidad (muy baja, baja, media, alta o muy alta)
- **Prioridad**: Grado de prioridad (Lowest, Low, Medium, High o Highest)

### Plantilla de bug
![Lista de Bugs](docs/img/ListaBugs.png)  
En la lista puedes filtrar y ordenar por Estado, Severidad, Prioridad o Persona asignada para priorizar qué arreglar primero; también se ve de un vistazo qué bugs están bloqueando la entrega y cuáles ya se han resuelto.

![Detalles de Bugs](docs/img/DetallesBugs.png)  
En los detalles del bug se muestra toda la información necesaria para reproducirlo y arreglarlo (descripción, pasos..), además del historial de cambios y los vínculos a la rama/PR relacionados para seguir su progreso técnica y funcionalmente.

**Severidad**: indica el impacto técnico/jugable del fallo (de muy baja a muy alta); ayuda a entender cuánto rompe la experiencia.  
**Prioridad**: marca la urgencia con la que debe solucionarse (de Lowest a Highest); sirve para planificar qué va primero en el sprint.  
**Estado**: refleja en qué punto del flujo está el bug (Backlog, To Do, En Progreso, En Revisión, Terminado) para que todos sepan su situación actual.  
**Persona Asignada**: responsable del arreglo; facilita la trazabilidad y evita duplicidad de esfuerzos.  
**Versiones Afectadas**: versión o build donde se detecta el problema; permite comprobar si el bug sigue presente o ya está corregido en versiones nuevas.  
**Estimación Original**: tiempo previsto para resolverlo (horas/días); ayuda a planificar carga de trabajo y compromisos del equipo.

### Ciclo de vida de un bug
1. **Backlog**: el bug entra al sistema, se revisa rápidamente la información y se le asignan Severidad y Prioridad iniciales (triage).  
2. **To Do**: el bug queda preparado para abordarse (tiene descripción suficiente y se confirma que no depende de otros bloqueos).  
3. **En Progreso**: un desarrollador lo toma, crea la rama `fix/GAS-…`, reproduce el error y comienza a implementar la solución.  
4. **En Revisión**: se abre el PR y otro miembro valida cambios y pruebas; si los checks están son validos se aprueba el merge.  
5. **Terminado**: se mergea el PR, se verifica el arreglo en la build correspondiente y se marca el bug como resuelto.
