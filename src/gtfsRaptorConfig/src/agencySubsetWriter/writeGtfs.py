import csv
import os
from concurrent.futures import ThreadPoolExecutor
from typing import List, Set, Dict
from readGtfs import load_gtfs_data, Trip, Stop, Transfer
from src.gtfsRaptorConfig.src.agencySubsetWriter.readGtfs import StopTime

import logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)


def quote(value):
    return f'"{value}"'  # if value else '""'


def write_gtfs_files(output_directory: str, agency_id: str, gtfs_data: Dict):
    routes_to_write = [route for route in gtfs_data['routes'] if route.agency_id == agency_id]
    logger.info(f"Found {len(routes_to_write)} routes for agency {agency_id}")

    trips: List[Trip] = [trip for route in routes_to_write for trip in route.trips]
    logger.info(f"Found {len(trips)} trips for agency {agency_id}")

    stop_times: List[StopTime] = [stop_time for trip in trips for stop_time in gtfs_data['stop_times'] if
                                  stop_time.trip_id == trip.trip_id]
    logger.info(f"Found {len(stop_times)} stop times for agency {agency_id}")

    stop_ids: Set[str] = {stop_time.stop_id for stop_time in stop_times}
    logger.info(f"Found {len(stop_ids)} stops for agency {agency_id}")

    stops: Set[Stop] = {stop for stop in gtfs_data['stops'] if stop.stop_id in stop_ids}
    logger.info(f"Found {len(stops)} stops for agency {agency_id}")

    stops_from_parent_station = {stop for stop in gtfs_data['stops'] for stop_item in stops if
                                 stop_item.parent_station == stop.parent_station}

    # update SET stops
    stops |= stops_from_parent_station

    transfers: Set[Transfer] = {transfer for stop in stops for transfer in gtfs_data['transfers'] if
                                transfer.from_stop_id == stop.stop_id}
    logger.info(f"Found {len(transfers)} transfers for agency {agency_id}")

    # calendars = [calendar for trip in trips for calendar in gtfs_data['calendars'] if
    #              calendar.service_id == trip.service_id]
    # logger.info(f"Found {len(calendars)} calendars for agency {agency_id}")
    #
    # calendar_dates = [calendar_date for calendar in calendars for calendar_date in gtfs_data['calendar_dates'] if
    #                   calendar_date.service_id == calendar.service_id]
    # logger.info(f"Found {len(calendar_dates)} calendar dates for agency {agency_id}")

    os.makedirs(output_directory, exist_ok=True)

    #
    def write_csv(file_name, header, rows):
        with open(os.path.join(output_directory, file_name), mode='w', newline='', encoding='utf-8') as file:
            writer = csv.writer(file, quoting=csv.QUOTE_ALL)
            writer.writerow(header)
            writer.writerows(rows)

    tasks = [
        ("routes.txt", ["route_id", "agency_id", "route_short_name", "route_long_name", "route_desc", "route_type"],
         [[route.route_id, route.agency_id, route.route_short_name, route.route_long_name, route.route_desc,
           route.route_type] for route in routes_to_write]),

        ("trips.txt",
         ["route_id", "service_id", "trip_id", "trip_headsign", "trip_short_name", "direction_id", "block_id"],
         [[trip.route_id, trip.service_id, trip.trip_id, trip.trip_headsign, trip.trip_short_name, trip.direction_id,
           trip.block_id] for trip in trips]),

        ("stop_times.txt",
         ["trip_id", "arrival_time", "departure_time", "stop_id", "stop_sequence", "pickup_type", "drop_off_type"],
         [[st.trip_id, st.arrival_time, st.departure_time, st.stop_id, st.stop_sequence, st.pickup_type,
           st.drop_off_type] for st in stop_times]),

        ("stops.txt", ["stop_id", "stop_name", "stop_lat", "stop_lon", "location_type", "parent_station"],
         [[stop.stop_id, stop.stop_name, stop.stop_lat, stop.stop_lon, stop.location_type, stop.parent_station] for stop
          in stops]),

        ("transfers.txt", ["from_stop_id", "to_stop_id", "transfer_type", "min_transfer_time"],
         [[transfer.from_stop_id, transfer.to_stop_id, transfer.transfer_type, transfer.min_transfer_time] for transfer
          in transfers]),

        # ("calendar.txt",
        #  ["service_id", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday", "start_date",
        #   "end_date"],
        #  [[cal.service_id, cal.monday, cal.tuesday, cal.wednesday, cal.thursday, cal.friday, cal.saturday, cal.sunday,
        #    cal.start_date, cal.end_date] for cal in calendars]),
        #
        # ("calendar_dates.txt", ["service_id", "date", "exception_type"],
        #  [[cd.service_id, cd.date, cd.exception_type] for cd in calendar_dates])
    ]

    logger.info(f"Writing GTFS files to {output_directory}")
    with ThreadPoolExecutor() as executor:
        futures = [executor.submit(write_csv, file_name, header, rows) for file_name, header, rows in tasks]

        for future in futures:
            future.result()


