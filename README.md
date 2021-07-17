# kaldifeat

Feature extraction compatible with `Kaldi` using PyTorch, supporting
CUDA, batch processing, chunk processing, and autograd.


# Usage

Let us first generate a test wave using sox:

```bash
# generate a wave of 1.2 seconds, containing a sine-wave
# swept from 300 Hz to 3300 Hz
sox -n -r 16000 -b 16 test.wav synth 1.2 sine 300-3300
```

**HINT**: Download [test_wav][test_wav].

[test_wav]: kaldifeat/python/tests/test_data/test.wav

## Fbank

```python
import torchaudio

import kaldifeat

filename = "./test.wav"
wave, samp_freq = torchaudio.load(filename)

wave = wave.squeeze()

opts = kaldifeat.FbankOptions()
opts.frame_opts.dither = 0
# Yes, it has same options like `Kaldi`

fbank = kaldifeat.Fbank(opts)
features = fbank(wave)
```

To compute features that are compatible with `Kaldi`, wave samples have to be
scaled to the range `[-32768, 32768]`. WARNING: You don't have to do this if
you don't care about the compatibility with `Kaldi`

The following is an example:

```python
wave *= 32768
fbank = kaldifeat.Fbank(opts)
features = fbank(wave)
print(features[:3])
```

The output is:

```
tensor([[15.0074, 21.1730, 25.5286, 24.4644, 16.6994, 13.8480, 11.2087, 11.7952,
         10.3911, 10.4491, 10.3012,  9.8743,  9.6997,  9.3751,  9.3476,  9.3559,
          9.1074,  9.0032,  9.0312,  8.8399,  9.0822,  8.7442,  8.4023],
        [13.8785, 20.5647, 25.4956, 24.6966, 16.9541, 13.9163, 11.3364, 11.8449,
         10.2565, 10.5871, 10.3484,  9.7474,  9.6123,  9.3964,  9.0695,  9.1177,
          8.9136,  8.8425,  8.5920,  8.8315,  8.6226,  8.8605,  8.9763],
        [13.9475, 19.9410, 25.4494, 24.9051, 17.0004, 13.9207, 11.6667, 11.8217,
         10.3411, 10.7258, 10.0983,  9.8109,  9.6762,  9.4218,  9.1246,  8.7744,
          9.0863,  8.7488,  8.4695,  8.6710,  8.7728,  8.7405,  8.9824]])
```

You can compute the fbank feature for the same wave with `Kaldi` using the following commands:

```bash
echo "1 test.wav" > test.scp
compute-fbank-feats --dither=0 scp:test.scp ark,t:test.txt
head -n4 test.txt
```

The output is:

```
1  [
  15.00744 21.17303 25.52861 24.46438 16.69938 13.84804 11.2087 11.79517 10.3911 10.44909 10.30123 9.874329 9.699727 9.37509 9.347578 9.355928 9.107419 9.00323 9.031268 8.839916 9.082197 8.744139 8.40221
  13.87853 20.56466 25.49562 24.69662 16.9541 13.91633 11.33638 11.84495 10.25656 10.58718 10.34841 9.747416 9.612316 9.39642 9.06955 9.117751 8.913527 8.842571 8.59212 8.831518 8.622513 8.86048 8.976251
  13.94753 19.94101 25.4494 24.90511 17.00044 13.92074 11.66673 11.82172 10.34108 10.72575 10.09829 9.810879 9.676199 9.421767 9.124647 8.774353 9.086291 8.74897 8.469534 8.670973 8.772754 8.740549 8.982433
```

You can see that ``kaldifeat`` produces the same output as `Kaldi` (within some tolerance due to numerical precision).


**HINT**: Download [test_scp][test_scp] and [test_txt][test_txt].

[test_scp]: kaldifeat/python/tests/test_data/test.scp
[test_txt]: kaldifeat/python/tests/test_data/test.txt


To use GPU, you can use:

```python
import torch

opts = kaldifeat.FbankOptions()
opts.device = torch.device("cuda", 0)

fbank = kaldifeat.Fbank(opts)
features = fbank(wave.to(opts.device))
```

## MFCC

To compute MFCC features, please replace `kaldifeat.FbankOptions` and `kaldifeat.Fbank`
with `kaldifeat.MfccOptions` and `kaldifeat.Mfcc`, respectively.

Please refer to [kaldifeat/python/tests/test_fbank.py](kaldifeat/python/tests/test_fbank.py)
and [kaldifeat/python/tests/test_mfcc.py](kaldifeat/python/tests/test_mfcc.py)
for more examples.

**HINT**: In the examples, you can find that

- ``kaldifeat`` supports batch processing as well as chunk processing
- ``kaldifeat`` uses the same options as `Kaldi`'s `compute-fbank-feats` and `compute-mfcc-feats`

# Installation

## From PyPi with pip

If you install `kaldifeat` using `pip`, it will also install
PyTorch 1.8.1. If this is not what you want, please install `kaldifeat`
from source (see below).

```bash
pip install kaldifeat
```

## From source

The following are the commands to compile `kaldifeat` from source.
We assume that you have installed `cmake` and PyTorch.
cmake 3.11 is known to work. Other cmake versions may also work.
PyTorch 1.8.1 is known to work. Other PyTorch versions may also work.

```bash
mkdir /some/path
git clone https://github.com/csukuangfj/kaldifeat.git
cd kaldifeat
python setup.py install
```

To test whether `kaldifeat` was installed successfully, you can run:
```bash
python3 -c "import kaldifeat; print(kaldifeat.__version__)"
```