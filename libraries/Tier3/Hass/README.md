Hass
====

Native Home Assistant controls and data models used by the Clock sample.

The module provides `MushroomButton` and `MushroomLightCard`, native controls
inspired by the Mushroom cards used by the Clock Home Assistant dashboards.
The C++ backend provides:

- `HassSocket`: Home Assistant WebSocket authentication and `state_changed`
  subscription over `Tier2/Rest`.
- `HassEntityModel`: generic state model, synchronised from REST data and the
  WebSocket event stream.
- `HassEntityMapper`: Home Assistant state mapper.

New code should use `HassSocket` with `HassEntityModel` or `HassEntityMapper`.

QML components are grouped by responsibility:

- `qml/buttons`: compact entity controls and the shared `HassDelegate` base.
- `qml/cards`: composite controls such as `HassLightCard`.
- `qml/displays`: read-only entity displays such as sensors and weather.
- `qml/delegates`: model delegates and domain-based delegate choosers.

The entity list is intentionally backed by one `HassEntityModel`, which keeps
REST state and WebSocket updates in one place. Room pages should create
filtered proxy views over that model. A `HassEntityMapper` is reserved for a
single entity form or editor that needs direct REST selection/submission; it
should not be instantiated once per entity in a dashboard.

Import it in QML with:

```qml
import Eco.Tier3.Hass
```
