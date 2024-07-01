//
// Created by MichaelBrunner on 28/05/2024.
//

/////// PSEUDO IMPLEMENTATION OF RAPTOR ALGORITHM ///////


#include <vector>
#include <limits>
#include <optional>
#include <queue>
#include <tuple>
#include <algorithm>
#include <limits>
#include <vector>
#include <string>

struct Footpath;
struct Route;

// Define the Stop structure
struct Stop
{
  int id;                     // Unique identifier for the stop
  std::vector<Route*> routes; // List of routes that serve this stop

  std::vector<Footpath> footpaths; // List of footpaths from this stop

  // Function to get the routes that serve this stop
  [[nodiscard]] std::vector<Route*> served_routes() const {
    return routes;
  }

  [[nodiscard]] std::vector<Footpath> footpaths_from() const {
    return footpaths;
  }

  static int count; // Total number of stops
};

// Define the Route structure
struct Route
{
  int id;                   // Unique identifier for the route
  std::vector<Stop*> stops; // List of stops that this route serves

  // Function to get the stops served by this route from a given stop
  std::vector<Stop*> stops_from(Stop* stop) {
    auto it = std::ranges::find(stops, stop);
    if (it != stops.end())
    {
      return std::vector<Stop*>(it, stops.end());
    }
    else
    {
      return std::vector<Stop*>();
    }
  }
};

// Define the Trip structure
struct Trip
{
  int id;                           // Unique identifier for the trip
  Route* route;                     // The route that this trip follows
  std::vector<int> arrival_times;   // Arrival times at each stop
  std::vector<int> departure_times; // Departure times from each stop

  // Assuming arr and dep are defined elsewhere
};

// Define the Footpath structure
struct Footpath
{
  Stop* from;   // The stop from which the footpath starts
  Stop* to;     // The stop at which the footpath ends
  int distance; // The distance of the footpath
};


constexpr auto INFINITY_VALUE = std::numeric_limits<int>::max();

// Define the arr function
int arr(const Trip& trip, const Stop& stop) {
  // Assuming the stops in a trip and the arrival times have the same order
  auto it = std::find(trip.route->stops.begin(), trip.route->stops.end(), &stop);
  if (it != trip.route->stops.end()) {
    return trip.arrival_times[it - trip.route->stops.begin()];
  } else {
    return INFINITY_VALUE;
  }
}

// Define the dep function
int dep(const Trip& trip, const Stop& stop) {
  // Assuming the stops in a trip and the departure times have the same order
  auto it = std::find(trip.route->stops.begin(), trip.route->stops.end(), &stop);
  if (it != trip.route->stops.end()) {
    return trip.departure_times[it - trip.route->stops.begin()];
  } else {
    return INFINITY_VALUE;
  }
}

// Define the et function
Trip et(const Route& route, const Stop& stop) {
  // This function should return the earliest trip that can be taken from the stop on the route
  // The implementation of this function depends on how the trips are stored and accessed
  // For the sake of this example, let's assume we have a function get_trips that returns all trips for a route
  std::vector<Trip> trips = get_trips(route);
  // Sort the trips by departure time from the stop
  std::sort(trips.begin(), trips.end(), [&stop](const Trip& a, const Trip& b) {
    return dep(a, stop) < dep(b, stop);
  });
  // Return the first trip
  return trips.front();
}

// Define the contains function for std::queue
template <typename T1, typename T2>
bool contains(const std::queue<std::tuple<T1, T2>>& q, const T1& value) {
  // This function checks if a queue of tuples contains a tuple with a specific first value
  for (size_t i = 0; i < q.size(); ++i) {
    if (std::get<0>(q.front()) == value) {
      return true;
    }
    q.push(q.front());
    q.pop();
  }
  return false;
}

// Define the replace function for std::queue
template <typename T1, typename T2>
void replace(std::queue<std::tuple<T1, T2>>& q, const T1& value, const T2& new_second_value) {
  // This function replaces the second value of the first tuple in a queue that has a specific first value
  for (size_t i = 0; i < q.size(); ++i) {
    if (std::get<0>(q.front()) == value) {
      q.front() = std::make_tuple(value, new_second_value);
      return;
    }
    q.push(q.front());
    q.pop();
  }
}


// Assuming Stop, Route, Trip, and Footpath are defined elsewhere
// Assuming functions arr(t, p), dep(t, p), et(r, p), and `(p, p0) are defined elsewhere

struct Label
{
  int time;
  Stop parent;
  // Additional fields as necessary
};

std::vector<Label> RAPTOR(const Stop& source, const Stop& target, int departure_time) {
  std::vector<int> arrival_times; // τ
  std::vector<int> best_times;    // τ*
  std::vector<std::tuple<Route, Stop>> Q;
  std::vector<bool> marked;
  // Initialization
  arrival_times.assign(Stop::count, INFINITY);
  best_times.assign(Stop::count, INFINITY);
  arrival_times[source.id] = departure_time;
  marked[source.id] = true;

  for (int k = 1; !Q.empty(); ++k)
  {
    // Accumulate routes serving marked stops from previous round

    Q.clear();
    for (const auto& p : marked)
    {
      for (const auto& r : p.served_routes())
      {
        if (std::ranges::find(Q, r) == Q.end())
        {
          continue;
        }
        auto [route_in_Q, first_stop] = Q.contains(r);
        if (route_in_Q)
        {
          if (p < first_stop)
          {
            Q.replace(r, p);
          }
        }
        else
        {
          Q.emplace(r, p);
        }
        marked[p.id] = false;
      }
    }

    // Traverse each route
    for (const auto& [r, p] : Q)
    {
      std::optional<Trip> current_trip;
      for (const auto& pi : r.stops_from(p))
      {
        // Can the label be improved in this round? Includes local and target pruning
        if (current_trip.has_value() && arr(*current_trip, pi) < std::min(best_times[pi.id], best_times[target.id]))
        {
          arrival_times[pi.id] = arr(*current_trip, pi);
          best_times[pi.id] = arr(*current_trip, pi);
          marked[pi.id] = true;
        }
        // Can we catch an earlier trip at pi?
        if (arrival_times[pi.id] <= dep(*current_trip, pi))
        {
          current_trip = et(r, pi);
        }
      }
    }

    // Look at foot-paths
    for (const auto& p : marked)
    {
      for (const auto& [p0, distance] : p.footpaths())
      {
        [^6 ^ ][6] int new_time = arrival_times[p.id] + distance;
        if (new_time < arrival_times[p0.id])
        {
          arrival_times[p0.id] = new_time;
          marked[p0.id] = true;
        }
      }
    }

    // Stopping criterion
    if (std::ranges::none_of(marked, [](bool m) { return m; }))
    {
      break;
    }
  }

  // Reconstruct the journey
  std::vector<Label> journey;
  // ... (omitted for brevity)
  return journey;
}

