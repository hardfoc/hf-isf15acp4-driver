# Changelog

All notable changes to this driver are listed here. Versions follow
[Semantic Versioning](https://semver.org/). Entries marked **BREAKING** need source changes in
code that used the affected API; together they call for a major version bump at release.

## Unreleased

### Documentation

- The documentation site moved to the HardFOC MkDocs platform: new page set (quickstart, hardware, architecture,
  graphics, animations, API guide, configuration, porting, examples, troubleshooting), the Doxygen API reference
  under `/api/`, and every code sample compiled by the host build from `tests/docs/`.
- Corrected the hardware facts against the NKK datasheet: the ISC15ANP4 is 64 x 48 and the ISC01P 52 x 36 (not
  96 x 64 and 128 x 128), VCC is 14 V to 16 V (15 V typical), the power sequence raises VCC after the reset pulse,
  and the panel's pixels are 0.222 mm x 0.293 mm.
- Removed the illustrations whose pinout and wiring did not match the part.

### Added

- Host unit tests (`HF_ISF15ACP4_BUILD_TESTS`) that run the driver against a recording SPI port.
