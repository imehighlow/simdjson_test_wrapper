# simdjson-test-wrapper

Minimal Python extension for JSON conversion.

```python
import simdjson_test_wrapper

value = simdjson_test_wrapper.toJson('{"a":[1,true,null]}')
text = simdjson_test_wrapper.toStr(value)
```

## Build

```bash
python -m pip install build
python -m build
```

## Local Install

```bash
python -m pip install .
```
