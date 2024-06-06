//
// Created by MichaelBrunner on 06/06/2024.
//


#include <gtest/gtest.h>
#include <source_location>
#include <iostream>

class Foo
{
public:
  void init() {
    std::cout << "Foo::init()" << std::endl;
    std::cout << std::source_location::current().file_name() << std::endl;
    std::cout << std::source_location::current().line() << std::endl;
    std::cout << std::source_location::current().function_name() << std::endl;
  }

  ~Foo() {
    std::cout << "Foo::~Foo()" << std::endl;
  }

  int Bar(const std::string& input_filepath, const std::string& output_filepath) {
    std::ignore = input_filepath;
    std::ignore = output_filepath;
    std::cout << std::source_location::current().function_name() << std::endl;
    return 0;
  }

  int Baz(const std::string& input_filepath, const std::string& output_filepath) {
    std::ignore = input_filepath;
    std::ignore = output_filepath;
    std::cout << std::source_location::current().function_name() << std::endl;
    return 0;
  }

};

namespace my::project {
  namespace {
    // The fixture for testing class Foo.
    class FooTest : public testing::Test
    {
    protected:
      // You can remove any or all of the following functions if their bodies would
      // be empty.

      FooTest() {
        // You can do set-up work for each test here.
      }

      ~FooTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
      }

      // If the constructor and destructor are not enough for setting up
      // and cleaning up each test, you can define the following methods:

      void SetUp() override {
        // Code here will be called immediately after the constructor (right
        // before each test)
        foo.init();
      }

      void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
      }

      // Class members declared here can be used by all tests in the test suite
      // for Foo.
      Foo foo;
    };
    // a TEST_FIXTURE is a class that includes a group of setup condtions and cleanup actions that are common to multiple tests
    TEST_F(FooTest, MethodBarDoesAbc) {
      const std::string input_filepath = "this/package/testdata/myinputfile.dat";
      const std::string output_filepath = "this/package/testdata/myoutputfile.dat";
      Foo f;
      EXPECT_EQ(f.Bar(input_filepath, output_filepath), 0);
    }

    class FooTestP : public ::testing::TestWithParam<std::function<int(const std::string&, const std::string&)>> {
    protected:
      Foo foo;
    };

    TEST_P(FooTestP, BarReturnsZeroForDifferentFilepaths) {
      auto func = GetParam();
      EXPECT_EQ(func("this/package/testdata/myinputfile.dat", "this/package/testdata/myoutputfile.dat"), 0);
    }

    INSTANTIATE_TEST_SUITE_P(
        FooTestInstantiation,
        FooTestP,
        ::testing::Values(
            [](const std::string& input_filepath, const std::string& output_filepath) {
                Foo foo;
                return foo.Bar(input_filepath, output_filepath);
            },
            [](const std::string& input_filepath, const std::string& output_filepath) {
                Foo foo;
                return foo.Baz(input_filepath, output_filepath);
            },
            [](const std::string& input_filepath, const std::string& output_filepath) {
                Foo foo;
                return foo.Bar(input_filepath, output_filepath);
            }
        )
    );


  }
}


