#include <string_view>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <string>

namespace json
{
#pragma region forward-declaration

    class JSONWriter;
    class TypedValue;

    template<class ObjectClass>
    struct DescribedProperty;
    template <class ObjectClass>
    using DescribedProperties = std::vector<DescribedProperty<ObjectClass>>;

#pragma endregion

    enum class TokenType
    {
        Empty,
        BeginObject,
        EndObject,
        BeginArray,
        EndArray,
        String,
        Number,
        True,
        False,
        Null,
        Colon,
        Comma,
        End
    };

    class TypedValue
    {
    public:
        enum class PropertyType
        {
            integer,
            string
        };

        explicit TypedValue(int value) : _type{ PropertyType::integer }, _value{ std::to_string(value) } { }
        explicit TypedValue(std::string_view value) : _type{ PropertyType::string }, _value{ value } { }

        PropertyType type() const { return _type; }
        int asInt() const { return std::stoi(_value); }
        std::string_view asString() const { return _value; }
    private:
        PropertyType _type;
        std::string _value;
    };

#pragma region JSONWriter

    class JSONWriter
    {
    public:
        class Scope;
        class ArrayScope;
        class ObjectScope;

        class Scope
        {
        public:
            explicit Scope(JSONWriter &w) : _writer(w) {}
            ~Scope() = default;

            void grammar();

        protected:
            JSONWriter &writer() const { return _writer; }

        private:
            JSONWriter &_writer;
            size_t itemCount{0};
        };

        class ArrayScope final : public Scope
        {
        public:
            explicit ArrayScope(JSONWriter &writer) : Scope(writer)
            {
                writer.beginArray();
                writer.increaseIndents();
            }
            ArrayScope(const ArrayScope&) = delete;
            ArrayScope& operator=(const ArrayScope&) = delete;
            ~ArrayScope()
            {
                writer().decreaseIndents();
                writer().endArray();
            }

            ObjectScope element();
        };

        class ObjectScope final : public Scope
        {
        public:
            explicit ObjectScope(JSONWriter &writer) : Scope(writer)
            {
                writer.beginObject();
                writer.increaseIndents();
            }
            ObjectScope(const ObjectScope&) = delete;
            ObjectScope& operator=(const ObjectScope&) = delete;
            ~ObjectScope()
            {
                writer().decreaseIndents();
                writer().endObject();
            }

            void field(std::string_view key, TypedValue value);

            ArrayScope array(std::string_view key);
            ObjectScope object(std::string_view key);
        };

        ObjectScope object() { return ObjectScope(*this); }
        ArrayScope array() { return ArrayScope(*this); }

        explicit JSONWriter(std::string_view filepath);

    private:
        void indent();
        void increaseIndents() { _indents++; }
        void decreaseIndents() { _indents--; }

        void comma() { _out << ",\n"; }

        void beginObject() { _out << "{\n"; }
        void endObject();

        void beginArray() { _out << "[\n"; }
        void endArray();

        void key(std::string_view key) { _out << "\"" << key << "\": "; }
        void value(std::string_view value) { _out << "\"" << value << "\""; }
        void value(int value) { _out << value; }

        std::ofstream _out;
        size_t _indents{0};
    };

#pragma endregion

#pragma region Describable

    template <class ObjectClass>
    struct DescribedProperty
    {
        DescribedProperty(std::string_view key) : _key{key} {}

        using Getter = TypedValue (*)(const ObjectClass *);
        using Setter = void (*)(ObjectClass *, const TypedValue &);

        DescribedProperty &getter(Getter f)
        {
            _getter = f;
            return *this;
        }
        DescribedProperty &setter(Setter f)
        {
            _setter = f;
            return *this;
        }

        Getter _getter = nullptr;
        Setter _setter = nullptr;
        std::string_view _key;
    };

    template <class ObjectClass>
    class Describable
    {
    public:
        Describable() = default;
        virtual ~Describable() = default;
        
        static void serialize(JSONWriter::ObjectScope& scope, ObjectClass* object)
        {
            auto& properties{ object->description() };
            for (auto& property : properties)
                scope.field(property._key, property._getter(object));
        }
    protected:
        virtual DescribedProperties<ObjectClass> &description() = 0;

        DescribedProperty<ObjectClass> &registerProperty(std::string_view key)
        {
            _properties.emplace_back(key);
            return _properties.back();
        }

        DescribedProperties<ObjectClass> &schema() { return _properties; }

    private:
        DescribedProperties<ObjectClass> _properties;
    };

#pragma endregion

}