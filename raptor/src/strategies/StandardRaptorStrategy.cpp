// //
// // Created by MichaelBrunner on 20/07/2024.
// //
//
// #include "StandardRaptorStrategy.h"
//
// namespace raptor::strategy {
//
//   StandardRaptorStrategy::StandardRaptorStrategy(schedule::gtfs::TimetableManager&& relationManager)
//     : timeTable(std::move(relationManager)) {}
//
//   std::shared_ptr<IConnection> StandardRaptorStrategy::execute(const std::string& sourceStop, const std::string& targetStop, unsigned departureTime) {
//     return {};
//   }
// }