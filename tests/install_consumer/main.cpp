// Builds against the installed headers only: every public header and the .ipp files they include.
#include "isf15acp4.hpp"
#include "isf15acp4_animator.hpp"
#include "isf15acp4_version.h"

#include <cstdio>

int main() {
  isf15acp4::GraphicsContext gfx(isf15acp4::ProductVariant::Isf15acp4, isf15acp4::GraphicsBackend::BuiltinCanvas);
  gfx.DrawString(0, 0, "OK", isf15acp4::colors::White);  // uses the font data from the installed library
  std::printf("hf_isf15acp4 %s\n", isf15acp4::GetDriverVersion());
  return 0;
}
