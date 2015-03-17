// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include <gtest/gtest.h>

#include <limits>
#include <memory>

#include "parser/values.h"

namespace parser {
namespace utils {

TEST(ValuesTest, Basic) {
  // Test basic dictionary getting/setting
  DictionaryValue settings;
  std::string homepage = "http://google.com";
  ASSERT_FALSE(settings.GetString("global.homepage", &homepage));
  ASSERT_EQ(std::string("http://google.com"), homepage);

  ASSERT_FALSE(settings.Get("global", nullptr));
  settings.Set("global", new FundamentalValue(true));
  ASSERT_TRUE(settings.Get("global", nullptr));
  settings.SetString("global.homepage", "http://scurvy.com");
  ASSERT_TRUE(settings.Get("global", nullptr));
  homepage = "http://google.com";
  ASSERT_TRUE(settings.GetString("global.homepage", &homepage));
  ASSERT_EQ(std::string("http://scurvy.com"), homepage);

  // Test storing a dictionary in a list.
  ListValue* toolbar_bookmarks;
  ASSERT_FALSE(
    settings.GetList("global.toolbar.bookmarks", &toolbar_bookmarks));

  toolbar_bookmarks = new ListValue;
  settings.Set("global.toolbar.bookmarks", toolbar_bookmarks);
  ASSERT_TRUE(settings.GetList("global.toolbar.bookmarks", &toolbar_bookmarks));

  DictionaryValue* new_bookmark = new DictionaryValue;
  new_bookmark->SetString("name", "Froogle");
  new_bookmark->SetString("url", "http://froogle.com");
  toolbar_bookmarks->Append(new_bookmark);

  ListValue* bookmark_list;
  ASSERT_TRUE(settings.GetList("global.toolbar.bookmarks", &bookmark_list));
  DictionaryValue* bookmark;
  ASSERT_EQ(1U, bookmark_list->GetSize());
  ASSERT_TRUE(bookmark_list->GetDictionary(0, &bookmark));
  std::string bookmark_name = "Unnamed";
  ASSERT_TRUE(bookmark->GetString("name", &bookmark_name));
  ASSERT_EQ(std::string("Froogle"), bookmark_name);
  std::string bookmark_url;
  ASSERT_TRUE(bookmark->GetString("url", &bookmark_url));
  ASSERT_EQ(std::string("http://froogle.com"), bookmark_url);
}

TEST(ValuesTest, List) {
  std::unique_ptr<ListValue> mixed_list(new ListValue());
  mixed_list->Set(0, new FundamentalValue(true));
  mixed_list->Set(1, new FundamentalValue(42));
  mixed_list->Set(2, new FundamentalValue(88.8));
  mixed_list->Set(3, new StringValue("foo"));
  ASSERT_EQ(4u, mixed_list->GetSize());

  Value *value = nullptr;
  bool bool_value = false;
  int int_value = 0;
  double double_value = 0.0;
  std::string string_value;

  ASSERT_FALSE(mixed_list->Get(4, &value));

  ASSERT_FALSE(mixed_list->GetInteger(0, &int_value));
  ASSERT_EQ(0, int_value);
  ASSERT_FALSE(mixed_list->GetBoolean(1, &bool_value));
  ASSERT_FALSE(bool_value);
  ASSERT_FALSE(mixed_list->GetString(2, &string_value));
  ASSERT_EQ("", string_value);
  ASSERT_FALSE(mixed_list->GetInteger(2, &int_value));
  ASSERT_EQ(0, int_value);
  ASSERT_FALSE(mixed_list->GetBoolean(3, &bool_value));
  ASSERT_FALSE(bool_value);

  ASSERT_TRUE(mixed_list->GetBoolean(0, &bool_value));
  ASSERT_TRUE(bool_value);
  ASSERT_TRUE(mixed_list->GetInteger(1, &int_value));
  ASSERT_EQ(42, int_value);
  // implicit conversion from Integer to Double should be possible.
  ASSERT_TRUE(mixed_list->GetDouble(1, &double_value));
  ASSERT_EQ(42, double_value);
  ASSERT_TRUE(mixed_list->GetDouble(2, &double_value));
  ASSERT_EQ(88.8, double_value);
  ASSERT_TRUE(mixed_list->GetString(3, &string_value));
  ASSERT_EQ("foo", string_value);

  // Try searching in the mixed list.
  utils::FundamentalValue sought_value(42);
  utils::FundamentalValue not_found_value(false);

  ASSERT_NE(mixed_list->end(), mixed_list->Find(sought_value));
  ASSERT_TRUE((*mixed_list->Find(sought_value))->GetAsInteger(&int_value));
  ASSERT_EQ(42, int_value);
  ASSERT_EQ(mixed_list->end(), mixed_list->Find(not_found_value));
}

TEST(ValuesTest, BinaryValue) {
  // Default constructor creates a BinaryValue with a nullptr buffer and size 0.
  std::unique_ptr<BinaryValue> binary(new BinaryValue());
  ASSERT_TRUE(binary.get());
  ASSERT_EQ(nullptr, binary->GetBuffer());
  ASSERT_EQ(0U, binary->GetSize());

  // Test the common case of a non-empty buffer
  char* buffer = new char[15];
  binary.reset(new BinaryValue(std::unique_ptr<char[]>(buffer), 15));
  ASSERT_TRUE(binary.get());
  ASSERT_TRUE(binary->GetBuffer());
  ASSERT_EQ(buffer, binary->GetBuffer());
  ASSERT_EQ(15U, binary->GetSize());

  char stack_buffer[42];
  memset(stack_buffer, '!', 42);
  binary.reset(BinaryValue::CreateWithCopiedBuffer(stack_buffer, 42));
  ASSERT_TRUE(binary.get());
  ASSERT_TRUE(binary->GetBuffer());
  ASSERT_NE(stack_buffer, binary->GetBuffer());
  ASSERT_EQ(42U, binary->GetSize());
  ASSERT_EQ(0, memcmp(stack_buffer, binary->GetBuffer(), binary->GetSize()));
}

TEST(ValuesTest, StringValue) {
  // Test overloaded StringValue constructor.
  std::unique_ptr<Value> narrow_value(new StringValue("narrow"));
  ASSERT_TRUE(narrow_value.get());
  ASSERT_TRUE(narrow_value->IsType(Value::TYPE_STRING));
  std::unique_ptr<Value> utf16_value(new StringValue("utf16"));
  ASSERT_TRUE(utf16_value.get());
  ASSERT_TRUE(utf16_value->IsType(Value::TYPE_STRING));

  // Test overloaded GetAsString.
  std::string narrow = "http://google.com";
  std::string utf16 = "http://google.com";
  const StringValue* string_value = nullptr;
  ASSERT_TRUE(narrow_value->GetAsString(&narrow));
  ASSERT_TRUE(narrow_value->GetAsString(&utf16));
  ASSERT_TRUE(narrow_value->GetAsString(&string_value));
  ASSERT_EQ(std::string("narrow"), narrow);
  ASSERT_EQ(std::string("narrow"), utf16);
  ASSERT_EQ(string_value->GetString(), narrow);

  ASSERT_TRUE(utf16_value->GetAsString(&narrow));
  ASSERT_TRUE(utf16_value->GetAsString(&utf16));
  ASSERT_TRUE(utf16_value->GetAsString(&string_value));
  ASSERT_EQ(std::string("utf16"), narrow);
  ASSERT_EQ(std::string("utf16"), utf16);
  ASSERT_EQ(string_value->GetString(), narrow);

  // Don't choke on nullptr values.
  ASSERT_TRUE(narrow_value->GetAsString(static_cast<std::string*>(nullptr)));
  ASSERT_TRUE(narrow_value->GetAsString(static_cast<std::string*>(nullptr)));
  ASSERT_TRUE(narrow_value->GetAsString(
                  static_cast<const StringValue**>(nullptr)));
}

// This is a Value object that allows us to tell if it's been
// properly deleted by modifying the value of external flag on destruction.
class DeletionTestValue : public Value {
 public:
  explicit DeletionTestValue(bool* deletion_flag) : Value(TYPE_NULL) {
    Init(deletion_flag);  // Separate function so that we can use ASSERT_*
  }

