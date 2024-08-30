target_sources(${PROJECT_NAME}
        PUBLIC
        DataReader.h
        GtfsData.h
        GtfsReader.h
        GtfsReaderStrategyFactory.h
        IGtfsReaderStrategyFactory.h
        ServiceDayTime.h
)

add_subdirectory(model)