if __name__ == '__main__':
    directory = 'C:\\Users\\MichaelBrunner\\Downloads\\gtfs_fp2024_2024-05-27'
    gtfs_data = load_gtfs_data(directory)

    output_directory = 'C:\\Users\\MichaelBrunner\\Downloads\\filtered_gtfs'
    agency_id = "885"  # "Verkehrsbetriebe der Stadt St.Gallen"
    write_gtfs_files(output_directory, agency_id, gtfs_data)

    # stop_times = list(filter(lambda stop_time_item: stop_time_item.trip_id in [trip.trip_id for trip in trips], gtfs_data['stop_times']))
    # stops_set = set(filter(lambda stop_item: stop_item.stop_id in [stop_time.stop_id for stop_time in stop_times], gtfs_data['stops']))
    # transfers_set = set(filter(lambda transfer_item: transfer_item.from_stop_id in [stop.stop_id for stop in stops_set], gtfs_data['transfers']))
    # calendars = list(filter(lambda calendar_item: calendar_item.service_id in [trip.service_id for trip in trips], gtfs_data['calendars']))
    # calendar_dates = list(filter(lambda calendar_date_item: calendar_date_item.service_id in [calendar.service_id for calendar in calendars], gtfs_data['calendar_dates']))
    #
    #
    #
    # stop_times_dict = {st.trip_id: [] for st in gtfs_data['stop_times']}
    # for st in gtfs_data['stop_times']:
    #     stop_times_dict[st.trip_id].append(st)
    # stops_dict = {stop.stop_id: stop for stop in gtfs_data['stops']}
    # transfers_dict = {stop.stop_id: [] for stop in gtfs_data['stops']}
    # for transfer in gtfs_data['transfers']:
    #     transfers_dict[transfer.from_stop_id].append(transfer)
    # calendars_dict = {cal.service_id: cal for cal in gtfs_data['calendars']}
    # calendar_dates_dict = {cal.service_id: [] for cal in gtfs_data['calendars']}
    # for cd in gtfs_data['calendar_dates']:
    #     calendar_dates_dict[cd.service_id].append(cd)
    #
    # for route in routes_to_write:
    #     logger.info(f"Processing route {route.route_id}")
    #     for trip in gtfs_data['trips']:
    #         if trip.route_id == route.route_id:
    #             trips.append(trip)
    #             stop_times.extend(stop_times_dict[trip.trip_id])
    #
    #             for stop_time in stop_times_dict[trip.trip_id]:
    #                 stop = stops_dict.get(stop_time.stop_id)
    #                 if stop:
    #                     stops_set.add(stop)
    #
    #             for stop in stops_set:
    #                 transfers_set.update(transfers_dict[stop.stop_id])
    #
    #             calendar = calendars_dict.get(trip.service_id)
    #             if calendar:
    #                 calendars.append(calendar)
    #                 calendar_dates.extend(calendar_dates_dict[trip.service_id])
    #
    # os.makedirs(output_directory, exist_ok=True)
