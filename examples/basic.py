import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "build"))

import simdjson_test_wrapper

text = '{"a":[1,true,null],"b":{"x":"y"}}'

value = simdjson_test_wrapper.toJson(text)
output = simdjson_test_wrapper.toStr(value)

print(value)
print(output)
