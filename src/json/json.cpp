#include <json/json.h>

namespace json
{

#pragma region JSONWriter

    JSONWriter::JSONWriter(std::string_view filepath) : _out{filepath.data()}
    {
        if (!_out.is_open())
            throw std::runtime_error(std::string("Couldn't open file ") + std::string(filepath));
    }

    void JSONWriter::indent()
    {
        for (size_t i{0}; i < _indents; ++i)
            _out << '\t';
    }

    void JSONWriter::endObject()
    {
        _out << "\n";
        indent();
        _out << "}";
    }

    void JSONWriter::endArray()
    {
        _out << "\n";
        indent();
        _out << "]";
    }

#pragma endregion

#pragma region JSONWriter::Scope

    void JSONWriter::Scope::grammar()
    {
        if (itemCount > 0)
            _writer.comma();
        _writer.indent();
        itemCount++;
    }

#pragma endregion

#pragma region JSONWriter::ArrayScope

    JSONWriter::ObjectScope JSONWriter::ArrayScope::element()
    {
        grammar();
        return writer().object();
    }

#pragma endregion

#pragma region JSONWriter::ObjectScope

    void JSONWriter::ObjectScope::field(std::string_view key, TypedValue value)
    {
        if (key.empty())
            return;

        grammar();
        writer().key(key);

        switch (value.type())
        {
            case TypedValue::PropertyType::integer:
                writer().value(value.asInt());
                break;
            case TypedValue::PropertyType::string:
                writer().value(value.asString());
                break;
            default:
                throw std::runtime_error(std::string("Unexpected TypedValue type"));
                break;
        }
    }

    JSONWriter::ArrayScope JSONWriter::ObjectScope::array(std::string_view key)
    {
        grammar();
        writer().key(key);
        return writer().array();
    }

    JSONWriter::ObjectScope JSONWriter::ObjectScope::object(std::string_view key)
    {
        grammar();
        writer().key(key);
        return writer().object();
    }

#pragma endregion
}
