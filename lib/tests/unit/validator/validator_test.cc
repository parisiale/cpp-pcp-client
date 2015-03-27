#include "tests/test.hpp"

#include <cthun-client/validator/validator.hpp>
#include <cthun-client/validator/schema.hpp>
#include <cthun-client/data_container/data_container.hpp>

namespace CthunClient {

TEST_CASE("Validator::registerSchema", "[validation]") {
    Validator validator {};
    Schema schema { "spam" };

    SECTION("it can register a schema") {
        validator.registerSchema(schema);
    }

    SECTION("it cannot register a schema name more than once") {
        validator.registerSchema(schema);
        REQUIRE_THROWS_AS(validator.registerSchema(schema),
                          schema_redefinition_error);
    }
}

TEST_CASE("Validator::validate", "[validation]") {
    DataContainer data {};
    Schema schema { "test-schema" };
    Validator validator {};

    SECTION("it throws a schema_not_found_error if the requested schema was "
            "not registered") {
        REQUIRE_THROWS_AS(validator.validate(data, "test-schema"),
                          schema_not_found_error);
    }

    SECTION("it throws a validation_error when validation fails") {
        data.set<std::string>("key", "value");
        schema.addConstraint("key", TypeConstraint::Int);
        validator.registerSchema(schema);
        REQUIRE_THROWS_AS(validator.validate(data, "test-schema"),
                          validation_error);
    }

    SECTION("it doesn't throw when validation succeeds") {
        data.set<std::string>("key", "value");
        schema.addConstraint("key", TypeConstraint::String);
        validator.registerSchema(schema);
        REQUIRE_NOTHROW(validator.validate(data, "test-schema"));
    }

    // TODO(ale): move old SECTION("default schemas") to Connector test
}

TEST_CASE("Validator::getSchemaContentType", "[validation]") {
    Schema schema { "foo", ContentType::Binary };
    Validator validator {};

    SECTION("it can return the schema's content type") {
        validator.registerSchema(schema);
        REQUIRE(validator.getSchemaContentType("foo") == ContentType::Binary);
    }

    SECTION("it throws when the schema is undefined") {
        REQUIRE_THROWS_AS(validator.getSchemaContentType("foo"),
                          schema_not_found_error);
    }
}

}  // namespace CthunClient