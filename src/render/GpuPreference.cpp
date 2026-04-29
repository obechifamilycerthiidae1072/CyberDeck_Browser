#include <windows.h>

// These well-known exports are consumed by NVIDIA Optimus and AMD PowerXpress
// drivers as a preference hint for hybrid laptops. They do not force a vendor
// path; systems without matching drivers ignore them and continue normally.
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
