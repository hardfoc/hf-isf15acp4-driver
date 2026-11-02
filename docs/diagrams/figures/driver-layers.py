"""The three layers of the driver: hardware-agnostic core, communication interface, platform port.

Reads: From top to bottom. Your application draws into a GraphicsContext and calls one SmartDisplay<SpiType>
object, whose public calls (listed from inc/isf15acp4.hpp) initialise the SSD1331 controller, upload frames, run
its hardware drawing commands and read the switch. Each call becomes command and data transactions. The
communication interface (SpiInterface in inc/isf15acp4_spi_interface.hpp) is a CRTP base that forwards them to
your class. The platform port is the only code you write: a class that writes SPI bytes with D/C and SS, sets the
control pins, reads the switch and waits. Below it is the hardware.
"""

import re
from pathlib import Path

from hardfoc_mkdocs_theme.figkit import Fig

ROOT = Path(__file__).resolve().parents[3]
DRIVER = "inc/isf15acp4.hpp"
COMM = "inc/isf15acp4_spi_interface.hpp"
SKIP = {"IsInitialized", "HasError", "Reset"}


def data():
    drv = (ROOT / DRIVER).read_text()
    public = drv.split("public:", 1)[1].split("private:", 1)[0]
    calls = []
    for name in re.findall(r"^    (?:\[\[nodiscard\]\] )?bool (\w+)\(", public, re.M):
        if name not in SKIP and name not in calls:
            calls.append(name)
    comm = (ROOT / COMM).read_text()
    iface = comm.split("class SpiInterface", 1)[1]
    hooks = []
    for name in re.findall(r"^    (?:bool|void) (\w+)\([^)]*\) noexcept \{\s*(?:return )?static_cast", iface, re.M):
        if name not in hooks:
            hooks.append(name)
    return {
        "calls": calls,
        "port_hooks": hooks,
        "sources": [f"{DRIVER}:SmartDisplay public calls", f"{COMM}:SpiInterface"],
    }


def build(d):
    f = Fig(
        "driver-layers",
        "Three layers, one of them yours",
        alt="Layer diagram: the application draws into a graphics context and calls the hardware-agnostic "
        "SmartDisplay driver, which uses the CRTP SPI interface, which calls the platform port you write, which "
        "drives the SPI bus, the D/C, SS, RES and VCC enable lines and reads the switch of the ISF15ACP4.",
        width=1200,
        kicker="Architecture",
        subtitle="The driver and the interface are the same on every MCU. The port is a small class with eight methods.",
        profile="overview",
        legend=[("brand", "this library"), ("teal", "you write"), ("slate", "hardware")],
    )
    W = f.body_w
    app = f.card(0, 0, W, "Your application",
                 ["GraphicsContext, Canvas, Animator, ButtonUI: frames, text, effects, the switch"], tone="slate")
    top = app.b + 40
    chips_h = f.chips(16, f.region_inner_top(top), W - 32, [c + "()" for c in d["calls"]], tone="brand")
    reg = f.region(0, top, W, f.region_inner_top(top) - top + chips_h + 18, "Hardware-agnostic core", tone="brand",
                   sub="SmartDisplay<SpiType>, inc/isf15acp4.hpp")
    comm = f.card(0, reg.b + 40, W, "Communication interface",
                  ["SpiInterface<Derived>: CRTP base, command and data transactions resolved at compile time,",
                   "control pins by name (SS, D/C, RES, VCC enable, switch) as ACTIVE / INACTIVE"],
                  tone="brand", accent=True)
    hooks = d["port_hooks"]
    half = (len(hooks) + 1) // 2
    port = f.card(0, comm.b + 40, W, "Platform port (your class)",
                  ["  ".join(h + "()" for h in hooks[:half]), "  ".join(h + "()" for h in hooks[half:])],
                  tone="teal", accent=True, mono=True)
    hw = f.card(0, port.b + 40, W, "ISF15ACP4",
                ["SPI write-only up to 6.6 MHz (150 ns cycle)  /  SS, D/C, RES, VCC 15 V, switch pins 1-2"],
                tone="slate")
    for a, b in ((app, reg), (reg, comm), (comm, port), (port, hw)):
        f.connect(a, b, "bottom", "top", tone="rail")
    return f
