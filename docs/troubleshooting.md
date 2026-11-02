# Troubleshooting

## Blank display after init

- Confirm **VCC_EN** is asserted and charge pump delay elapsed (`enable_vcc_on_init = true`).
- Verify **D/C** toggles: commands with D/C low, pixel data with D/C high.
- Check SPI mode 0 wiring (MOSI/SCK only; MISO not used).
- Measure module supply rails per NKK datasheet.

## Garbled or shifted image

- Wrong `ProductVariant` init table for your module.
- SPI clock too fast for wiring length — reduce `clock_speed_hz`.
- CS released too early — ensure `EndTransaction()` raises CS after each burst.

## Colors wrong

- Framebuffer is **RGB565**; byte order must match SSD1331 expectations in `UploadFramebuffer`.
- When importing PNGs, use `scripts/convert_image_to_rgb565.py`.

## Button always pressed / never pressed

- Configure `switch_active_low` to match hardware (ISF15ACP4 is typically active low).
- Enable internal pull-up on MCU GPIO if the module does not provide one.
- Pass `switch_in` GPIO to handler/adapter; omit only if you do not read the switch.

## hf-core handler will not link

- Set `HF_CORE_ENABLE_ISF15ACP4 ON` before building handler tests (platform default is OFF).
- Ensure submodule `hf-isf15acp4-driver` is initialized.
- SPI device uses a **dummy CS** pin; real CS is controlled via `BaseGpio` (software SS).

## CI build failures

- Run `python generate_matrix.py --validate` in `examples/esp32/scripts`.
- Submodule `hf-espidf-project-tools` must be present (`git submodule update --init --recursive`).
