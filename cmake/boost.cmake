#set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS thread chrono regex context)

include_directories( ${Boost_INCLUDE_DIR} )

set(BOOST_LIBRARIES Boost::headers Boost::regex Boost::thread Boost::context Boost::chrono)
