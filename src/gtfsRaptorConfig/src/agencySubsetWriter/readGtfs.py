import os
import csv
from typing import List, Dict
from dataclasses import dataclass, field


@dataclass(frozen=True)
class Stop:
    stop_id: str
    stop_name: str
    stop_lat: float
    stop_lon: float
    location_type: int
    parent_station: str

    def __hash__(self):
        return hash(self.stop_id)

    def __eq__(self, other):
        if isinstance(other, Stop):
            return self.stop_id == other.stop_id
        return False


@dataclass(frozen=True)
class StopTime:
    trip_id: str
    arrival_time: str
    departure_time: str
    stop_id: str
    stop_sequence: int
    pickup_type: int
    drop_off_type: int


@dataclass(frozen=True)
class Trip:
    route_id: str
    service_id: str
    trip_id: str
    trip_headsign: str
    trip_short_name: str
    direction_id: int
    block_id: str


@dataclass
class Route:
    route_id: str
    agency_id: str
    route_short_name: str
    route_long_name: str
    route_desc: str
    route_type: int
    trips: List[Trip] = field(default_factory=list)


@dataclass(frozen=True)
class Calendar:
    service_id: str
    monday: int
    tuesday: int
    wednesday: int
    thursday: int
    friday: int
    saturday: int
    sunday: int
    start_date: str
    end_date: str


@dataclass(frozen=True)
class CalendarDate:
    service_id: str
    date: str
    exception_type: int


@dataclass(frozen=True)
class Transfer:
    from_stop_id: str
    to_stop_id: str
    transfer_type: int
    min_transfer_time: int

    def __hash__(self):
        return hash(self.from_stop_id + self.to_stop_id)

    def __eq__(self, other):
        if isinstance(other, Transfer):
            return self.from_stop_id == other.from_stop_id and self.to_stop_id == other.to_stop_id
        return False


def load_csv(filename: str, data_class, key_field=None):
    data_list = []
    data_dict = {}

    with open(filename, mode='r', encoding='utf-8-sig') as file:
        reader = csv.DictReader(file)
        for row in reader:
            # Strip any hidden characters from the keys
            row = {key.strip(): value for key, value in row.items()}
            data_obj = data_class(**row)
            data_list.append(data_obj)
            if key_field:
                data_dict[row[key_field]] = data_obj

    return data_list, data_dict


def load_gtfs_data(directory: str):
    gtfs_data_dict = {
        'routes': [],
        'trips': [],
        'stop_times': [],
        'stops': [],
        'transfers': [],
        'calendars': [],
        'calendar_dates': []
    }

    # Read calendar.txt
    calendar_file = os.path.join(directory, 'calendar.txt')
    if os.path.exists(calendar_file):
        gtfs_data_dict['calendars'], _ = load_csv(
            calendar_file,
            Calendar,
            key_field='service_id'
        )

    # Read calendar_dates.txt
    calendar_dates_file = os.path.join(directory, 'calendar_dates.txt')
    if os.path.exists(calendar_dates_file):
        gtfs_data_dict['calendar_dates'], _ = load_csv(
            calendar_dates_file,
            CalendarDate,
            key_field=None
        )

    # Read routes.txt
    routes_file = os.path.join(directory, 'routes.txt')
    if os.path.exists(routes_file):
        gtfs_data_dict['routes'], _ = load_csv(
            routes_file,
            Route,
            key_field='route_id'
        )

    # Read stops.txt
    stops_file = os.path.join(directory, 'stops.txt')
    if os.path.exists(stops_file):
        gtfs_data_dict['stops'], _ = load_csv(
            stops_file,
            Stop,
            key_field='stop_id'
        )

    # Read stop_times.txt
    stop_times_file = os.path.join(directory, 'stop_times.txt')
    if os.path.exists(stop_times_file):
        gtfs_data_dict['stop_times'], _ = load_csv(
            stop_times_file,
            StopTime,
            key_field=None
        )

    # Read transfers.txt
    transfers_file = os.path.join(directory, 'transfers.txt')
    if os.path.exists(transfers_file):
        gtfs_data_dict['transfers'], _ = load_csv(
            transfers_file,
            Transfer,
            key_field=None
        )

    # Read trips.txt
    trips_file = os.path.join(directory, 'trips.txt')
    if os.path.exists(trips_file):
        gtfs_data_dict['trips'], _ = load_csv(
            trips_file,
            Trip,
            key_field='trip_id'
        )

    for trip in gtfs_data_dict['trips']:
        filtered_routes = [route for route in gtfs_data_dict['routes'] if route.route_id == trip.route_id]
        for route in filtered_routes:
            route.trips.append(trip)

    return gtfs_data_dict

# if __name__ == '__main__':
#     gtfs_data = load_gtfs_data('C:\\Users\\MichaelBrunner\\source\\master-thesis\\raptorxx\\test\\test_gtfsRaptorConfig\\test-data\\Verkehrsbetriebe_der_Stadt_St_Gallen')
#     print(gtfs_data)
