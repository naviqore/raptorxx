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
)

add_subdirectory(strategies)