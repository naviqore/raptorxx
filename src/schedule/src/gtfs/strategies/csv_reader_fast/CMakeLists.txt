target_sources(${PROJECT_NAME} PRIVATE
        csv.h
        csv_wrapper.h
        GtfsAgencyReaderCsvParser.cpp
        GtfsAgencyReaderCsvParser.h
        GtfsCalendarDateReaderCsvParser.cpp
        GtfsCalendarDateReaderCsvParser.h
        GtfsCalendarReaderCsvParser.cpp
        GtfsCalendarReaderCsvParser.h
        GtfsRouteReaderCsvParser.cpp
        GtfsRouteReaderCsvParser.h
        GtfsStopReaderCsvParser.cpp
        GtfsStopReaderCsvParser.h
        GtfsStopTimeReaderCsvParser.cpp
        GtfsStopTimeReaderCsvParser.h
        GtfsTransferReaderCsvParser.cpp
        GtfsTransferReaderCsvParser.h
        GtfsTripReaderCsvParser.cpp
        GtfsTripReaderCsvParser.h
)