//
// Created by MichaelBrunner on 15/09/2024.
//

#include <gtest/gtest.h>
#include "LocalDateTime.h"

using namespace raptor::utils;
using namespace std::chrono;

TEST(LocalDateTimeTests, ConvertsToYearMonthDayCorrectly) {
  const LocalDateTime dateTime{year{2024}, month{10}, day{5}, hours{12}, minutes{30}, seconds{45}};
  const auto ymd = dateTime.toYearMonthDay();
  EXPECT_EQ(ymd.year(), year{2024});
  EXPECT_EQ(ymd.month(), month{10});
  EXPECT_EQ(ymd.day(), day{5});
}

TEST(LocalDateTimeTests, HandlesEpochTimeCorrectly) {
  const LocalDateTime dateTime{year{1970}, month{1}, day{1}, hours{0}, minutes{0}, seconds{0}};
  const auto ymd = dateTime.toYearMonthDay();
  EXPECT_EQ(ymd.year(), year{1970});
  EXPECT_EQ(ymd.month(), month{1});
  EXPECT_EQ(ymd.day(), day{1});
}

TEST(LocalDateTimeTests, HandlesLeapYearCorrectly) {
  const LocalDateTime dateTime{year{2020}, month{2}, day{29}, hours{0}, minutes{0}, seconds{0}};
  const auto ymd = dateTime.toYearMonthDay();
  EXPECT_EQ(ymd.year(), year{2020});
  EXPECT_EQ(ymd.month(), month{2});
  EXPECT_EQ(ymd.day(), day{29});
}

TEST(LocalDateTimeTests, HandlesEndOfYearCorrectly) {
  const LocalDateTime dateTime{year{2024}, month{12}, day{31}, hours{23}, minutes{59}, seconds{59}};
  const auto ymd = dateTime.toYearMonthDay();
  EXPECT_EQ(ymd.year(), year{2024});
  EXPECT_EQ(ymd.month(), month{12});
  EXPECT_EQ(ymd.day(), day{31});
}

TEST(LocalDateTimeTests, HandlesStartOfYearCorrectly) {
  const LocalDateTime dateTime{year{2024}, month{1}, day{1}, hours{0}, minutes{0}, seconds{0}};
  const auto ymd = dateTime.toYearMonthDay();
  EXPECT_EQ(ymd.year(), year{2024});
  EXPECT_EQ(ymd.month(), month{1});
  EXPECT_EQ(ymd.day(), day{1});
}
