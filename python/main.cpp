// Binding code for pybind11

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/DAngeloModel.h"
#include "../src/HuovilainenModel.h"
#include "../src/ImprovedModel.h"
#include "../src/ImprovedAltModel.h"
#include "../src/KrajeskiModel.h"
#include "../src/MicrotrackerModel.h"
#include "../src/MusicDSPModel.h"
#include "../src/OberheimVariationModel.h"
#include "../src/RKSimulationModel.h"
#include "../src/SimplifiedModel.h"
#include "../src/StilsonModel.h"
#include "../src/ValimakiModel.h"

namespace py = pybind11;

// clang-format off
#define CLASS(cls) \
py::class_<cls>(m, #cls)\
.def(py::init<float>())\
.def("process", [](cls& filter, std::vector<float>& samples) {\
filter.Process(samples.data(), static_cast<uint32_t>(samples.size()));\
return samples;})\
.def_property("resonance", &cls::GetResonance, &cls::SetResonance)\
.def_property("cutoff", &cls::GetCutoff, &cls::SetCutoff)\
.def_property("drive", &cls::GetDrive, &cls::SetDrive)\
// clang-format on

PYBIND11_MODULE(_MoogLadder, m)
{
    CLASS(DAngeloMoog);
    CLASS(HuovilainenMoog);
    CLASS(ImprovedMoog);
    CLASS(ImprovedAltMoog);
    CLASS(KrajeskiMoog);
    CLASS(MicrotrackerMoog);
    CLASS(MusicDSPMoog);
    CLASS(OberheimVariationMoog);
    CLASS(RKSimulationMoog);
    CLASS(SimplifiedMoog);
    CLASS(StilsonMoog);
    CLASS(ValimakiMoog);
}
