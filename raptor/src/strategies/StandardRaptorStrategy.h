// //
// // Created by MichaelBrunner on 20/07/2024.
// //
//
//
// #pragma once
//
// #include "IConnection.h"
// #include "IRaptorAlgorithmStrategy.h"
// #include "TimetableManager.h"
//
// namespace raptor::strategy {
//
//   class StandardRaptorStrategy final : public IRaptorAlgorithmStrategy
//   {
//
//     schedule::gtfs::TimetableManager timeTable;
//     static constexpr size_t MAX_ROUNDS = 5;
//     std::string sourceStop;
//     std::string targetStop;
//     int startTime{};
//
//   public:
//     explicit StandardRaptorStrategy(schedule::gtfs::TimetableManager&& relationManager);
//
//     std::shared_ptr<IConnection> execute(const std::string& sourceStop, const std::string& targetStop, unsigned departureTime) override;
//   };
//
// } // strategy
// // raptor
