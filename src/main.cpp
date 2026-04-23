#include <json/json.h>
#include <string_view>

class BasicDescribable : public json::Describable<BasicDescribable>
{
public:
    BasicDescribable() = default;
    
    json::DescribedProperties<BasicDescribable>& description() override
    {
        registerProperty("score")
            .getter([](const BasicDescribable* bd) -> std::string_view { return bd->caption; })
            .setter([](BasicDescribable* bd, const std::string_view& value) { bd->caption = value; });

        return schema();
    }
private:
    int score{ 0 };
    std::string caption { "Название" };
};

int main(int argc, char* argv[]) 
{
    BasicDescribable bd;
    json::JSONWriter reader("text.json");

    auto object { reader.object() };
    BasicDescribable::serialize(object, &bd);

    return 0;
}