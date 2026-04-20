#include <cmath>
#include <cstdint>
#include <iomanip>
#include <pybind11/pybind11.h>
#include <simdjson.h>
#include <sstream>
#include <string>
#include <string_view>

namespace py = pybind11;

static_assert(simdjson::SIMDJSON_VERSION_MAJOR == 4 && simdjson::SIMDJSON_VERSION_MINOR == 6 &&
                  simdjson::SIMDJSON_VERSION_REVISION == 2,
              "simdjson-test-wrapper requires simdjson v4.6.2");

py::object toPy(simdjson::dom::element value);

py::object toPy(simdjson::dom::array value) {
    py::list out;

    for (simdjson::dom::element item : value) {
        out.append(toPy(item));
    }

    return out;
}

py::object toPy(simdjson::dom::object value) {
    py::dict out;

    for (auto field : value) {
        out[py::str(std::string(field.key))] = toPy(field.value);
    }

    return out;
}

py::object toBigInt(std::string_view text) {
    std::string digits(text);
    PyObject* value = PyLong_FromString(digits.data(), nullptr, 10);

    if (value == nullptr) {
        throw py::error_already_set();
    }

    return py::reinterpret_steal<py::object>(value);
}

py::object toPy(simdjson::dom::element value) {
    using simdjson::dom::element_type;

    switch (value.type()) {
        case element_type::OBJECT: {
            simdjson::dom::object object = value.get_object();
            return toPy(object);
        }

        case element_type::ARRAY: {
            simdjson::dom::array array = value.get_array();
            return toPy(array);
        }

        case element_type::STRING: {
            std::string_view text = value.get_string();
            return py::str(std::string(text));
        }

        case element_type::INT64: {
            int64_t number = value.get_int64();
            return py::int_(number);
        }

        case element_type::UINT64: {
            uint64_t number = value.get_uint64();
            return py::int_(number);
        }

        case element_type::DOUBLE: {
            double number = value.get_double();
            return py::float_(number);
        }

        case element_type::BOOL: {
            bool boolean = value.get_bool();
            return py::bool_(boolean);
        }

        case element_type::NULL_VALUE:
            return py::none();

        case element_type::BIGINT:
            return toBigInt(value.get_bigint());
    }

    return py::none();
}

py::object toJson(const std::string& text) {
    simdjson::padded_string source(text);
    simdjson::dom::parser parser;
    simdjson::dom::element root;

    auto error = parser.parse(source).get(root);
    if (error) {
        throw py::value_error(simdjson::error_message(error));
    }

    return toPy(root);
}

void appendString(std::string_view text, std::string& out) {
    out.push_back('"');

    for (unsigned char ch : text) {
        switch (ch) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\b':
                out += "\\b";
                break;
            case '\f':
                out += "\\f";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if (ch < 0x20) {
                    constexpr char hex[] = "0123456789abcdef";
                    out += "\\u00";
                    out.push_back(hex[ch >> 4]);
                    out.push_back(hex[ch & 0x0f]);
                } else {
                    out.push_back(static_cast<char>(ch));
                }
                break;
        }
    }

    out.push_back('"');
}

void appendJson(py::handle value, std::string& out);

void appendList(py::handle value, std::string& out) {
    py::sequence list = py::reinterpret_borrow<py::sequence>(value);

    out.push_back('[');
    bool first = true;

    for (py::handle item : list) {
        if (!first) {
            out.push_back(',');
        }

        first = false;
        appendJson(item, out);
    }

    out.push_back(']');
}

void appendDict(py::handle value, std::string& out) {
    py::dict dict = py::reinterpret_borrow<py::dict>(value);

    out.push_back('{');
    bool first = true;

    for (auto item : dict) {
        if (!first) {
            out.push_back(',');
        }

        first = false;
        appendString(py::str(item.first).cast<std::string>(), out);
        out.push_back(':');
        appendJson(item.second, out);
    }

    out.push_back('}');
}

void appendJson(py::handle value, std::string& out) {
    if (value.is_none()) {
        out += "null";
    } else if (PyBool_Check(value.ptr())) {
        out += value.is(py::bool_(true)) ? "true" : "false";
    } else if (PyLong_Check(value.ptr())) {
        out += py::str(value).cast<std::string>();
    } else if (PyFloat_Check(value.ptr())) {
        double number = PyFloat_AsDouble(value.ptr());
        if (!std::isfinite(number)) {
            throw py::value_error("JSON does not support NaN or Infinity");
        }

        std::ostringstream stream;
        stream << std::setprecision(17) << number;
        out += stream.str();
    } else if (PyUnicode_Check(value.ptr())) {
        appendString(py::str(value).cast<std::string>(), out);
    } else if (PyDict_Check(value.ptr())) {
        appendDict(value, out);
    } else if (PyList_Check(value.ptr())) {
        appendList(value, out);
    } else {
        throw py::type_error("toStr expects dict, list, str, int, float, bool, or None");
    }
}

std::string toStr(py::handle value) {
    std::string out;
    appendJson(value, out);
    return out;
}

PYBIND11_MODULE(simdjson_test_wrapper, m) {
    m.def("toJson", &toJson);
    m.def("toStr", &toStr);
}
