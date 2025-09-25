This is an external component for Electrolux EACM-DR14, controlled via IR remote.

```yaml
remote_transmitter:
  pin: GPIO32
  carrier_duty_percent: 50%

external_components:
  - source: github://flicker581/esphome_electrolux_eacm
    refresh: 0s 

climate:
  platform: electrolux_eacm
  supports_heat: false
  name: "Electrolux air conditioner"
```
