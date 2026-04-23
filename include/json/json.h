#include <string_view>
#include <sstream>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>

namespace json
{
#pragma region forward-declaration

    class JSONWriter;

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
            size_t items() const { return itemCount; }
            void items(size_t value) { itemCount = value; }

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
            ArrayScope(ArrayScope &scope) = delete;
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
            ObjectScope(ObjectScope &scope) = delete;
            ~ObjectScope()
            {
                writer().decreaseIndents();
                writer().endObject();
            }

            void field(std::string_view key, int value);
            void field(std::string_view key, std::string_view value);

            ArrayScope array(std::string_view key);
            ObjectScope object(std::string_view key);
        };

        ObjectScope object() { return ObjectScope(*this); }
        ArrayScope array() { return ArrayScope(*this); }

        JSONWriter(std::string_view filepath);
        ~JSONWriter();

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
        void value(int value) { _out << "\"" << value << "\""; }

        const std::string_view _filename;
        std::ofstream _out;
        size_t _indents{0};

        const std::string _comma{", "};
    };

#pragma endregion

#pragma region Describable

    template <class ObjectClass>
    struct DescribedProperty
    {
        DescribedProperty(std::string_view key) : _key{key} {}

        using Getter = std::string_view (*)(const ObjectClass *);
        using Setter = void (*)(ObjectClass *, const std::string_view &);

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
        explicit Describable() = default;
        virtual ~Describable() = default;
        virtual DescribedProperties<ObjectClass> &description() = 0;

        static void serialize(JSONWriter::ObjectScope& scope, ObjectClass* object)
        {
            auto& properties{ object->description() };
            for (auto& property : properties)
                scope.field(property._key, property._getter(object));
        }
    protected:
        DescribedProperty<ObjectClass> &registerProperty(std::string_view key)
        {
            _properties.emplace_back(std::move(DescribedProperty<ObjectClass>(key)));
            return _properties[_properties.size() - 1];
        }

        DescribedProperties<ObjectClass> &schema() { return _properties; };

    private:
        DescribedProperties<ObjectClass> _properties;
    };

#pragma endregion

}