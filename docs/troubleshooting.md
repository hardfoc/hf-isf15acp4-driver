# Troubleshooting

Symptoms you may see while bringing up an ISF15ACP4, their causes and the fix for each, cheapest check first.

The display sends nothing back, so the driver cannot see a wiring or supply fault: `Initialize()` returns `true`
as long as your port's calls succeed. A logic analyser on SS, D/C, SCK, SDI and RES, and a meter on VCC, answer
most questions in minutes.

## Screen stays dark after `Initialize()` returns `true`

| Check | Fix |
| --- | --- |
| RES is low after `Initialize()` | Map `Reset` `Active` to low, `Inactive` to high ([Porting](porting.md#control-pins)) |
| VCC (pin 9) is not 14 V to 16 V | `VccEnable` mapping, your supply's enable level, `Initialize(true)` |
| D/C is high during the init table | `BeginCommand()` must set D/C low |
| SS rises between an opcode and its parameters | One SS-low transaction per Begin / End pair |
| SCK above 6.6 MHz, or not mode 0 | Lower the clock; 1 MHz for bring-up |
| Pins numbered from the part | Numbers are not printed on it: use the datasheet's footprint drawing |

## `Initialize()` returns `false`

`GetLastError()` says which part failed:

| Error | Cause |
| --- | --- |
| `NotInitialized` | Your `EnsureInitialized()` returned `false`: the SPI bus or GPIO set-up failed |
| `SpiWrite` | `BeginCommand()` or `WriteBytes()` returned `false` |
| `InvalidParam` | The driver was constructed with a null port pointer |
| `OutOfRange` | A window outside the visible area: see the next section |

## `Initialize()` fails with `OutOfRange`

`SetWindow()` takes display coordinates and rejects a rectangle that goes past the variant's visible area
(96 x 64, 64 x 48 or 52 x 36). Do not add the ISC15ANP4 or ISC01P column offset yourself: the driver does
([Hardware](hardware.md#product-variants)).

## Colours wrong after `Present()`

Shapes and text are in the right place but the colours are not. The driver sends each pixel high byte first on
any CPU, so check your own code:

- Remove any byte swap added around `Present()` for earlier releases: it now swaps the frame back.
- `WritePixels()` sends your bytes unchanged. For `Color565` buffers use `WriteColors()`
  ([API guide](api-guide.md#write-pixels-yourself)).

## Red and blue swapped

The panel stores pixels as blue, green, red, and the driver converts every `Color565` it sends. If red shows as
blue in one kind of drawing only, check:

- Raw `WritePixels()` bytes: they go out unchanged, so convert each colour with `ssd1331::ToPanelPixel()` or use
  `WriteColors()`.
- A remap value (`A0h`) changed from the NKK initialisation table: bit 2 selects the colour order. Keep the
  table's value.

## Image stretched or offset

| Symptom | Cause and fix |
| --- | --- |
| Circles look like tall ovals | Pixels are 1.32 times taller than wide: draw at 96 : 84, resize to 96 x 64 ([Graphics](graphics.md#images)) |
| A frame lands shifted or wrapped | A `SetWindow()` window was left in place: `SetFullWindow()` before raw writes |
| The last rows are missing | The frame was cut short: `WriteBytes()` must send all 12,288 bytes |

## Switch reads wrong

| Symptom | Cause and fix |
| --- | --- |
| Always pressed or never | Missing pull-up or wrong level: pull-up on pin 1, pin 2 to GND, pressed reads low |
| Several presses per push | Contact bounce: use `ButtonUI::Update()`, called more often than the debounce time |
| `LongPress` never fires | Wrong elapsed time: pass the milliseconds since the previous `Update()` |

## Screen dims or shows old content

| Symptom | Cause and fix |
| --- | --- |
| Uneven brightness after months | Busy pixels age faster: move static screens, lower the master current, turn off when idle |
| Old content after scrolling stops | The SSD1331 needs its RAM rewritten: `Present()` after `ConfigureScroll(..., false)` |
| Hardware shapes disappear | `Present()` overwrote them: draw everything in the canvas |

## ESP32 example does not build

- Initialise the build scripts: `git submodule update --init examples/esp32/scripts`.
- Load ESP-IDF v5.5 (`. $IDF_PATH/export.sh`) before `./scripts/build_app.sh`.
- `./scripts/build_app.sh list` shows the apps and build types `app_config.yml` allows.
