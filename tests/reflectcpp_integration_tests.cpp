#include <gtest/gtest.h>

#include <eventmanager.hpp>

TEST(serialization_deserialization, serialization_deserialization_of_bindings_is_symmetrical)
{
  auto bindings = buildBindings();
  auto jsonBindings = serializeBindings(bindings);
  auto deserializedBindings = deserializeBindings(jsonBindings);
  EXPECT_TRUE(bindingsAreEquivalent(bindings, deserializedBindings));
}
