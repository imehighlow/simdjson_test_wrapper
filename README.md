# simdjson-test-wrapper

Test task package built with `conda-build`.

The package is a small compiled Python extension written in C++ with
`pybind11`. It uses `simdjson` v4.6.2 as an external C++ dependency and exposes
two functions:

- `toJson(str)`: parse a JSON string into Python `dict` / `list` / primitive
  values.
- `toStr(value)`: serialize Python `dict` / `list` / primitive values back to
  compact JSON text.

```python
import simdjson_test_wrapper

value = simdjson_test_wrapper.toJson('{"a":[1,true,null]}')
text = simdjson_test_wrapper.toStr(value)

print(value)
print(text)
```

## Links

- GitHub repository: https://github.com/imehighlow/simdjson_test_wrapper
- Anaconda.org package: https://anaconda.org/mehighlow/simdjson-test-wrapper
- Conda build tutorial followed for this task:
  https://docs.conda.io/projects/conda-build/en/latest/user-guide/tutorials/build-pkgs.html

## Install From Anaconda.org

```bash
conda install -c mehighlow simdjson-test-wrapper
```

Smoke test:

```bash
python -c "import simdjson_test_wrapper as s; print(s.toStr(s.toJson('[1,true,null]')))"
```

Expected output:

```text
[1,true,null]
```

## Repository Layout

```text
.
├── CMakeLists.txt
├── pyproject.toml
├── recipe/
│   └── meta.yaml
├── src/
│   └── simdjson_test_wrapper.cpp
└── examples/
    └── basic.py
```

The Conda recipe is in `recipe/meta.yaml`. The C++ extension is built through
CMake and `scikit-build-core`.

## Build Locally With Conda

```bash
conda build recipe
```

Install the locally built package:

```bash
conda install --use-local simdjson-test-wrapper
```

## Build Python Wheel Locally

```bash
python -m pip install build scikit-build-core pybind11
python -m build
```

## CI

GitHub Actions builds Conda packages for:

- Linux x86_64
- macOS arm64
- Windows x86_64
- Python 3.11, 3.12, and 3.13

Packages are uploaded to Anaconda.org from version tags such as `v0.1.0`.
