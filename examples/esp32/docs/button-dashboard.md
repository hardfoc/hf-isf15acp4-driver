# ISF15ACP4 Button Dashboard

**App:** `isf15acp4_button_dashboard`  
**Source:** `main/isf15acp4_button_dashboard.cpp`

## Purpose

Motor-control style dashboard demonstrating production UI patterns:

- Header bar and RPM readout
- Circular gauge via `ButtonUI::DrawGauge`
- Debounced click / long-press (start/stop simulated motor)
- RUN/STOP status with button highlight

## Build

```bash
./scripts/build_app.sh isf15acp4_button_dashboard Debug
./scripts/flash_app.sh flash_monitor isf15acp4_button_dashboard Debug
```

## Related Docs

- [../../../docs/graphics.md](../../../docs/graphics.md)
- [../../../docs/animations.md](../../../docs/animations.md)
