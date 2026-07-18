#1. Indicate cross-comilation
set(CMAKE_SYSTEM_NAME   Generic)

#2. Indicate processor archetechture
set(CMAKE_SYSTEM_PROCESSOR arm)

#3. Try-complile for static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#4. Locate Arm GNU toolchain utilities and set varibles
set(ARM_PREFIX arm-none-eabi- )

find_program(CMAKE_C_COMPILER      NAMES ${ARM_PREFIX}gcc REQUIRED)
find_program(CMAKE_ASM_COMPILER    NAMES ${ARM_PREFIX}gcc REQUIRED)
find_program(CMAKE_CXX_COMPILER    NAMES ${ARM_PREFIX}g++ REQUIRED)
find_program(CMAKE_AR              NAMES ${ARM_PREFIX}ar  REQUIRED)
find_program(CMAKE_OBJCOPY         NAMES ${ARM_PREFIX}objcopy  REQUIRED)
find_program(CMAKE_OBJDUMP         NAMES ${ARM_PREFIX}objdump  REQUIRED)
find_program(CMAKE_SIZE            NAMES ${ARM_PREFIX}size  REQUIRED)

#5. Use deterministic archives for reproducibility
set(CMAKE_AR "${CMAKE_AR}" CACHE FILEPATH "" FORCE)
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcD <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")

#6. Define baseline flags, overlay will extend
set(CMAKE_C_FLAGS_INIT "-g3 -Wall -Wextra -Werror -Wno-unused-parameter -fno-common -fmessage-length=0 -MMD -MP")
set(CMAKE_CXX_FLAGS_INIT "-g3 -fdata-sections -ffunction-sections -Wall -Wno-volatile -Og -O0") #check later
set(CMAKE_ASM_FLAGS_INIT "-g3 -x assembler-with-cpp")