  void Init(bool* deletion_flag) {
    ASSERT_TRUE(deletion_flag);
    deletion_flag_ = deletion_flag;
    *deletion_flag_ = false;
  }

  ~DeletionTestValue() override { *deletion_flag_ = true; }

 private:
  bool* deletion_flag_;
};

TEST(ValuesTest, ListDeletion) {
  bool deletion_flag = true;

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
  }
  EXPECT_TRUE(deletion_flag);

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    list.Clear();
    EXPECT_TRUE(deletion_flag);
  }

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(list.Set(0, Value::CreateNullValue()));
    EXPECT_TRUE(deletion_flag);
  }
}

TEST(ValuesTest, ListRemoval) {
  bool deletion_flag = true;
  std::unique_ptr<Value> removed_item;

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_EQ(1U, list.GetSize());
    EXPECT_FALSE(list.Remove(std::numeric_limits<size_t>::max(),
                             &removed_item));
    EXPECT_FALSE(list.Remove(1, &removed_item));
    EXPECT_TRUE(list.Remove(0, &removed_item));
    ASSERT_TRUE(removed_item ? true : false);
    EXPECT_EQ(0U, list.GetSize());
  }
  EXPECT_FALSE(deletion_flag);
  removed_item.reset();
  EXPECT_TRUE(deletion_flag);

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(list.Remove(0, nullptr));
    EXPECT_TRUE(deletion_flag);
    EXPECT_EQ(0U, list.GetSize());
  }

  {
    ListValue list;
    DeletionTestValue* value = new DeletionTestValue(&deletion_flag);
    list.Append(value);
    EXPECT_FALSE(deletion_flag);
    size_t index = 0;
    list.Remove(*value, &index);
    EXPECT_EQ(0U, index);
    EXPECT_TRUE(deletion_flag);
    EXPECT_EQ(0U, list.GetSize());
  }
}

TEST(ValuesTest, DictionaryDeletion) {
  std::string key = "test";
  bool deletion_flag = true;

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
  }
  EXPECT_TRUE(deletion_flag);

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    dict.Clear();
    EXPECT_TRUE(deletion_flag);
  }

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    dict.Set(key, Value::CreateNullValue());
    EXPECT_TRUE(deletion_flag);
  }
}

TEST(ValuesTest, DictionaryRemoval) {
  std::string key = "test";
  bool deletion_flag = true;
  std::unique_ptr<Value> removed_item;

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(dict.HasKey(key));
    EXPECT_FALSE(dict.Remove("absent key", &removed_item));
    EXPECT_TRUE(dict.Remove(key, &removed_item));
    EXPECT_FALSE(dict.HasKey(key));
    ASSERT_TRUE(removed_item ? true : false);
  }
  EXPECT_FALSE(deletion_flag);
  removed_item.reset();
  EXPECT_TRUE(deletion_flag);

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(dict.HasKey(key));
    EXPECT_TRUE(dict.Remove(key, nullptr));
    EXPECT_TRUE(deletion_flag);
    EXPECT_FALSE(dict.HasKey(key));
  }
}

TEST(ValuesTest, DictionaryWithoutPathExpansion) {
  DictionaryValue dict;
  dict.Set("this.is.expanded", Value::CreateNullValue());
  dict.SetWithoutPathExpansion("this.isnt.expanded", Value::CreateNullValue());

  EXPECT_FALSE(dict.HasKey("this.is.expanded"));
  EXPECT_TRUE(dict.HasKey("this"));
  Value* value1;
  EXPECT_TRUE(dict.Get("this", &value1));
  DictionaryValue* value2;
  ASSERT_TRUE(dict.GetDictionaryWithoutPathExpansion("this", &value2));
  EXPECT_EQ(value1, value2);
  EXPECT_EQ(1U, value2->size());

  EXPECT_TRUE(dict.HasKey("this.isnt.expanded"));
  Value* value3;
  EXPECT_FALSE(dict.Get("this.isnt.expanded", &value3));
  Value* value4;
  ASSERT_TRUE(dict.GetWithoutPathExpansion("this.isnt.expanded", &value4));
  EXPECT_EQ(Value::TYPE_NULL, value4->GetType());
}

// Tests the deprecated version of SetWithoutPathExpansion.
// TODO(estade): remove.
TEST(ValuesTest, DictionaryWithoutPathExpansionDeprecated) {
  DictionaryValue dict;
  dict.Set("this.is.expanded", Value::CreateNullValue());
  dict.SetWithoutPathExpansion("this.isnt.expanded", Value::CreateNullValue());

  EXPECT_FALSE(dict.HasKey("this.is.expanded"));
  EXPECT_TRUE(dict.HasKey("this"));
  Value* value1;
  EXPECT_TRUE(dict.Get("this", &value1));
  DictionaryValue* value2;
  ASSERT_TRUE(dict.GetDictionaryWithoutPathExpansion("this", &value2));
  EXPECT_EQ(value1, value2);
  EXPECT_EQ(1U, value2->size());

  EXPECT_TRUE(dict.HasKey("this.isnt.expanded"));
  Value* value3;
  EXPECT_FALSE(dict.Get("this.isnt.expanded", &value3));
  Value* value4;
  ASSERT_TRUE(dict.GetWithoutPathExpansion("this.isnt.expanded", &value4));
  EXPECT_EQ(Value::TYPE_NULL, value4->GetType());
}

