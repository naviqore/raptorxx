target_sources(${PROJECT_NAME}
        PRIVATE
        GtfsCsvParserReaderStrategyFactory.cpp
        GtfsCsvParserReaderStrategyFactory.h
        GtfsCsvReaderStrategyFactory.cpp
        GtfsCsvReaderStrategyFactory.h
        GtfsReader.cpp
        GtfsReaderStrategyFactory.cpp
        GtfsTxtReaderStrategyFactory.cpp
        GtfsTxtReaderStrategyFactory.h
        GtfsData.cpp
)

add_subdirectory(strategies)