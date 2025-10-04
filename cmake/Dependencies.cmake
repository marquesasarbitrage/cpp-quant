include(FetchContent)

FetchContent_Declare(
    cpp-math
    GIT_REPOSITORY ssh://git@github.com/marquesasarbitrage/cpp-math.git
    GIT_TAG main  # or a release tag
)
FetchContent_MakeAvailable(cpp-math)

FetchContent_Declare(
    cpp-datetime
    GIT_REPOSITORY ssh://git@github.com/marquesasarbitrage/cpp-datetime.git
    GIT_TAG main  # or a release tag
)
FetchContent_MakeAvailable(cpp-datetime)