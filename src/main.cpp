#include <json/json.h>

class BasicDescribable : public json::Describable<BasicDescribable>
{
public:
    BasicDescribable() 
    {
        registerProperty("caption")
            .getter([](const BasicDescribable* bd) { return json::TypedValue(bd->caption); })
            .setter([](BasicDescribable* bd, const json::TypedValue& value) { bd->caption = value.asString(); });
        registerProperty("score")
            .getter([](const BasicDescribable* bd) { return json::TypedValue(bd->score); })
            .setter([](BasicDescribable* bd, const json::TypedValue& value) { bd->score = value.asInt(); });
    }

    json::DescribedProperties<BasicDescribable>& description() override
    {


        return schema();
    }
private:
    int score{ 0 };
    std::string caption{ "Название" };
};

int main(int argc, char* argv[]) 
{
    BasicDescribable bd;
    json::JSONWriter writer("text.json");

    auto object{ writer.object() };
    BasicDescribable::serialize(object, &bd);

    return 0;
}