#include <json/json.h>

namespace json
{

#pragma region JSONWriter

    JSONWriter::JSONWriter(std::string_view filepath) : _out{filepath.data()}
    {
        if (!_out.is_open())
        {
            auto message{std::stringstream() << "Couldn't open file " << filepath};
            throw std::exception(message.str().c_str());
        }
    }

    JSONWriter::~JSONWriter()
    {
        _out.close();
    }

    void JSONWriter::indent()
    {
        if (const auto count{_indents}; count > 0)
        {
            std::stringstream buffer;
            for (size_t i{0}; i < count; ++i)
                buffer << "\t";

            _out << buffer.str();
        }
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
    }

#pragma endregion

#pragma region JSONWriter::ArrayScope

    JSONWriter::ObjectScope JSONWriter::ArrayScope::element()
    {
        grammar();
        items(items() + 1);
        return writer().object();
    }

#pragma endregion

#pragma region JSONWriter::ObjectScope

    void JSONWriter::ObjectScope::field(std::string_view key, std::string_view value)
    {
        if (key.empty() || value.empty())
            return;

        grammar();
        writer().key(key);
        writer().value(value);
        items(items() + 1);
    }

    void JSONWriter::ObjectScope::field(std::string_view key, int value)
    {
        if (key.empty())
            return;

        grammar();
        writer().key(key);
        writer().value(value);
        items(items() + 1);
    }

    JSONWriter::ArrayScope JSONWriter::ObjectScope::array(std::string_view key)
    {
        grammar();
        writer().key(key);
        items(items() + 1);

        return writer().array();
    }

    JSONWriter::ObjectScope JSONWriter::ObjectScope::object(std::string_view key)
    {
        grammar();
        writer().key(key);

        items(items() + 1);

        return writer().object();
    }

#pragma endregion
}
