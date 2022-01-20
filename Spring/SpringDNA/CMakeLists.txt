set(SRCS
	"src/buildDNA.cpp"
)

add_executable(SpringDNA ${SRCS})

target_include_directories(SpringDNA
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_SOURCE_DIR}/dna.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dna_type_offsets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dna_verify.c
  COMMAND
    "$<TARGET_FILE:SpringDNA>" ${CMAKE_CURRENT_BINARY_DIR}/dna.c ${CMAKE_CURRENT_BINARY_DIR}/dna_type_offsets.h ${CMAKE_CURRENT_BINARY_DIR}/dna_verify.c ${CMAKE_SOURCE_DIR}/
  DEPENDS SpringDNA
)


#add_custom_target(
#    some_target ALL
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dna.c
#)