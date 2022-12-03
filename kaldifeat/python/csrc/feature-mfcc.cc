// kaldifeat/python/csrc/feature-mfcc.cc
//
// Copyright (c)  2021  Xiaomi Corporation (authors: Fangjun Kuang)

#include "kaldifeat/python/csrc/feature-mfcc.h"

#include <memory>
#include <string>

#include "kaldifeat/csrc/feature-mfcc.h"
#include "kaldifeat/python/csrc/utils.h"

namespace kaldifeat {

void PybindMfccOptions(py::module &m) {
  using PyClass = MfccOptions;
  py::class_<PyClass>(m, "MfccOptions")
      .def(py::init<>())
      .def(py::init([](const MelBanksOptions &mel_opts,
                       const FrameExtractionOptions &frame_opts =
                           FrameExtractionOptions(),
                       int32_t num_ceps = 13, bool use_energy = true,
                       float energy_floor = 0.0, bool raw_energy = true,
                       float cepstral_lifter = 22.0, bool htk_compat = false,
                       py::object device =
                           py::str("cpu")) -> std::unique_ptr<MfccOptions> {
             auto opts = std::make_unique<MfccOptions>();
             opts->frame_opts = frame_opts;
             opts->mel_opts = mel_opts;
             opts->num_ceps = num_ceps;
             opts->use_energy = use_energy;
             opts->energy_floor = energy_floor;
             opts->raw_energy = raw_energy;
             opts->cepstral_lifter = cepstral_lifter;
             opts->htk_compat = htk_compat;

             std::string s = static_cast<py::str>(device);
             opts->device = torch::Device(s);

             return opts;
           }),
           py::arg("mel_opts"),
           py::arg("frame_opts") = FrameExtractionOptions(),
           py::arg("num_ceps") = 13, py::arg("use_energy") = true,
           py::arg("energy_floor") = 0.0f, py::arg("raw_energy") = true,
           py::arg("cepstral_lifter") = 22.0, py::arg("htk_compat") = false,
           py::arg("device") = py::str("cpu"))
      .def_readwrite("frame_opts", &PyClass::frame_opts)
      .def_readwrite("mel_opts", &PyClass::mel_opts)
      .def_readwrite("num_ceps", &PyClass::num_ceps)
      .def_readwrite("use_energy", &PyClass::use_energy)
      .def_readwrite("energy_floor", &PyClass::energy_floor)
      .def_readwrite("raw_energy", &PyClass::raw_energy)
      .def_readwrite("cepstral_lifter", &PyClass::cepstral_lifter)
      .def_readwrite("htk_compat", &PyClass::htk_compat)
      .def_property(
          "device",
          [](const PyClass &self) -> py::object {
            py::object ans = py::module_::import("torch").attr("device");
            return ans(self.device.str());
          },
          [](PyClass &self, py::object obj) -> void {
            std::string s = static_cast<py::str>(obj);
            self.device = torch::Device(s);
          })
      .def("__str__",
           [](const PyClass &self) -> std::string { return self.ToString(); })
      .def("as_dict",
           [](const PyClass &self) -> py::dict { return AsDict(self); })
      .def_static(
          "from_dict",
          [](py::dict dict) -> PyClass { return MfccOptionsFromDict(dict); })
      .def(py::pickle(
          [](const PyClass &self) -> py::dict { return AsDict(self); },
          [](py::dict dict) -> PyClass { return MfccOptionsFromDict(dict); }));
}

static void PybindMfcc(py::module &m) {
  using PyClass = Mfcc;
  py::class_<PyClass>(m, "Mfcc")
      .def(py::init<const MfccOptions &>(), py::arg("opts"))
      .def("dim", &PyClass::Dim)
      .def_property_readonly("options", &PyClass::GetOptions)
      .def("compute_features", &PyClass::ComputeFeatures, py::arg("wave"),
           py::arg("vtln_warp"), py::call_guard<py::gil_scoped_release>())
      .def(py::pickle(
          [](const PyClass &self) -> py::dict {
            return AsDict(self.GetOptions());
          },
          [](py::dict dict) -> std::unique_ptr<PyClass> {
            return std::make_unique<PyClass>(MfccOptionsFromDict(dict));
          }));
}

void PybindFeatureMfcc(py::module &m) {
  PybindMfccOptions(m);
  PybindMfcc(m);
}

}  // namespace kaldifeat