TEST(ValuesTest, DictionaryRemovePath) {
  DictionaryValue dict;
  dict.SetInteger("a.long.way.down", 1);
  dict.SetBoolean("a.long.key.path", true);

  std::unique_ptr<Value> removed_item;
  EXPECT_TRUE(dict.RemovePath("a.long.way.down", &removed_item));
  ASSERT_TRUE(removed_item ? true : false);
  EXPECT_TRUE(removed_item->IsType(utils::Value::TYPE_INTEGER));
  EXPECT_FALSE(dict.HasKey("a.long.way.down"));
  EXPECT_FALSE(dict.HasKey("a.long.way"));
  EXPECT_TRUE(dict.Get("a.long.key.path", nullptr));

  removed_item.reset();
  EXPECT_FALSE(dict.RemovePath("a.long.way.down", &removed_item));
  EXPECT_FALSE(removed_item);
  EXPECT_TRUE(dict.Get("a.long.key.path", nullptr));

  removed_item.reset();
  EXPECT_TRUE(dict.RemovePath("a.long.key.path", &removed_item));
  ASSERT_TRUE(removed_item ? true : false);
  EXPECT_TRUE(removed_item->IsType(utils::Value::TYPE_BOOLEAN));
  EXPECT_TRUE(dict.empty());
}

