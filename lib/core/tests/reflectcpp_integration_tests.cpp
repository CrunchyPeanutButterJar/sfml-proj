#include <gtest/gtest.h>

#include <core/eventmanager.hpp>

TEST(serialization_deserialization, serialization_deserialization_of_bindings_is_symmetrical)
{
    using namespace core;

    auto bindings              = buildBindings();
    auto json_bindings         = serializeBindings(bindings);
    auto deserialized_bindings = deserializeBindings(json_bindings);
    EXPECT_TRUE(bindingsAreEquivalent(bindings, deserialized_bindings));
}
