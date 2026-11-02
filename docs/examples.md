# Examples

ESP32 applications live under `examples/esp32/`. Build orchestration uses `hf-espidf-project-tools` (`app_config.yml` matrix).

## Applications

| App | Category | Description |
|:----|:---------|:------------|
| `isf15acp4_comprehensive_test` | test | Exercises init, fill, graphics, HW draw, button |
| `isf15acp4_ui_demo` | demo | Press counter + animated progress bar |
| `isf15acp4_animation_demo` | demo | Color wheel, plasma, scroll, pulse — press to cycle |
| `isf15acp4_button_dashboard` | demo | Motor RPM gauge + debounced start/stop |

## Build (local)

```bash
cd examples/esp32/scripts
python generate_matrix.py --validate
idf.py -C ../ build
```

Or use `idf_flash_monitor.sh` from `examples/esp32/scripts/`.

## CI matrix

`app_config.yml` sets `ci_enabled: true` for all four apps. GitHub Actions workflow `esp32-examples-build-ci.yml` validates the matrix via `hf-espidf-ci-tools`.

## hf-core handler test

When integrated into hf-core, see `examples/esp32/main/handler_tests/isf15acp4_handler_comprehensive_test.cpp`. Enable `HF_CORE_ENABLE_ISF15ACP4` in the `hf_core` component before building that app.