TEST(ValuesTest, DeepCopy) {
  DictionaryValue original_dict;
  Value* original_nullptr = Value::CreateNullValue();
  original_dict.Set("nullptr", original_nullptr);
  FundamentalValue* original_bool = new FundamentalValue(true);
  original_dict.Set("bool", original_bool);
  FundamentalValue* original_int = new FundamentalValue(42);
  original_dict.Set("int", original_int);
  FundamentalValue* original_double = new FundamentalValue(3.14);
  original_dict.Set("double", original_double);
  StringValue* original_string = new StringValue("hello");
  original_dict.Set("string", original_string);
  StringValue* original_string16 = new StringValue("hello16");
  original_dict.Set("string16", original_string16);

  std::unique_ptr<char[]> original_buffer(new char[42]);
  memset(original_buffer.get(), '!', 42);
  BinaryValue* original_binary =
      new BinaryValue(std::move(original_buffer), 42);
  original_dict.Set("binary", original_binary);

  ListValue* original_list = new ListValue();
  FundamentalValue* original_list_element_0 = new FundamentalValue(0);
  original_list->Append(original_list_element_0);
  FundamentalValue* original_list_element_1 = new FundamentalValue(1);
  original_list->Append(original_list_element_1);
  original_dict.Set("list", original_list);

  DictionaryValue* original_nested_dictionary = new DictionaryValue();
  original_nested_dictionary->SetString("key", "value");
  original_dict.Set("dictionary", original_nested_dictionary);

  std::unique_ptr<DictionaryValue> copy_dict(original_dict.DeepCopy());
  ASSERT_TRUE(copy_dict.get());
  ASSERT_NE(copy_dict.get(), &original_dict);

  Value* copy_nullptr = nullptr;
  ASSERT_TRUE(copy_dict->Get("nullptr", &copy_nullptr));
  ASSERT_TRUE(copy_nullptr);
  ASSERT_NE(copy_nullptr, original_nullptr);
  ASSERT_TRUE(copy_nullptr->IsType(Value::TYPE_NULL));

  Value* copy_bool = nullptr;
  ASSERT_TRUE(copy_dict->Get("bool", &copy_bool));
  ASSERT_TRUE(copy_bool);
  ASSERT_NE(copy_bool, original_bool);
  ASSERT_TRUE(copy_bool->IsType(Value::TYPE_BOOLEAN));
  bool copy_bool_value = false;
  ASSERT_TRUE(copy_bool->GetAsBoolean(&copy_bool_value));
  ASSERT_TRUE(copy_bool_value);

  Value* copy_int = nullptr;
  ASSERT_TRUE(copy_dict->Get("int", &copy_int));
  ASSERT_TRUE(copy_int);
  ASSERT_NE(copy_int, original_int);
  ASSERT_TRUE(copy_int->IsType(Value::TYPE_INTEGER));
  int copy_int_value = 0;
  ASSERT_TRUE(copy_int->GetAsInteger(&copy_int_value));
  ASSERT_EQ(42, copy_int_value);

  Value* copy_double = nullptr;
  ASSERT_TRUE(copy_dict->Get("double", &copy_double));
  ASSERT_TRUE(copy_double);
  ASSERT_NE(copy_double, original_double);
  ASSERT_TRUE(copy_double->IsType(Value::TYPE_DOUBLE));
  double copy_double_value = 0;
  ASSERT_TRUE(copy_double->GetAsDouble(&copy_double_value));
  ASSERT_EQ(3.14, copy_double_value);

  Value* copy_string = nullptr;
  ASSERT_TRUE(copy_dict->Get("string", &copy_string));
  ASSERT_TRUE(copy_string);
  ASSERT_NE(copy_string, original_string);
  ASSERT_TRUE(copy_string->IsType(Value::TYPE_STRING));
  std::string copy_string_value;
  std::string copy_string16_value;
  ASSERT_TRUE(copy_string->GetAsString(&copy_string_value));
  ASSERT_TRUE(copy_string->GetAsString(&copy_string16_value));
  ASSERT_EQ(std::string("hello"), copy_string_value);
  ASSERT_EQ(std::string("hello"), copy_string16_value);

  Value* copy_string16 = nullptr;
  ASSERT_TRUE(copy_dict->Get("string16", &copy_string16));
  ASSERT_TRUE(copy_string16);
  ASSERT_NE(copy_string16, original_string16);
  ASSERT_TRUE(copy_string16->IsType(Value::TYPE_STRING));
  ASSERT_TRUE(copy_string16->GetAsString(&copy_string_value));
  ASSERT_TRUE(copy_string16->GetAsString(&copy_string16_value));
  ASSERT_EQ(std::string("hello16"), copy_string_value);
  ASSERT_EQ(std::string("hello16"), copy_string16_value);

  Value* copy_binary = nullptr;
  ASSERT_TRUE(copy_dict->Get("binary", &copy_binary));
  ASSERT_TRUE(copy_binary);
  ASSERT_NE(copy_binary, original_binary);
  ASSERT_TRUE(copy_binary->IsType(Value::TYPE_BINARY));
  ASSERT_NE(original_binary->GetBuffer(),
    static_cast<BinaryValue*>(copy_binary)->GetBuffer());
  ASSERT_EQ(original_binary->GetSize(),
    static_cast<BinaryValue*>(copy_binary)->GetSize());
  ASSERT_EQ(0, memcmp(original_binary->GetBuffer(),
               static_cast<BinaryValue*>(copy_binary)->GetBuffer(),
               original_binary->GetSize()));

  Value* copy_value = nullptr;
  ASSERT_TRUE(copy_dict->Get("list", &copy_value));
  ASSERT_TRUE(copy_value);
  ASSERT_NE(copy_value, original_list);
  ASSERT_TRUE(copy_value->IsType(Value::TYPE_LIST));
  ListValue* copy_list = nullptr;
  ASSERT_TRUE(copy_value->GetAsList(&copy_list));
  ASSERT_TRUE(copy_list);
  ASSERT_EQ(2U, copy_list->GetSize());

  Value* copy_list_element_0;
  ASSERT_TRUE(copy_list->Get(0, &copy_list_element_0));
  ASSERT_TRUE(copy_list_element_0);
  ASSERT_NE(copy_list_element_0, original_list_element_0);
  int copy_list_element_0_value;
  ASSERT_TRUE(copy_list_element_0->GetAsInteger(&copy_list_element_0_value));
  ASSERT_EQ(0, copy_list_element_0_value);

  Value* copy_list_element_1;
  ASSERT_TRUE(copy_list->Get(1, &copy_list_element_1));
  ASSERT_TRUE(copy_list_element_1);
  ASSERT_NE(copy_list_element_1, original_list_element_1);
  int copy_list_element_1_value;
  ASSERT_TRUE(copy_list_element_1->GetAsInteger(&copy_list_element_1_value));
  ASSERT_EQ(1, copy_list_element_1_value);

  copy_value = nullptr;
  ASSERT_TRUE(copy_dict->Get("dictionary", &copy_value));
  ASSERT_TRUE(copy_value);
  ASSERT_NE(copy_value, original_nested_dictionary);
  ASSERT_TRUE(copy_value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* copy_nested_dictionary = nullptr;
  ASSERT_TRUE(copy_value->GetAsDictionary(&copy_nested_dictionary));
  ASSERT_TRUE(copy_nested_dictionary);
  EXPECT_TRUE(copy_nested_dictionary->HasKey("key"));
}

TEST(ValuesTest, Equals) {
  Value* nullptr1 = Value::CreateNullValue();
  Value* nullptr2 = Value::CreateNullValue();
  EXPECT_NE(nullptr1, nullptr2);
  EXPECT_TRUE(nullptr1->Equals(nullptr2));

  Value* boolean = new FundamentalValue(false);
  EXPECT_FALSE(nullptr1->Equals(boolean));
  delete nullptr1;
  delete nullptr2;
  delete boolean;

  DictionaryValue dv;
  dv.SetBoolean("a", false);
  dv.SetInteger("b", 2);
  dv.SetDouble("c", 2.5);
  dv.SetString("d1", "string");
  dv.SetString("d2", "http://google.com");
  dv.Set("e", Value::CreateNullValue());

  std::unique_ptr<DictionaryValue> copy;
  copy.reset(dv.DeepCopy());
  EXPECT_TRUE(dv.Equals(copy.get()));

  ListValue* list = new ListValue;
  list->Append(Value::CreateNullValue());
  list->Append(new DictionaryValue);
  dv.Set("f", list);

  EXPECT_FALSE(dv.Equals(copy.get()));
  copy->Set("f", list->DeepCopy());
  EXPECT_TRUE(dv.Equals(copy.get()));

  list->Append(new FundamentalValue(true));
  EXPECT_FALSE(dv.Equals(copy.get()));

  // Check if Equals detects differences in only the keys.
  copy.reset(dv.DeepCopy());
  EXPECT_TRUE(dv.Equals(copy.get()));
  copy->Remove("a", nullptr);
  copy->SetBoolean("aa", false);
  EXPECT_FALSE(dv.Equals(copy.get()));
}

TEST(ValuesTest, StaticEquals) {
  std::unique_ptr<Value> nullptr1(Value::CreateNullValue());
  std::unique_ptr<Value> nullptr2(Value::CreateNullValue());
  EXPECT_TRUE(Value::Equals(nullptr1.get(), nullptr2.get()));
  EXPECT_TRUE(Value::Equals(nullptr, nullptr));

  std::unique_ptr<Value> i42(new FundamentalValue(42));
  std::unique_ptr<Value> j42(new FundamentalValue(42));
  std::unique_ptr<Value> i17(new FundamentalValue(17));
  EXPECT_TRUE(Value::Equals(i42.get(), i42.get()));
  EXPECT_TRUE(Value::Equals(j42.get(), i42.get()));
  EXPECT_TRUE(Value::Equals(i42.get(), j42.get()));
  EXPECT_FALSE(Value::Equals(i42.get(), i17.get()));
  EXPECT_FALSE(Value::Equals(i42.get(), nullptr));
  EXPECT_FALSE(Value::Equals(nullptr, i42.get()));

  // nullptr and Value::CreateNullValue() are intentionally different: We need
  // support for nullptr as a return value for "undefined" without caring for
  // ownership of the pointer.
  EXPECT_FALSE(Value::Equals(nullptr1.get(), nullptr));
  EXPECT_FALSE(Value::Equals(nullptr, nullptr1.get()));
}

TEST(ValuesTest, DeepCopyCovariantReturnTypes) {
  DictionaryValue original_dict;
  Value* original_nullptr = Value::CreateNullValue();
  original_dict.Set("nullptr", original_nullptr);
  FundamentalValue* original_bool = new FundamentalValue(true);
  original_dict.Set("bool", original_bool);
  FundamentalValue* original_int = new FundamentalValue(42);
  original_dict.Set("int", original_int);
  FundamentalValue* original_double = new FundamentalValue(3.14);
  original_dict.Set("double", original_double);
  StringValue* original_string = new StringValue("hello");
  original_dict.Set("string", original_string);
  StringValue* original_string16 = new StringValue("hello16");
  original_dict.Set("string16", original_string16);

  std::unique_ptr<char[]> original_buffer(new char[42]);
  memset(original_buffer.get(), '!', 42);
  BinaryValue* original_binary =
      new BinaryValue(std::move(original_buffer), 42);
  original_dict.Set("binary", original_binary);

  ListValue* original_list = new ListValue();
  FundamentalValue* original_list_element_0 = new FundamentalValue(0);
  original_list->Append(original_list_element_0);
  FundamentalValue* original_list_element_1 = new FundamentalValue(1);
  original_list->Append(original_list_element_1);
  original_dict.Set("list", original_list);

  Value* original_dict_value = &original_dict;
  Value* original_bool_value = original_bool;
  Value* original_int_value = original_int;
  Value* original_double_value = original_double;
  Value* original_string_value = original_string;
  Value* original_string16_value = original_string16;
  Value* original_binary_value = original_binary;
  Value* original_list_value = original_list;

  std::unique_ptr<Value> copy_dict_value(original_dict_value->DeepCopy());
  std::unique_ptr<Value> copy_bool_value(original_bool_value->DeepCopy());
  std::unique_ptr<Value> copy_int_value(original_int_value->DeepCopy());
  std::unique_ptr<Value> copy_double_value(original_double_value->DeepCopy());
  std::unique_ptr<Value> copy_string_value(original_string_value->DeepCopy());
  std::unique_ptr<Value> copy_string16_value(
      original_string16_value->DeepCopy());
  std::unique_ptr<Value> copy_binary_value(original_binary_value->DeepCopy());
  std::unique_ptr<Value> copy_list_value(original_list_value->DeepCopy());

  EXPECT_TRUE(original_dict_value->Equals(copy_dict_value.get()));
  EXPECT_TRUE(original_bool_value->Equals(copy_bool_value.get()));
  EXPECT_TRUE(original_int_value->Equals(copy_int_value.get()));
  EXPECT_TRUE(original_double_value->Equals(copy_double_value.get()));
  EXPECT_TRUE(original_string_value->Equals(copy_string_value.get()));
  EXPECT_TRUE(original_string16_value->Equals(copy_string16_value.get()));
  EXPECT_TRUE(original_binary_value->Equals(copy_binary_value.get()));
  EXPECT_TRUE(original_list_value->Equals(copy_list_value.get()));
}

TEST(ValuesTest, RemoveEmptyChildren) {
  std::unique_ptr<DictionaryValue> root(new DictionaryValue);
  // Remove empty lists and dictionaries.
  root->Set("empty_dict", new DictionaryValue);
  root->Set("empty_list", new ListValue);
  root->SetWithoutPathExpansion("a.b.c.d.e", new DictionaryValue);
  root.reset(root->DeepCopyWithoutEmptyChildren());
  EXPECT_TRUE(root->empty());

  // Make sure we don't prune too much.
  root->SetBoolean("bool", true);
  root->Set("empty_dict", new DictionaryValue);
  root->SetString("empty_string", std::string());
  root.reset(root->DeepCopyWithoutEmptyChildren());
  EXPECT_EQ(2U, root->size());

  // Should do nothing.
  root.reset(root->DeepCopyWithoutEmptyChildren());
  EXPECT_EQ(2U, root->size());

  // Nested test cases.  These should all reduce back to the bool and string
  // set above.
  {
    root->Set("a.b.c.d.e", new DictionaryValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }
  {
    DictionaryValue* inner = new DictionaryValue;
    root->Set("dict_with_emtpy_children", inner);
    inner->Set("empty_dict", new DictionaryValue);
    inner->Set("empty_list", new ListValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }
  {
    ListValue* inner = new ListValue;
    root->Set("list_with_empty_children", inner);
    inner->Append(new DictionaryValue);
    inner->Append(new ListValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }

  // Nested with siblings.
  {
    ListValue* inner = new ListValue;
    root->Set("list_with_empty_children", inner);
    inner->Append(new DictionaryValue);
    inner->Append(new ListValue);
    DictionaryValue* inner2 = new DictionaryValue;
    root->Set("dict_with_empty_children", inner2);
    inner2->Set("empty_dict", new DictionaryValue);
    inner2->Set("empty_list", new ListValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }

  // Make sure nested values don't get pruned.
  {
    ListValue* inner = new ListValue;
    root->Set("list_with_empty_children", inner);
    ListValue* inner2 = new ListValue;
    inner->Append(new DictionaryValue);
    inner->Append(inner2);
    inner2->Append(new StringValue("hello"));
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(3U, root->size());
    EXPECT_TRUE(root->GetList("list_with_empty_children", &inner));
    EXPECT_EQ(1U, inner->GetSize());  // Dictionary was pruned.
    EXPECT_TRUE(inner->GetList(0, &inner2));
    EXPECT_EQ(1U, inner2->GetSize());
  }
}

TEST(ValuesTest, MergeDictionary) {
  std::unique_ptr<DictionaryValue> base(new DictionaryValue);
  base->SetString("base_key", "base_key_value_base");
  base->SetString("collide_key", "collide_key_value_base");
  DictionaryValue* base_sub_dict = new DictionaryValue;
  base_sub_dict->SetString("sub_base_key", "sub_base_key_value_base");
  base_sub_dict->SetString("sub_collide_key", "sub_collide_key_value_base");
  base->Set("sub_dict_key", base_sub_dict);

  std::unique_ptr<DictionaryValue> merge(new DictionaryValue);
  merge->SetString("merge_key", "merge_key_value_merge");
  merge->SetString("collide_key", "collide_key_value_merge");
  DictionaryValue* merge_sub_dict = new DictionaryValue;
  merge_sub_dict->SetString("sub_merge_key", "sub_merge_key_value_merge");
  merge_sub_dict->SetString("sub_collide_key", "sub_collide_key_value_merge");
  merge->Set("sub_dict_key", merge_sub_dict);

  base->MergeDictionary(merge.get());

  EXPECT_EQ(4U, base->size());
  std::string base_key_value;
  EXPECT_TRUE(base->GetString("base_key", &base_key_value));
  EXPECT_EQ("base_key_value_base", base_key_value);  // Base value preserved.
  std::string collide_key_value;
  EXPECT_TRUE(base->GetString("collide_key", &collide_key_value));
  EXPECT_EQ("collide_key_value_merge", collide_key_value);  // Replaced.
  std::string merge_key_value;
  EXPECT_TRUE(base->GetString("merge_key", &merge_key_value));
  EXPECT_EQ("merge_key_value_merge", merge_key_value);  // Merged in.

  DictionaryValue* res_sub_dict;
  EXPECT_TRUE(base->GetDictionary("sub_dict_key", &res_sub_dict));
  EXPECT_EQ(3U, res_sub_dict->size());
  std::string sub_base_key_value;
  EXPECT_TRUE(res_sub_dict->GetString("sub_base_key", &sub_base_key_value));
  EXPECT_EQ("sub_base_key_value_base", sub_base_key_value);  // Preserved.
  std::string sub_collide_key_value;
  EXPECT_TRUE(res_sub_dict->GetString("sub_collide_key",
                                      &sub_collide_key_value));
  EXPECT_EQ("sub_collide_key_value_merge", sub_collide_key_value);  // Replaced.
  std::string sub_merge_key_value;
  EXPECT_TRUE(res_sub_dict->GetString("sub_merge_key", &sub_merge_key_value));
  EXPECT_EQ("sub_merge_key_value_merge", sub_merge_key_value);  // Merged in.
}

TEST(ValuesTest, MergeDictionaryDeepCopy) {
  DictionaryValue* child = new DictionaryValue;
  child->SetString("test", "value");
  EXPECT_EQ(1U, child->size());

  std::string value;
  EXPECT_TRUE(child->GetString("test", &value));
  EXPECT_EQ("value", value);

  std::unique_ptr<DictionaryValue> base(new DictionaryValue);
  base->Set("dict", child);
  EXPECT_EQ(1U, base->size());

  DictionaryValue* ptr;
  EXPECT_TRUE(base->GetDictionary("dict", &ptr));
  EXPECT_EQ(child, ptr);

  std::unique_ptr<DictionaryValue> merged(new DictionaryValue);
  merged->MergeDictionary(base.get());
  EXPECT_EQ(1U, merged->size());
  EXPECT_TRUE(merged->GetDictionary("dict", &ptr));
  EXPECT_NE(child, ptr);
  EXPECT_TRUE(ptr->GetString("test", &value));
  EXPECT_EQ("value", value);

  child->SetString("test", "overwrite");
  base.reset();
  EXPECT_TRUE(ptr->GetString("test", &value));
  EXPECT_EQ("value", value);
}

TEST(ValuesTest, DictionaryIterator) {
  DictionaryValue dict;
  for (DictionaryValue::Iterator it(dict); !it.IsAtEnd(); it.Advance()) {
    ADD_FAILURE();
  }

  StringValue value1("value1");
  dict.Set("key1", value1.DeepCopy());
  bool seen1 = false;
  for (DictionaryValue::Iterator it(dict); !it.IsAtEnd(); it.Advance()) {
    EXPECT_FALSE(seen1);
    EXPECT_EQ("key1", it.key());
    EXPECT_TRUE(value1.Equals(&it.value()));
    seen1 = true;
  }
  EXPECT_TRUE(seen1);

  StringValue value2("value2");
  dict.Set("key2", value2.DeepCopy());
  bool seen2 = seen1 = false;
  for (DictionaryValue::Iterator it(dict); !it.IsAtEnd(); it.Advance()) {
    if (it.key() == "key1") {
      EXPECT_FALSE(seen1);
      EXPECT_TRUE(value1.Equals(&it.value()));
      seen1 = true;
    } else if (it.key() == "key2") {
      EXPECT_FALSE(seen2);
      EXPECT_TRUE(value2.Equals(&it.value()));
      seen2 = true;
    } else {
      ADD_FAILURE();
    }
  }
  EXPECT_TRUE(seen1);
  EXPECT_TRUE(seen2);
}

// DictionaryValue/ListValue's Get*() methods should
// accept nullptr as an out-value
// and still return true/false based on success.
TEST(ValuesTest, GetWithnullptrOutValue) {
  DictionaryValue main_dict;
  ListValue main_list;

  FundamentalValue bool_value(false);
  FundamentalValue int_value(1234);
  FundamentalValue double_value(12.34567);
  StringValue string_value("foo");
  BinaryValue binary_value;
  DictionaryValue dict_value;
  ListValue list_value;

  main_dict.Set("bool", bool_value.DeepCopy());
  main_dict.Set("int", int_value.DeepCopy());
  main_dict.Set("double", double_value.DeepCopy());
  main_dict.Set("string", string_value.DeepCopy());
  main_dict.Set("binary", binary_value.DeepCopy());
  main_dict.Set("dict", dict_value.DeepCopy());
  main_dict.Set("list", list_value.DeepCopy());

  main_list.Append(bool_value.DeepCopy());
  main_list.Append(int_value.DeepCopy());
  main_list.Append(double_value.DeepCopy());
  main_list.Append(string_value.DeepCopy());
  main_list.Append(binary_value.DeepCopy());
  main_list.Append(dict_value.DeepCopy());
  main_list.Append(list_value.DeepCopy());

  EXPECT_TRUE(main_dict.Get("bool", nullptr));
  EXPECT_TRUE(main_dict.Get("int", nullptr));
  EXPECT_TRUE(main_dict.Get("double", nullptr));
  EXPECT_TRUE(main_dict.Get("string", nullptr));
  EXPECT_TRUE(main_dict.Get("binary", nullptr));
  EXPECT_TRUE(main_dict.Get("dict", nullptr));
  EXPECT_TRUE(main_dict.Get("list", nullptr));
  EXPECT_FALSE(main_dict.Get("DNE", nullptr));

  EXPECT_TRUE(main_dict.GetBoolean("bool", nullptr));
  EXPECT_FALSE(main_dict.GetBoolean("int", nullptr));
  EXPECT_FALSE(main_dict.GetBoolean("double", nullptr));
  EXPECT_FALSE(main_dict.GetBoolean("string", nullptr));
  EXPECT_FALSE(main_dict.GetBoolean("binary", nullptr));
  EXPECT_FALSE(main_dict.GetBoolean("dict", nullptr));
  EXPECT_FALSE(main_dict.GetBoolean("list", nullptr));
  EXPECT_FALSE(main_dict.GetBoolean("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetInteger("bool", nullptr));
  EXPECT_TRUE(main_dict.GetInteger("int", nullptr));
  EXPECT_FALSE(main_dict.GetInteger("double", nullptr));
  EXPECT_FALSE(main_dict.GetInteger("string", nullptr));
  EXPECT_FALSE(main_dict.GetInteger("binary", nullptr));
  EXPECT_FALSE(main_dict.GetInteger("dict", nullptr));
  EXPECT_FALSE(main_dict.GetInteger("list", nullptr));
  EXPECT_FALSE(main_dict.GetInteger("DNE", nullptr));

  // Both int and double values can be obtained from GetDouble.
  EXPECT_FALSE(main_dict.GetDouble("bool", nullptr));
  EXPECT_TRUE(main_dict.GetDouble("int", nullptr));
  EXPECT_TRUE(main_dict.GetDouble("double", nullptr));
  EXPECT_FALSE(main_dict.GetDouble("string", nullptr));
  EXPECT_FALSE(main_dict.GetDouble("binary", nullptr));
  EXPECT_FALSE(main_dict.GetDouble("dict", nullptr));
  EXPECT_FALSE(main_dict.GetDouble("list", nullptr));
  EXPECT_FALSE(main_dict.GetDouble("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetString("bool",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("int",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("double",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_TRUE(main_dict.GetString("string",
                                  static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("binary",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("dict",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("list",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("DNE",
                                   static_cast<std::string*>(nullptr)));

  EXPECT_FALSE(main_dict.GetString("bool",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("int",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("double",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_TRUE(main_dict.GetString("string",
                                  static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("binary",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("dict",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("list",
                                   static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetString("DNE",
                                   static_cast<std::string*>(nullptr)));

  EXPECT_FALSE(main_dict.GetBinary("bool", nullptr));
  EXPECT_FALSE(main_dict.GetBinary("int", nullptr));
  EXPECT_FALSE(main_dict.GetBinary("double", nullptr));
  EXPECT_FALSE(main_dict.GetBinary("string", nullptr));
  EXPECT_TRUE(main_dict.GetBinary("binary", nullptr));
  EXPECT_FALSE(main_dict.GetBinary("dict", nullptr));
  EXPECT_FALSE(main_dict.GetBinary("list", nullptr));
  EXPECT_FALSE(main_dict.GetBinary("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetDictionary("bool", nullptr));
  EXPECT_FALSE(main_dict.GetDictionary("int", nullptr));
  EXPECT_FALSE(main_dict.GetDictionary("double", nullptr));
  EXPECT_FALSE(main_dict.GetDictionary("string", nullptr));
  EXPECT_FALSE(main_dict.GetDictionary("binary", nullptr));
  EXPECT_TRUE(main_dict.GetDictionary("dict", nullptr));
  EXPECT_FALSE(main_dict.GetDictionary("list", nullptr));
  EXPECT_FALSE(main_dict.GetDictionary("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetList("bool", nullptr));
  EXPECT_FALSE(main_dict.GetList("int", nullptr));
  EXPECT_FALSE(main_dict.GetList("double", nullptr));
  EXPECT_FALSE(main_dict.GetList("string", nullptr));
  EXPECT_FALSE(main_dict.GetList("binary", nullptr));
  EXPECT_FALSE(main_dict.GetList("dict", nullptr));
  EXPECT_TRUE(main_dict.GetList("list", nullptr));
  EXPECT_FALSE(main_dict.GetList("DNE", nullptr));

  EXPECT_TRUE(main_dict.GetWithoutPathExpansion("bool", nullptr));
  EXPECT_TRUE(main_dict.GetWithoutPathExpansion("int", nullptr));
  EXPECT_TRUE(main_dict.GetWithoutPathExpansion("double", nullptr));
  EXPECT_TRUE(main_dict.GetWithoutPathExpansion("string", nullptr));
  EXPECT_TRUE(main_dict.GetWithoutPathExpansion("binary", nullptr));
  EXPECT_TRUE(main_dict.GetWithoutPathExpansion("dict", nullptr));
  EXPECT_TRUE(main_dict.GetWithoutPathExpansion("list", nullptr));
  EXPECT_FALSE(main_dict.GetWithoutPathExpansion("DNE", nullptr));

  EXPECT_TRUE(main_dict.GetBooleanWithoutPathExpansion("bool", nullptr));
  EXPECT_FALSE(main_dict.GetBooleanWithoutPathExpansion("int", nullptr));
  EXPECT_FALSE(main_dict.GetBooleanWithoutPathExpansion("double", nullptr));
  EXPECT_FALSE(main_dict.GetBooleanWithoutPathExpansion("string", nullptr));
  EXPECT_FALSE(main_dict.GetBooleanWithoutPathExpansion("binary", nullptr));
  EXPECT_FALSE(main_dict.GetBooleanWithoutPathExpansion("dict", nullptr));
  EXPECT_FALSE(main_dict.GetBooleanWithoutPathExpansion("list", nullptr));
  EXPECT_FALSE(main_dict.GetBooleanWithoutPathExpansion("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetIntegerWithoutPathExpansion("bool", nullptr));
  EXPECT_TRUE(main_dict.GetIntegerWithoutPathExpansion("int", nullptr));
  EXPECT_FALSE(main_dict.GetIntegerWithoutPathExpansion("double", nullptr));
  EXPECT_FALSE(main_dict.GetIntegerWithoutPathExpansion("string", nullptr));
  EXPECT_FALSE(main_dict.GetIntegerWithoutPathExpansion("binary", nullptr));
  EXPECT_FALSE(main_dict.GetIntegerWithoutPathExpansion("dict", nullptr));
  EXPECT_FALSE(main_dict.GetIntegerWithoutPathExpansion("list", nullptr));
  EXPECT_FALSE(main_dict.GetIntegerWithoutPathExpansion("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetDoubleWithoutPathExpansion("bool", nullptr));
  EXPECT_TRUE(main_dict.GetDoubleWithoutPathExpansion("int", nullptr));
  EXPECT_TRUE(main_dict.GetDoubleWithoutPathExpansion("double", nullptr));
  EXPECT_FALSE(main_dict.GetDoubleWithoutPathExpansion("string", nullptr));
  EXPECT_FALSE(main_dict.GetDoubleWithoutPathExpansion("binary", nullptr));
  EXPECT_FALSE(main_dict.GetDoubleWithoutPathExpansion("dict", nullptr));
  EXPECT_FALSE(main_dict.GetDoubleWithoutPathExpansion("list", nullptr));
  EXPECT_FALSE(main_dict.GetDoubleWithoutPathExpansion("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "bool", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "int", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "double", static_cast<std::string*>(nullptr)));
  EXPECT_TRUE(main_dict.GetStringWithoutPathExpansion(
      "string", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "binary", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "dict", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "list", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "DNE", static_cast<std::string*>(nullptr)));

  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "bool", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "int", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "double", static_cast<std::string*>(nullptr)));
  EXPECT_TRUE(main_dict.GetStringWithoutPathExpansion(
      "string", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "binary", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "dict", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "list", static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_dict.GetStringWithoutPathExpansion(
      "DNE", static_cast<std::string*>(nullptr)));

  // There is no GetBinaryWithoutPathExpansion for some reason, but if there
  // were it should be tested here...

  EXPECT_FALSE(main_dict.GetDictionaryWithoutPathExpansion("bool", nullptr));
  EXPECT_FALSE(main_dict.GetDictionaryWithoutPathExpansion("int", nullptr));
  EXPECT_FALSE(main_dict.GetDictionaryWithoutPathExpansion("double", nullptr));
  EXPECT_FALSE(main_dict.GetDictionaryWithoutPathExpansion("string", nullptr));
  EXPECT_FALSE(main_dict.GetDictionaryWithoutPathExpansion("binary", nullptr));
  EXPECT_TRUE(main_dict.GetDictionaryWithoutPathExpansion("dict", nullptr));
  EXPECT_FALSE(main_dict.GetDictionaryWithoutPathExpansion("list", nullptr));
  EXPECT_FALSE(main_dict.GetDictionaryWithoutPathExpansion("DNE", nullptr));

  EXPECT_FALSE(main_dict.GetListWithoutPathExpansion("bool", nullptr));
  EXPECT_FALSE(main_dict.GetListWithoutPathExpansion("int", nullptr));
  EXPECT_FALSE(main_dict.GetListWithoutPathExpansion("double", nullptr));
  EXPECT_FALSE(main_dict.GetListWithoutPathExpansion("string", nullptr));
  EXPECT_FALSE(main_dict.GetListWithoutPathExpansion("binary", nullptr));
  EXPECT_FALSE(main_dict.GetListWithoutPathExpansion("dict", nullptr));
  EXPECT_TRUE(main_dict.GetListWithoutPathExpansion("list", nullptr));
  EXPECT_FALSE(main_dict.GetListWithoutPathExpansion("DNE", nullptr));

  EXPECT_TRUE(main_list.Get(0, nullptr));
  EXPECT_TRUE(main_list.Get(1, nullptr));
  EXPECT_TRUE(main_list.Get(2, nullptr));
  EXPECT_TRUE(main_list.Get(3, nullptr));
  EXPECT_TRUE(main_list.Get(4, nullptr));
  EXPECT_TRUE(main_list.Get(5, nullptr));
  EXPECT_TRUE(main_list.Get(6, nullptr));
  EXPECT_FALSE(main_list.Get(7, nullptr));

  EXPECT_TRUE(main_list.GetBoolean(0, nullptr));
  EXPECT_FALSE(main_list.GetBoolean(1, nullptr));
  EXPECT_FALSE(main_list.GetBoolean(2, nullptr));
  EXPECT_FALSE(main_list.GetBoolean(3, nullptr));
  EXPECT_FALSE(main_list.GetBoolean(4, nullptr));
  EXPECT_FALSE(main_list.GetBoolean(5, nullptr));
  EXPECT_FALSE(main_list.GetBoolean(6, nullptr));
  EXPECT_FALSE(main_list.GetBoolean(7, nullptr));

  EXPECT_FALSE(main_list.GetInteger(0, nullptr));
  EXPECT_TRUE(main_list.GetInteger(1, nullptr));
  EXPECT_FALSE(main_list.GetInteger(2, nullptr));
  EXPECT_FALSE(main_list.GetInteger(3, nullptr));
  EXPECT_FALSE(main_list.GetInteger(4, nullptr));
  EXPECT_FALSE(main_list.GetInteger(5, nullptr));
  EXPECT_FALSE(main_list.GetInteger(6, nullptr));
  EXPECT_FALSE(main_list.GetInteger(7, nullptr));

  EXPECT_FALSE(main_list.GetDouble(0, nullptr));
  EXPECT_TRUE(main_list.GetDouble(1, nullptr));
  EXPECT_TRUE(main_list.GetDouble(2, nullptr));
  EXPECT_FALSE(main_list.GetDouble(3, nullptr));
  EXPECT_FALSE(main_list.GetDouble(4, nullptr));
  EXPECT_FALSE(main_list.GetDouble(5, nullptr));
  EXPECT_FALSE(main_list.GetDouble(6, nullptr));
  EXPECT_FALSE(main_list.GetDouble(7, nullptr));

  EXPECT_FALSE(main_list.GetString(0, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(1, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(2, static_cast<std::string*>(nullptr)));
  EXPECT_TRUE(main_list.GetString(3, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(4, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(5, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(6, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(7, static_cast<std::string*>(nullptr)));

  EXPECT_FALSE(main_list.GetString(0, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(1, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(2, static_cast<std::string*>(nullptr)));
  EXPECT_TRUE(main_list.GetString(3, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(4, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(5, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(6, static_cast<std::string*>(nullptr)));
  EXPECT_FALSE(main_list.GetString(7, static_cast<std::string*>(nullptr)));

  EXPECT_FALSE(main_list.GetBinary(0, nullptr));
  EXPECT_FALSE(main_list.GetBinary(1, nullptr));
  EXPECT_FALSE(main_list.GetBinary(2, nullptr));
  EXPECT_FALSE(main_list.GetBinary(3, nullptr));
  EXPECT_TRUE(main_list.GetBinary(4, nullptr));
  EXPECT_FALSE(main_list.GetBinary(5, nullptr));
  EXPECT_FALSE(main_list.GetBinary(6, nullptr));
  EXPECT_FALSE(main_list.GetBinary(7, nullptr));

  EXPECT_FALSE(main_list.GetDictionary(0, nullptr));
  EXPECT_FALSE(main_list.GetDictionary(1, nullptr));
  EXPECT_FALSE(main_list.GetDictionary(2, nullptr));
  EXPECT_FALSE(main_list.GetDictionary(3, nullptr));
  EXPECT_FALSE(main_list.GetDictionary(4, nullptr));
  EXPECT_TRUE(main_list.GetDictionary(5, nullptr));
  EXPECT_FALSE(main_list.GetDictionary(6, nullptr));
  EXPECT_FALSE(main_list.GetDictionary(7, nullptr));

  EXPECT_FALSE(main_list.GetList(0, nullptr));
  EXPECT_FALSE(main_list.GetList(1, nullptr));
  EXPECT_FALSE(main_list.GetList(2, nullptr));
  EXPECT_FALSE(main_list.GetList(3, nullptr));
  EXPECT_FALSE(main_list.GetList(4, nullptr));
  EXPECT_FALSE(main_list.GetList(5, nullptr));
  EXPECT_TRUE(main_list.GetList(6, nullptr));
  EXPECT_FALSE(main_list.GetList(7, nullptr));
}

}  // namespace utils
}  // namespace parser
